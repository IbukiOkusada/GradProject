//<=================================================
//タイトルでのプレイヤ―の動き処理 [PlayerTitle.cpp]
//
//Author : Kazuki Watanabe
//<=================================================
#include "PlayerTitle.h"
#include "camera.h"
#include "camera_manager.h"
#include "debugproc.h"

//<************************************************
//名前宣言
//<************************************************
namespace
{
	//目的地の位置
	const D3DXVECTOR3 DEST_POS[] = 
	{ D3DXVECTOR3(-4734.0f, 0.0f, 1054.0f), 
		D3DXVECTOR3(-4734.0f, 0.0f, -200.0f),
		D3DXVECTOR3(-4734.0f, 0.0f, -800.0f),
		D3DXVECTOR3(-4734.0f, 0.0f, -800.0f),
		D3DXVECTOR3(-4734.0f, 0.0f, -800.0f),
	
	};

	const float DEST_DIFF = 5.0f;										//距離の差
}

//<================================================
//コンストラクタ
//<================================================
CPlayerTitle::CPlayerTitle()
{
	m_eState = STATE_NONE;
	m_bReached = false;
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
	//コンテナだけいらないのでこのような形にしました
	m_pObj = CObjectX::Create(D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), "data\\MODEL\\flyingscooter.x");
	m_pObj->SetType(CObject::TYPE_PLAYER);
	m_pObj->SetRotateType(CObjectX::TYPE_QUATERNION);
	SetMatrix();
	m_pSound = CMasterSound::CObjectSound::Create("data\\SE\\idol.wav", -1);
	m_pSoundBrake = CMasterSound::CObjectSound::Create("data\\SE\\flight.wav", -1);
	m_pSoundBrake->SetVolume(0.0f);
	pRadio = CRadio::Create();
	m_pNavi = CNavi::Create();

	return S_OK;
}
//<================================================
//終了処理
//<================================================
void CPlayerTitle::Uninit(void)
{
	CPlayer::Uninit();
}
//<================================================
//更新処理
//<================================================
void CPlayerTitle::Update(void)
{
	//デバッグ以外だったら
	if (m_eState == STATE_NONE)
	{
		// 前回の座標を取得
		m_Info.posOld = GetPosition();

		StateSet();
		pRadio->Update();

		// マトリックス
		SetMatrix();

		// 当たり判定
		Collision();

		//オブジェクト自体の中身があれば
		if (m_pObj != nullptr)
		{
			//設定
			SetPosition(GetPosition());
			SetRotation(GetRotation());
			m_pObj->SetPosition(GetPosition());
			m_pObj->SetRotation(GetRotation());
			m_pObj->SetShadowHeight(GetPosition().y);
		}

		// 荷物を所持
		if (m_pBaggage != nullptr)
		{
			D3DXVECTOR3 rot = GetRotation();
			rot.y -= D3DX_PI * 0.5f;
			//CCamera* pCamera = CCameraManager::GetInstance()->GetTop();
		}
	}
	//デバッグだったら
	else if (m_eState == STATE_DEBUG)
	{
		//行動モード
		SetType(CPlayer::TYPE::TYPE_ACTIVE);
		CPlayer::Update();
	}

	//// デバッグ表示
	//CDebugProc::GetInstance()->Print("プレイヤー :");
	//CDebugProc::GetInstance()->Print("座標: [ %f, %f, %f ]", m_Info.pos.x, m_Info.pos.y, m_Info.pos.z);
	CDebugProc::GetInstance()->Print(" プレイヤー向き: [ %f, %f, %f ]\n", m_pObj->GetRotation().x, m_pObj->GetRotation().y, m_pObj->GetRotation().z);
}
//<================================================
//動きに関する処理
//<================================================
void CPlayerTitle::Moving(const int nNum)
{
	//正規状態をなくす
	if (m_bReached) { m_bReached = false; }

	//デバッグ以外だったら
	if (m_eState == STATE_NONE)
	{
		//目的地まで移動
		m_Info.pos.x += (DEST_POS[nNum].x - m_Info.pos.x - m_Info.move.x) * 0.075f;//X軸
		m_Info.pos.z += (DEST_POS[nNum].z - m_Info.pos.z - m_Info.move.z) * 0.075f;//Z軸

		//目的地に到着したら判定をtrueにする
		if (Function::BoolToDest(m_Info.pos, DEST_POS[nNum], DEST_DIFF)) { m_bReached = true; }
	}
	//到着していなかったら
	if (!m_bReached)
	{
		//<********************************
		//エフェクトを出す
		//<********************************
		m_pTailLamp->m_pos = GetPosition();
		m_pTailLamp->m_rot = GetRotation();
		m_pBackdust->m_pos = GetPosition();
		m_pBackdust->m_rot = m_pObj->GetRotation();
		m_pBackdust->m_Scale = VECTOR3_ONE * m_fEngine * 300.0f;
		m_pAfterburner->m_pos = GetPosition();
		m_pAfterburner->m_Scale = VECTOR3_ONE * m_fEngine * m_fBrake * 150.0f;
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

		//動かない状態にする
		pPlayertitle->SetType(CPlayer::TYPE::TYPE_NONE);
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