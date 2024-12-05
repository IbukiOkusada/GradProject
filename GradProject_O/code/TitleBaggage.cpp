//<========================================================
//タイトルでのアイス処理 [TitleBaggage.h]
//
//Author : Kazuki Watanabe
//<========================================================
#include "TitleBaggage.h"
#include "model.h"
#include "character.h"
#include "road_manager.h"
#include "road.h"
#include "goal_manager.h"

//<=========================================================
//初期化
//<=========================================================
HRESULT CTitleBaggage::Init(void)
{
	CBaggage::Init();
	return S_OK;
}
//<=========================================================
//生成処理
//<=========================================================
CTitleBaggage* CTitleBaggage::Create(const D3DXVECTOR3& pos)
{
	//メモリ確保するための変数
	CTitleBaggage* pTitleBaggage = DEBUG_NEW CTitleBaggage;

	//メモリ確保されていたら
	if (pTitleBaggage)
	{
		//設定
		pTitleBaggage->Init();
	}
	else { return nullptr; }

	return pTitleBaggage;
}
//<=========================================================
//ゴールの初期化
//<=========================================================
HRESULT CTitleGoal::Init(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot)
{
	constexpr char* MOTION_HUMAN = "data\\TXT\\motion_kidsboy.txt";	//ゴールとなる人間のモーション
	const D3DXVECTOR3 SCALE = D3DXVECTOR3(2.0f, 2.0f, 2.0f);		//スケール

	//初期化
	m_pPeople = nullptr;
	m_rPos = VECTOR3_ZERO;

	//人間の設定
	m_pPeople = CCharacter::Create(pos, rot,MOTION_HUMAN);
	m_pPeople->SetScale(SCALE);

	//位置設定
	m_rPos = pos;

	return S_OK;
}
//<=========================================================
//ゴールの終了処理
//<=========================================================
void CTitleGoal::Uninit(void)
{
	//終了処理
	SAFE_UNINIT(m_pPeople);
	Release();
}
//<=========================================================
//更新処理
//<=========================================================
void CTitleGoal::Update(void)
{
	//人間の更新
	if (m_pPeople){m_pPeople->Update();}
}
//<=========================================================
//ゴールの生成処理
//<=========================================================
CTitleGoal* CTitleGoal::Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot)
{
	CTitleGoal* pTitleGoal = DEBUG_NEW CTitleGoal;

	if (pTitleGoal) { pTitleGoal->Init(pos, rot); }
	else { return nullptr; }

	return pTitleGoal;
}