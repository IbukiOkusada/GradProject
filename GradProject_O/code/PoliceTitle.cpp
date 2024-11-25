//<=================================================
//タイトルでの警察の動き処理 [PlayerTitle.h]
//
//Author : Kazuki Watanabe
//<=================================================
#include "PoliceTitle.h"
#include "debugproc.h"

//<======================================
//コンストラクタ
//<======================================
CPoliceTitle::CPoliceTitle()
{
	
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
	//位置情報を設定
	//CPolice::Init(pos);

	return S_OK;
}
//<======================================
//終了処理
//<======================================
void CPoliceTitle::Uninit(void)
{
	CPolice::Uninit();
}
//<======================================
//更新処理
//<======================================
void CPoliceTitle::Update(void)
{
	CDebugProc::GetInstance()->Print("座標: [ %f, %f, %f ]", 
		this->GetPosition().x, this->GetPosition().y, this->GetPosition().z);

	CCar::Set();
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