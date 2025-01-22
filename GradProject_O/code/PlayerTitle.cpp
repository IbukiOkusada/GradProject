//<=================================================
//タイトルでのプレイヤ―の動き処理 [PlayerTitle.cpp]
//
//Author : Kazuki Watanabe
//<=================================================
#include "PlayerTitle.h"
#include "debugproc.h"
#include "objectX.h"
#include "TitleBaggage.h"
#include "character.h"
#include "model.h"
#include "motion.h"
#include "title.h"

//<************************************************
//名前宣言
//<************************************************
namespace
{
	//目的地の位置(ここでしか使いません)
	const D3DXVECTOR3 DEST_POS[] =
	{ 
		D3DXVECTOR3(2630.0f, 0.0f, 1054.0f),
		D3DXVECTOR3(2630.0f, 0.0f, -200.0f),
	};
}

//<================================================
//コンストラクタ
//<================================================
CPlayerTitle::CPlayerTitle()
{
	//初期化
	m_nNumDest = 0;
	m_fBDustValue = 50.0f;
	m_eState = STATE_NONE;
	m_bReached = false;
	m_bNextMove = false;
	m_bFirst = true;
	m_pTitleBaggage = nullptr;
	m_pTitleGoal = nullptr;
}
//<================================================
//デストラクタ
//<================================================
CPlayerTitle::~CPlayerTitle()
{

}
//<===============================================
//初期化処理(オーバーロード)
//<===============================================
HRESULT CPlayerTitle::Init(const char* pBodyName, const char* pLegName)
{
	constexpr char* MODEL_NAME = "data\\MODEL\\bike.x";				//モデル名
	constexpr char* SoundName = "data\\SE\\idol.wav";				//サウンド名
	const D3DXCOLOR YellowCol = D3DXCOLOR(1.0f, 1.0f, 0.3f, 1.0f);	//黄色
	CModel* pParts = nullptr;										//モデルのパーツ取得用変数

	//コンテナだけいらないのでこのような形にしました
	m_pObj = CObjectX::Create(D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), MODEL_NAME);
	m_pObj->SetType(CObject::TYPE_PLAYER);
	m_pObj->SetRotateType(CObjectX::TYPE_QUATERNION);
	SetMatrix();
	SetCol();

	//バイカー生成
	m_pCharacter = CCharacter::Create("data\\TXT\\character\\player\\motion_player.txt");
	m_pCharacter->SetParent(m_pObj->GetMtx());
	m_pCharacter->GetMotion()->InitSet(0);
	m_pCharacter->SetScale(D3DXVECTOR3(3.0f, 3.0f, 3.0f));
	m_pCharacter->SetPosition(D3DXVECTOR3(0.0f, -60.0f, 75.0f));
	m_pCharacter->SetRotation(D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	//バイカーの色設定
	for (int nCntParts = 0; nCntParts < m_pCharacter->GetNumParts(); nCntParts++)
	{
		//モデルのパーツを取得
		pParts = m_pCharacter->GetParts(nCntParts);
		pParts->SetColMulti(YellowCol);
	}

	//サウンド生成
	m_pSound = CMasterSound::CObjectSound::Create(SoundName, -1);
	m_pSound->SetVolume(1.0f);
	m_pSound->Stop();

	//荷物生成
	m_pTitleBaggage = CTitleBaggage::Create(this->GetPosition());
	m_pTitleBaggage->GetObj()->SetScale(D3DXVECTOR3(2.0f, 2.0f, 2.0f));

	//エフェクト生成
	m_pAfterburner = CEffekseer::GetInstance()->Create("data\\EFFEKSEER\\afterburner.efkefc", VECTOR3_ZERO, VECTOR3_ZERO, VECTOR3_ZERO, 45.0f, false, false);
	m_pTailLamp = CEffekseer::GetInstance()->Create("data\\EFFEKSEER\\trail.efkefc", VECTOR3_ZERO, VECTOR3_ZERO, VECTOR3_ZERO, 10.0f, false, false);
	m_pBackdust = CEffekseer::GetInstance()->Create("data\\EFFEKSEER\\backdust.efkefc", VECTOR3_ZERO, VECTOR3_ZERO, VECTOR3_ZERO, 45.0f, false, false);

	return S_OK;
}
//<================================================
//終了処理
//<================================================
void CPlayerTitle::Uninit(void)
{
	//終了処理
	CPlayer::Uninit();
	SAFE_UNINIT_DELETE(m_pSound);
	SAFE_UNINIT(m_pTitleBaggage);
	SAFE_UNINIT(m_pTitleGoal);
}
//<================================================
//更新処理
//<================================================
void CPlayerTitle::Update(void)
{
	//デバッグ以外だったら
	if (m_eState == STATE_NONE)
	{
		// マトリックス
		SetMatrix();

		//オブジェクト自体の中身があれば
		if (m_pObj)
		{
			//設定
			SetPosition(GetPosition());
			SetRotation(GetRotation());
			m_pObj->SetPosition(GetPosition());
			m_pObj->SetRotation(GetRotation());
			m_pObj->SetShadowHeight(GetPosition().y);

			//バイカーの更新
			m_pCharacter->Update();
		}
		//次の動きに移行していなかったら、アイスをプレイヤーの上に乗っける
		if (!m_bNextMove) 
		{
			m_pTitleBaggage->GetObj()->SetParent(m_pCharacter->GetParts(5)->GetMtx());
		}

		//移行していたら初期位置に戻す
		else 
		{ 
			constexpr float BDustMaxValue = 300.0f;		//後ろから出る煙の最大値
			m_pTitleBaggage->SetPosition(VECTOR3_ZERO); 
			m_fBDustValue = BDustMaxValue;
		}

		//<********************************
		//エフェクトを出す
		//<********************************
		m_pTailLamp->m_pos = GetPosition();
		m_pTailLamp->m_rot = GetRotation();
		m_pBackdust->m_pos = GetPosition();
		m_pBackdust->m_rot = GetRotation();
		m_pBackdust->m_Scale = VECTOR3_ONE * m_fBDustValue;
		m_pAfterburner->m_pos = GetPosition();
		m_pAfterburner->m_Scale = VECTOR3_ONE * m_fBrake * m_fBDustValue;
	}
	//デバッグだったら
	else if (m_eState == STATE_DEBUG)
	{
		//行動モード
		SetType(CPlayer::TYPE::TYPE_ACTIVE);
		CPlayer::Update();
	}

	correction::Adjust(&m_Info.rot);

	//デバッグ表示
	CDebugProc::GetInstance()->Print("プレイヤー座標: [ %f, %f, %f ]\n", m_Info.pos.x, m_Info.pos.y, m_Info.pos.z);
	CDebugProc::GetInstance()->Print("プレイヤー向き: [ %f, %f, %f ]\n", m_Info.rot.x, m_Info.rot.y, m_Info.rot.z);
}
//<================================================
//動きに関する処理
//<================================================
void CPlayerTitle::Moving(const int nNum)
{
	constexpr float DEST_DIFF = 5.0f;										//距離の差
	constexpr float fSpeed = 0.075f;										//速さ
	D3DXVECTOR3 rPos = GetPosition();										//位置
	D3DXVECTOR3 rMove = GetMove();

	//正規状態をなくす
	if (m_bReached) { m_bReached = false; }

	//デバッグ以外だったら
	if (m_eState == STATE_NONE)
	{
		//目的地まで移動
		rPos.x += (DEST_POS[nNum].x - rPos.x) * fSpeed;//X軸
		rPos.z += (DEST_POS[nNum].z - rPos.z) * fSpeed;//Z軸

		//目的地に到着したら判定をtrueにする
		if (Function::BoolDis(rPos, DEST_POS[nNum], DEST_DIFF)) { m_bReached = true; }
	}

	//位置設定
	SetPosition(rPos);
}
//<================================================
//荷物の動き処理
//<================================================
void CPlayerTitle::BaggageMove(void)
{
	//次の動きに移行していなかったら
	if (!m_bNextMove)
	{
		constexpr float fDis = -200.0f;		//距離
		m_bNextMove = true;					//次の行動に移動していますということを証明

		//位置を設定
		D3DXVECTOR3 GoalPos = D3DXVECTOR3(this->GetPosition().x + fDis, this->GetPosition().y, this->GetPosition().z);

		//ゴールをプレイヤーの横に設定する
		m_pTitleGoal = CTitleGoal::Create(GoalPos, D3DXVECTOR3(0.0f, -1.56f, 0.0f));

		//人間と荷物の設定
		m_pTitleBaggage->GetObj()->SetParent(m_pTitleGoal->GetCHR()->GetParts(6)->GetMtx());
		m_pTitleGoal->GetCHR()->GetMotion()->BlendSet(4);
	}
}
//<================================================
//選択画面の際の動き
//<================================================
void CPlayerTitle::MovingSelect(void)
{
	constexpr float fMove = 0.12f;					//移動速度と回転速度
	const float fRad1 = 500.0f,fRad2 = 1000.0f;		//範囲

	//目的地に到着したら判定をtrueにする
	if (Function::BoolDis(GetPosition(), DEST_POS_SELECT[m_nNumDest], fRad1))
	{
		//目的地変更
		m_nNumDest = (m_nNumDest + 1) % DEST_MAX;
	}
	
	PlayerRotSet();

	//デバッグ表示
	CDebugProc::GetInstance()->Print("[現在の番号]：%d\n", m_nNumDest);
}
//<================================================
//プレイヤーの位置向き設定
//<================================================
void CPlayerTitle::PlayerRotSet(void)
{
	//位置情報など
	constexpr float fMove = 50.0f;									//移動速度
	D3DXVECTOR3 rDestRot = VECTOR3_ZERO;							//目的向き
	D3DXVECTOR3 rRot = GetRotation();								//向き
	D3DXVECTOR3 rPos = GetPosition();								//位置

	//目的向き設定
	rDestRot = DEST_ROT_SELECT[m_nNumDest];

	//番号によって変更させる
	switch (m_nNumDest)
	{
	case DEST::DEST_FIRST:							rPos.z += fMove; break;			//最初の番号
	case DEST::DEST_SECOND:case DEST::DEST_FOUTH:	rPos.x -= fMove; break;			//二番目か四番目
	case DEST::DEST_THIRD: case DEST::DEST_FIFTH:	rPos.z -= fMove; break;			//三番目か五番目
	case DEST::DEST_SIXTH:							rPos.x += fMove; break;			//最後の番号
	}

	//位置を設定
	SetPosition(rPos);

	//<**********************************************
	//回転関連
	float fDiff = rDestRot.y - rRot.y;	//差分
	float fIner = 0.3f;							//慣性

	correction::Adjust(&fDiff);

	rRot.y += fDiff * fIner;

	//向き調整と設定
	SetRotation(rRot);
	//<**********************************************
}
//<================================================
//生成処理
//<================================================
CPlayerTitle* CPlayerTitle::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, const D3DXVECTOR3 move
, const char* pBodyName, const char* pLegName)
{
	//生成のためのオブジェクト
	CPlayerTitle* pPlayertitle = DEBUG_NEW CPlayerTitle;

	//中身があったら
	if (pPlayertitle)
	{
		// 初期化処理
		pPlayertitle->Init(pBodyName, pLegName);

		// 座標設定
		pPlayertitle->SetPosition(pos);

		// 向き設定
		pPlayertitle->SetRotation(rot);

		//いるかは分からない
		pPlayertitle->m_fRotDest = rot.y;

		// 移動量設定
		pPlayertitle->SetMove(move);
	}
	//無かった場合、中身なしを返す
	else
	{
		return nullptr;
	}

	//その情報を返す
	return pPlayertitle;
}
//<================================================
//目的地に到着したかの判定
//<================================================
bool Function::BoolToDest(const D3DXVECTOR3 Pos, const D3DXVECTOR3 DestPos, const float Distance, bool bZuse)
{
	//Z軸を使うなら
	if (bZuse)
	{
		//目的の位置についたら
		if (Pos.x >= DestPos.x + Distance && Pos.x <= DestPos.x - Distance
			|| Pos.x <= DestPos.x + Distance && Pos.x >= DestPos.x - Distance
			&& Pos.z >= DestPos.z + Distance && Pos.z <= DestPos.z - Distance
			|| Pos.z <= DestPos.z + Distance && Pos.z >= DestPos.z - Distance)
		{
			return true;
		}

	}
	//Z軸を使わないなら
	else
	{
		//目的の位置についたら
		if (Pos.x >= DestPos.x + Distance && Pos.x <= DestPos.x - Distance
			|| Pos.x <= DestPos.x + Distance && Pos.x >= DestPos.x - Distance)
		{
			return true;
		}

	}

	return false;
}
//<================================================
//目的地に到着したかの判定
//<================================================
bool Function::BoolDis(const D3DXVECTOR3 Pos, const D3DXVECTOR3 DestPos, const float Distance)
{
	//距離計算
	const D3DXVECTOR3 rDis = D3DXVECTOR3(Pos.x - DestPos.x, Pos.y - DestPos.y, Pos.z - DestPos.z);

	//もし近づいていたら
	if (rDis.x <= Distance &&
		rDis.z <= Distance &&
		!(-rDis.x >= Distance) &&
		!(-rDis.z >= Distance))
	{
		return true;
	}

	return false;

}