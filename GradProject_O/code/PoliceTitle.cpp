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

//<======================================
//コンストラクタ
//<======================================
CPoliceTitle::CPoliceTitle() : CPolice(-1)
{
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
	//モデルの名前
	constexpr char* MODEL_NAME = "data\\MODEL\\police.x";

	//位置ありのオブジェクト生成
	m_pObj = CObjectX::Create(pos, VECTOR3_ZERO, MODEL_NAME);

	return S_OK;
}
//<======================================
//終了処理
//<======================================
void CPoliceTitle::Uninit(void)
{
	CPolice::Uninit();

	SAFE_DELETE(m_pPatrolLamp);
	SAFE_DELETE(m_pTailLamp);

}
//<======================================
//更新処理
//<======================================
void CPoliceTitle::Update(void)
{
	//唯のデバッグ用
	CDebugProc::GetInstance()->Print("座標: [ %f, %f, %f ]", 
		this->GetPosition().x, this->GetPosition().y, this->GetPosition().z);
}
//<======================================
//追いかけている処理
//<======================================
void CPoliceTitle::Chasing(const float fMoveZ)
{
	D3DXVECTOR3 PolicePos = this->GetPosition();		//警察の位置取得

	///警察の位置を移動させ、位置を設定する
	PolicePos.z += fMoveZ;
	SetPosition(PolicePos);

	//<*******************************************
	//パトランプ生成
	if (!m_pPatrolLamp)
	{
		m_pPatrolLamp = CEffekseer::GetInstance()->Create("data\\EFFEKSEER\\patrollamp.efkefc", VECTOR3_ZERO, VECTOR3_ZERO, VECTOR3_ZERO, 45.0f, false, false);
	}
	//ているらんぷ生成
	if (!m_pTailLamp)
	{
		m_pTailLamp = CEffekseer::GetInstance()->Create("data\\EFFEKSEER\\taillamp.efkefc", VECTOR3_ZERO, VECTOR3_ZERO, VECTOR3_ZERO, 45.0f, false, false);
	}
	//位置と向きを設定
	m_pPatrolLamp->m_pos = this->GetPosition();
	m_pPatrolLamp->m_rot = this->GetRotation();

	//位置と向きを設定
	m_pTailLamp->m_pos = this->GetPosition();
	m_pTailLamp->m_rot = this->GetRotation();
	//
	//<*******************************************
}
//<======================================
//生成処理
//<======================================
CPoliceTitle* CPoliceTitle::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, const D3DXVECTOR3 move)
{
	//生成用のオブジェクト
	CPoliceTitle* pPoliceTitle = DEBUG_NEW CPoliceTitle;

	//確保できていたら
	if (pPoliceTitle != nullptr)
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