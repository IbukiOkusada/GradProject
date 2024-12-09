//<=================================================
//タイトルでのプレイヤ―の動き処理 [PlayerTitle.cpp]
//
//Author : Kazuki Watanabe
//<=================================================
#include "PlayerTitle.h"
#include "camera.h"
#include "camera_manager.h"
#include "debugproc.h"
#include "objectX.h"
#include "navi.h"
#include "TitleBaggage.h"
#include "character.h"
#include "model.h"
#include "motion.h"
#include "PoliceTitle.h"
#include "title.h"

//<************************************************
//名前宣言
//<************************************************
namespace
{
	//目的地の位置
	const D3DXVECTOR3 DEST_POS[CPlayerTitle::DEST::DEST_MAX] =
	{ D3DXVECTOR3(2630.0f, 0.0f, 1054.0f),
		D3DXVECTOR3(2630.0f, 0.0f, -200.0f),
		D3DXVECTOR3(-4734.0f, 0.0f, -800.0f),
		D3DXVECTOR3(-4734.0f, 0.0f, -800.0f),
		D3DXVECTOR3(-4734.0f, 0.0f, -800.0f),
		D3DXVECTOR3(-4734.0f, 0.0f, -800.0f)
	
	};

	//目的地の位置
	const D3DXVECTOR3 DEST_POS_SELECT[CPlayerTitle::DEST::DEST_MAX]=
	{
		D3DXVECTOR3(2700.0f, 0.0f, 6000.0f),		//１個目
		D3DXVECTOR3(-3160.0f, 0.0f, 6000.0f),		//２個目
		D3DXVECTOR3(-3160.0f, 0.0f, 4000.0f),		//３個目
		D3DXVECTOR3(-4750.0f, 0.0f, 4000.0f),		//４個目
		D3DXVECTOR3(-4750.0f, 0.0f, -200.0f),		//５個目
		D3DXVECTOR3(2700.0f, 0.0f, -200.0f)			//６個目
	};
	//目的の向き
	const D3DXVECTOR3 DEST_ROT_SELECT[CPlayerTitle::DEST::DEST_MAX] =
	{
		D3DXVECTOR3(0.0f,-3.14f,0.0f),				//１個目
		D3DXVECTOR3(0.0f,1.57f,0.0f),				//２個目
		D3DXVECTOR3(0.0f,0.0f,0.0f),				//３個目
		D3DXVECTOR3(0.0f,1.57f,0.0f),				//４個目
		D3DXVECTOR3(0.0f,0.0f,0.0f),				//５個目
		D3DXVECTOR3(0.0f,-1.57f,0.0f)				//６個目
	};
}

