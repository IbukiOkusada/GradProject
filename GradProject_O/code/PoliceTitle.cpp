//<=================================================
//タイトルでの警察の動き処理 [PlayerTitle.h]
//
//Author : Kazuki Watanabe
//<=================================================
#include "PoliceTitle.h"
#include "police_manager.h"
#include "debugproc.h"
#include "manager.h"
#include "title.h"
#include "car_manager.h"
#include "PlayerTitle.h"

//サウンド再生
CMasterSound::CObjectSound* CPoliceTitle::m_pSound = nullptr;

int CPoliceTitle::m_nNumAll = 0;
 
//<======================================
//コンストラクタ
//<======================================
CPoliceTitle::CPoliceTitle() : CPolice(-1)
{
	//初期化
	m_pPatrolLamp = nullptr;
	m_pTailLamp = nullptr;
	m_nNumDest = 0;
	m_nNumAll++;
}

//<======================================
//デストラクタ
//<======================================
CPoliceTitle::~CPoliceTitle()
{
	m_nNumAll--;
}
//<======================================
//初期化処理(位置情報あり)
//<======================================
HRESULT CPoliceTitle::Init(void)
{
	constexpr char* MODEL_NAME = "data\\MODEL\\police.x";	//モデル名
	constexpr char* SoundName = "data\\SE\\siren.wav";		//サウンド名
	constexpr float fDis = 1000.0f;							//プレイヤーからの距離
	constexpr float fDis_Police = 350.0f;
	constexpr float fDiff = 150.0f;							//プレイヤーからの距離
	
	
	////それぞれの座標設定
	//rPos.x = aPos.x - fDiff + fDis_Police * nCnt;
	//rPos.y = aPos.y;
	//rPos.z = aPos.z - fDis;

	//位置設定
	D3DXVECTOR3 rPos = D3DXVECTOR3(
		CTitle::GetPlayer()->GetPosition().x,
		CTitle::GetPlayer()->GetPosition().y,
		CTitle::GetPlayer()->GetPosition().z - fDis * m_nNumAll);

	//音生成
	if (!m_pSound)
	{
		//生成開始
		m_pSound = CMasterSound::CObjectSound::Create(SoundName, -1);
		m_pSound->SetVolume(0.2f);
		m_pSound->Stop();
	}

	//位置ありのオブジェクト生成
	m_pObj = CObjectX::Create(rPos, VECTOR3_ZERO, MODEL_NAME);


	SetPosition(rPos);

	return S_OK;
}
//<======================================
//終了処理
//<======================================
void CPoliceTitle::Uninit(void)
{
	//終了処理
	CPolice::Uninit();
	SAFE_UNINIT_DELETE(m_pSound);
	SAFE_DELETE(m_pPatrolLamp);
	SAFE_DELETE(m_pTailLamp);
}
//<======================================
//追いかけている処理
//<======================================
void CPoliceTitle::Chasing(const float fMoveZ)
{
	D3DXVECTOR3 PolicePos = this->GetPosition();		//警察の位置取得

	//エフェクト生成
	SettingPatLamp();

	///警察の位置を移動させ、位置を設定する
	PolicePos.z += fMoveZ;
	SetPosition(PolicePos);
}
//<======================================
//タイトルの動き処理
//<======================================
void CPoliceTitle::TitleMove(void)
{
	const float fRad1 = 500.0f;		//範囲

	SettingPatLamp();

	//目的地に到着したら判定をtrueにする
	if (Function::BoolDis(GetPosition(), DEST_POS_SELECT[m_nNumDest], fRad1))
	{
		//目的地変更
		m_nNumDest = (m_nNumDest + 1) % DEST_MAX;
	}

	Move();
}
//<======================================
//動き処理
//<======================================
void CPoliceTitle::Move(void)
{
	D3DXVECTOR3 rDestRot = VECTOR3_ZERO;
	D3DXVECTOR3 rRot = GetRotation();
	D3DXVECTOR3 rPos = GetPosition();
	constexpr float fMove = 50.0f;

	//番号によって変更させる
	switch (m_nNumDest)
	{
		//最初の目的地
	case DEST::DEST_FIRST:

		//最初の向きにする
		rDestRot = DEST_ROT_SELECT[DEST::DEST_FIRST];

		//移動
		rPos.z += fMove;

		break;

		//二番目か四番目
	case DEST::DEST_SECOND:
	case DEST::DEST_FOUTH:

		//二番目の向きにする
		rDestRot = DEST_ROT_SELECT[DEST::DEST_SECOND];

		//移動
		rPos.x -= fMove;

		break;

		//三番目か五番目
	case DEST::DEST_THIRD:
	case DEST::DEST_FIFTH:

		//三番目の向きにする
		rDestRot = DEST_ROT_SELECT[DEST::DEST_THIRD];

		//移動
		rPos.z -= fMove;

		break;

		//最後の番号
	case DEST::DEST_SIXTH:

		//最終地点の向きにする
		rDestRot = DEST_ROT_SELECT[DEST::DEST_SIXTH];

		//移動
		rPos.x += fMove;

		break;
	}

	//位置を設定
	SetPosition(rPos);
	//<****************************************
	//向き回転関連
	float fDiff = rDestRot.y - rRot.y;	//差分
	float fIner = 0.3f;					//慣性の動き

	//調整
	Adjust(fDiff);

	//回転させる
	rRot.y += fDiff * fIner;

	//向き調整と設定
	SetRotation(rRot);
	//<****************************************

}
//<======================================
//生成処理
//<======================================
CPoliceTitle* CPoliceTitle::Create(const D3DXVECTOR3 rot, const D3DXVECTOR3 move)
{
	//生成用のオブジェクト
	CPoliceTitle* pPoliceTitle = DEBUG_NEW CPoliceTitle;

	//確保できていたら
	if (pPoliceTitle)
	{
		//初期化処理
		pPoliceTitle->Init();

		//位置設定
		pPoliceTitle->SetPosition(pPoliceTitle->GetPosition());

		//向き設定
		pPoliceTitle->SetRotation(rot);

		//移動量設定
		pPoliceTitle->SetMove(move);
	}
	//確保できていなかったら
	else { return nullptr; }

	return pPoliceTitle;
}