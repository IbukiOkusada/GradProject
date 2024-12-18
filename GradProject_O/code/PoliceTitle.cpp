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

//サウンド再生
CMasterSound::CObjectSound* CPoliceTitle::m_pSound = nullptr;
 
//<======================================
//コンストラクタ
//<======================================
CPoliceTitle::CPoliceTitle() : CPolice(-1)
{
	//初期化
	m_pPatrolLamp = nullptr;
	m_pTailLamp = nullptr;
}

//<======================================
//デストラクタ
//<======================================
CPoliceTitle::~CPoliceTitle()
{

}
//<======================================
//初期化処理(位置情報あり)
//<======================================
HRESULT CPoliceTitle::Init(const D3DXVECTOR3 pos)
{
	constexpr char* MODEL_NAME = "data\\MODEL\\police.x";	//モデル名
	constexpr char* SoundName = "data\\SE\\siren.wav";		//サウンド名

	//位置ありのオブジェクト生成
	m_pObj = CObjectX::Create(pos, VECTOR3_ZERO, MODEL_NAME);

	//音生成
	if (!m_pSound)
	{
		//生成開始
		m_pSound = CMasterSound::CObjectSound::Create(SoundName, -1);
		m_pSound->SetVolume(0.2f);
		m_pSound->Stop();
	}

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
//生成処理
//<======================================
CPoliceTitle* CPoliceTitle::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, const D3DXVECTOR3 move)
{
	//生成用のオブジェクト
	CPoliceTitle* pPoliceTitle = DEBUG_NEW CPoliceTitle;

	//確保できていたら
	if (pPoliceTitle)
	{
		//初期化処理
		pPoliceTitle->Init(pos);

		//座標設定
		pPoliceTitle->SetPosition(pos);

		//向き設定
		pPoliceTitle->SetRotation(rot);

		//移動量設定
		pPoliceTitle->SetMove(move);
	}
	//確保できていなかったら
	else { return nullptr; }

	return pPoliceTitle;
}