//<================================================
//コンストラクタ
//<================================================
CPlayerTitle::CPlayerTitle()
{
	//初期化
	//<*************************************
	//int型
	m_nNumDest = 0;
	//<*************************************
	//float型
	m_fDestrot = 0.0f;
	m_fMoveRot = 0.0f;
	m_fBDustValue = 50.0f;
	//<*************************************
	//その他
	m_eState = STATE_NONE;
	m_bReached = false;
	m_bNextMove = false;
	m_bMoved = false;
	m_bFirst = true;
	m_pTitleBaggage = nullptr;
	m_pTitleGoal = nullptr;
	//<*************************************
}
//<================================================
//デストラクタ
//<================================================
CPlayerTitle::~CPlayerTitle()
{

}
//<================================================
//初期化処理
//<================================================
HRESULT CPlayerTitle::Init(void)
{
	CPlayer::Init();

	return S_OK;
}
//<===============================================
//初期化処理(オーバーロード)
//<===============================================
HRESULT CPlayerTitle::Init(const char* pBodyName, const char* pLegName)
{
	constexpr char* MODEL_NAME = "data\\MODEL\\bike.x";		//モデル名
	constexpr char* SoundName = "data\\SE\\idol.wav";		//サウンド名

	//コンテナだけいらないのでこのような形にしました
	m_pObj = CObjectX::Create(D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), MODEL_NAME);
	m_pObj->SetType(CObject::TYPE_PLAYER);
	m_pObj->SetRotateType(CObjectX::TYPE_QUATERNION);
	SetMatrix();
	SetMatrix();
	m_pNavi = CNavi::Create();

	//サウンド生成
	m_pSound = CMasterSound::CObjectSound::Create(SoundName, -1);
	m_pSound->SetVolume(1.0f);
	m_pSound->Stop();

	//荷物生成
	m_pTitleBaggage = CTitleBaggage::Create(this->GetPosition());

	//エフェクト生成
	m_pAfterburner = CEffekseer::GetInstance()->Create("data\\EFFEKSEER\\afterburner.efkefc", VECTOR3_ZERO, VECTOR3_ZERO, VECTOR3_ZERO, 45.0f, false, false);
	m_pTailLamp = CEffekseer::GetInstance()->Create("data\\EFFEKSEER\\taillamp.efkefc", VECTOR3_ZERO, VECTOR3_ZERO, VECTOR3_ZERO, 45.0f, false, false);
	m_pBackdust = CEffekseer::GetInstance()->Create("data\\EFFEKSEER\\backdust.efkefc", VECTOR3_ZERO, VECTOR3_ZERO, VECTOR3_ZERO, 45.0f, false, false);

	return S_OK;
}
//<================================================
//終了処理
//<================================================
void CPlayerTitle::Uninit(void)
{
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
	constexpr float BDustMaxValue = 300.0f;									//後ろから出る煙の最大値

	//デバッグ以外だったら
	if (m_eState == STATE_NONE)
	{
		// 前回の座標を取得
		m_Info.posOld = GetPosition();

		StateSet();

		// マトリックス
		SetMatrix();

		// 当たり判定
		Collision();

		//オブジェクト自体の中身があれば
		if (m_pObj)
		{
			//設定
			SetPosition(GetPosition());
			SetRotation(GetRotation());
			m_pObj->SetPosition(GetPosition());
			m_pObj->SetRotation(GetRotation());
			m_pObj->SetShadowHeight(GetPosition().y);
		}
		//次の動きに移行していなかったら、アイスをプレイヤーの上に乗っける
		if (!m_bNextMove) { m_pTitleBaggage->SetPosition(GetPosition()); }

		//移行していたら初期位置に戻す
		else { m_pTitleBaggage->SetPosition(VECTOR3_ZERO); m_fBDustValue = BDustMaxValue; }

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

	//正規状態をなくす
	if (m_bReached) { m_bReached = false; }

	//デバッグ以外だったら
	if (m_eState == STATE_NONE)
	{
		//目的地まで移動
		m_Info.pos.x += (DEST_POS[nNum].x - m_Info.pos.x - m_Info.move.x) * 0.075f;//X軸
		m_Info.pos.z += (DEST_POS[nNum].z - m_Info.pos.z - m_Info.move.z) * 0.075f;//Z軸

		//目的地に到着したら判定をtrueにする
		if (Function::BoolDis(m_Info.pos, DEST_POS[nNum], DEST_DIFF)) { m_bReached = true; }
	}
}
//<================================================
//選択画面の際の動き
//<================================================
void CPlayerTitle::MovingSelect(void)
{
	constexpr float fMove = 0.12f;	//移動速度と回転速度
	const float fRad1 = 95.0f,fRad2 = 500.0f;		//範囲

	//目的地に到着したら判定をtrueにする
	if (Function::BoolDis(GetPosition(), DEST_POS_SELECT[m_nNumDest], fRad1))
	{
		//目的地変更
		m_nNumDest = (m_nNumDest + 1) % DEST_MAX;
	}
	//判定初期化用
	if (m_nNumDest >= DEST_SIXTH){m_bFirst = false;}
	
	PoliceRotSet();

	//デバッグ表示
	CDebugProc::GetInstance()->Print("[現在の番号]：%d\n", m_nNumDest);
}
//<================================================
//荷物の動き処理
//<================================================
void CPlayerTitle::BaggageMove(void)
{
	constexpr float fDis = -200.0f;
	D3DXVECTOR3 GoalPos = VECTOR3_ZERO;

	//次の動きに移行していなかったら
	if (!m_bNextMove)
	{
		//設定
		m_bNextMove = true;

		//位置を設定
		GoalPos = D3DXVECTOR3(this->GetPosition().x + fDis, this->GetPosition().y, this->GetPosition().z);

		//ゴールをプレイヤーの横に設定する
		m_pTitleGoal = CTitleGoal::Create(GoalPos,D3DXVECTOR3(0.0f,-1.56f,0.0f));

		//人間と荷物の設定
		m_pTitleBaggage->GetObj()->SetParent(m_pTitleGoal->GetCHR()->GetParts(6)->GetMtx());
		m_pTitleGoal->GetCHR()->GetMotion()->BlendSet(4);
	}
}
//<================================================
//警察の位置設定
//<================================================
void CPlayerTitle::PoliceRotSet(void)
{
	CPoliceTitle* apPolice[INITIAL::POLICE_MAX] = { nullptr };	//警察のポインタ
	D3DXVECTOR3 rRot = VECTOR3_ZERO;							//向き
	float fMove = 125.0f;										//移動速度
	constexpr float fRotation = 0.3f;							//回転速度

	//警察の情報を取得してくる
	for (int nCnt = 0; nCnt < INITIAL::POLICE_MAX; nCnt++)
	{
		apPolice[nCnt] = CTitle::GetPoliTitle(nCnt);
	}

	//番号によって変更させる
	switch (m_nNumDest)
	{
		//最初の目的地
	case DEST::DEST_FIRST:

		//最初の向きにする
		rRot = DEST_ROT_SELECT[DEST::DEST_FIRST];

		//移動
		m_Info.pos.z += fMove;

		break;

		//二番目か四番目
	case DEST::DEST_SECOND:
	case DEST::DEST_FOUTH:

		//二番目の向きにする
		rRot = DEST_ROT_SELECT[DEST::DEST_SECOND];

		//移動
		m_Info.pos.x -= fMove;

		break;

		//三番目か五番目
	case DEST::DEST_THIRD:
	case DEST::DEST_FIFTH:

		//三番目の向きにする
		rRot = DEST_ROT_SELECT[DEST::DEST_THIRD];

		//移動
		m_Info.pos.z -= fMove;

		break;

		//最後の番号
	case DEST::DEST_SIXTH:

		//最終地点の向きにする
		rRot = DEST_ROT_SELECT[DEST::DEST_SIXTH];

		//移動
		m_Info.pos.x += fMove;


		break;
	}

	//警察の向きを設定
	for (int nCnt = 0; nCnt < INITIAL::POLICE_MAX; nCnt++)
	{
		apPolice[nCnt]->SetRotation(rRot);
	}

	//回転向きを設定
	m_fDestrot = rRot.y;

	//四番目以外だったら
	if (m_nNumDest != DEST::DEST_FOUTH)
	{
		//二番目の目的地且つまだ判定されてなかったら、判定と向きを設定
		if (m_nNumDest == DEST::DEST_SECOND && !m_bFirst) { m_Info.rot.y = 3.14f; m_bFirst = true; }

		//移動速度設定
		m_fMoveRot = -fRotation;

		//プレイヤー回転
		m_Info.rot.y += m_fMoveRot;

		//小さかったら
		if (m_Info.rot.y <= m_fDestrot) { m_Info.rot.y = m_fDestrot; }
	}
	//四番目だったら
	else
	{
		//移動速度設定
		m_fMoveRot = fRotation;

		//プレイヤー回転
		m_Info.rot.y += m_fMoveRot;

		//小さかったら
		if (m_Info.rot.y >= m_fDestrot) { m_Info.rot.y = m_fDestrot; }
	}

	//調整
	Adjust(m_Info.rot.y);

	//その向きに設定
	SetRotation(m_Info.rot);

	PolicePosSet();
}
//<================================================
//警察の位置設定
//<================================================
void CPlayerTitle::PolicePosSet(void)
{
	CPoliceTitle* apPolice[INITIAL::POLICE_MAX] = { nullptr };	//警察ポインタ
	D3DXVECTOR3 arPos[INITIAL::POLICE_MAX] = { VECTOR3_ZERO };	//警察の位置
	constexpr float fDis = 650.0f;								//プレイヤーからの距離
	constexpr float fDis_Police = 350.0f;						//警察間の距離
	constexpr float fDiff = 250.0f;								//補正(警察用)

	const D3DXVECTOR3 aPos = GetPosition();						//プレイヤー位置

	//警察の情報を取得してくる
	for (int nCnt = 0; nCnt < INITIAL::POLICE_MAX; nCnt++)
	{
		//情報取得+位置情報取得
		apPolice[nCnt] = CTitle::GetPoliTitle(nCnt);
		arPos[nCnt] = apPolice[nCnt]->GetPosition();
	}

	//警察の情報を取得してくる
	for (int nCnt = 0; nCnt < INITIAL::POLICE_MAX; nCnt++)
	{
		//パトランプ
		apPolice[nCnt]->SettingPatLamp();

		//番号によって変更させる
		switch (m_nNumDest)
		{
			//最初の目的地
		case DEST::DEST_FIRST:

			//その位置に設定
			arPos[nCnt] = D3DXVECTOR3(aPos.x - fDiff + fDis_Police *nCnt, aPos.y, aPos.z - fDis);

			break;

			//二番目か四番目
		case DEST::DEST_SECOND:
		case DEST::DEST_FOUTH:

			//その位置に設定
			arPos[nCnt] = D3DXVECTOR3(aPos.x + fDis, aPos.y, aPos.z - fDiff + fDis_Police * nCnt);

			break;

			//三番目か五番目
		case DEST::DEST_THIRD:
		case DEST::DEST_FIFTH:

			//その位置に設定
			arPos[nCnt] = D3DXVECTOR3(aPos.x - fDiff + fDis_Police * nCnt, aPos.y, GetPosition().z + fDis);

			break;

			//最後の番号
		case DEST::DEST_SIXTH:

			//その位置に設定
			arPos[nCnt] = D3DXVECTOR3(aPos.x - fDis, aPos.y, aPos.z - fDiff + fDis_Police * nCnt);

			break;
		}
		//位置と向きを設定
		apPolice[nCnt]->SetPosition(arPos[nCnt]);
	}
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