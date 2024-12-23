//==========================================================
//
// プレイヤーマネージャー [car_manager.cpp]
// Author : Riku Nakamura
//
//==========================================================
#include "car_manager.h"
#include "car.h"
#include "police.h"
#include "add_police.h"
#include "player_manager.h"
#include "network.h"

// 静的メンバ変数宣言
CCarManager* CCarManager::m_pInstance = nullptr;	// インスタンス

//==========================================================
// コンストラクタ
//==========================================================
CCarManager::CCarManager() : 
m_pList(nullptr),
m_nNum(0),
m_List(),
m_NextList(),
m_NextTempList()
{
	// 値のクリア
}

//==========================================================
// デストラクタ
//==========================================================
CCarManager::~CCarManager()
{

}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CCarManager::Init(void)
{
	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CCarManager::Uninit(void)
{
	// 親クラスの終了処理
	CListManager::Uninit();

	// リスト廃棄
	ListRelease();

	m_List.Clear();

	m_NextList.Clear();
	m_NextTempList.Clear();

	// インスタンスの廃棄
	if (m_pInstance != nullptr) {	// インスタンスを確保されている
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

//==========================================================
// 更新処理
//==========================================================
void CCarManager::Update(void)
{
	m_bSet = true;

	auto list = *m_NextList.GetList();

	for (const auto& it : list)
	{
		// 既に存在する場合は生成しない
		CCar* pCar = m_List.Get(it.first);
		if (pCar != nullptr) { continue; }

		auto info = it.second;

		// 種類ごとに生成
		switch (it.second->type)
		{
		case CCar::CAR_TYPE::CAR_TYPE_CAR:
		{
			pCar = CCar::Create(info->pos, info->rot, info->move, it.first);
			pCar->SetType(CCar::TYPE::TYPE_RECV);
		}
			break;

		case CCar::CAR_TYPE::CAR_TYPE_POLICE:
		{
			// 応援の警察を生成
			CPolice* pP = CPolice::Create(info->pos, info->rot, info->move, it.first);
			CPlayer* pPlayer = CPlayerManager::GetInstance()->GetPlayer(info->nChaseId);

			pP->SetType(CCar::TYPE::TYPE_RECV);
			//pP->SetChaseNext(info->chase);

			//if (info->nChaseId == CNetWork::GetInstance()->GetIdx())
			//{
			//	// 自分自身
			//	pP->SetTypeNext(CCar::TYPE::TYPE_ACTIVE);
			//	pP->SetNextPlayer(pPlayer);
			//}
			//else
			//{
			//	pP->SetTypeNext(CCar::TYPE::TYPE_RECV);
			//	pP->SetNextPlayer(pPlayer);
			//}

			pCar = pP;
		}
			break;

		case CCar::CAR_TYPE::CAR_TYPE_ADDPOLICE:
		{
			// 応援の警察を生成
			CAddPolice* pP = CAddPolice::Create(info->pos, info->rot, info->move, it.first);
			CPlayer* pPlayer = CPlayerManager::GetInstance()->GetPlayer(info->nChaseId);

			if (pP == nullptr) { continue; }

			pP->SetType(CCar::TYPE::TYPE_RECV);

			//if (info->chase == CPolice::CHASE::CHASE_BEGIN)
			//{
			//	// 追跡状態に変更
			//	pP->SetChase(true);
			//	pP->GetAi()->BeginChase(pPlayer);

			//	// 応援の警察は応援を呼ばないようにする
			//	pP->GetAi()->SetCall(true);
			//	pP->SetChaseNext(info->chase);
			//}
			//else if (info->chase == CPolice::CHASE::CHASE_END)
			//{
			//	pP->SetChaseNext(info->chase);
			//}

			//// 応援の警察のタイプを設定
			//pP->SetTypeAI((CPoliceAI::TYPE)(rand() % CPoliceAI::TYPE_MAX));
			//pP->SetTypeAI(CPoliceAI::TYPE_ELITE);
			//pP->SetType(CCar::TYPE::TYPE_ACTIVE);

			//// 目的地設定
			//pP->SetRoadTarget(nullptr);

			//if (info->nChaseId == CNetWork::GetInstance()->GetIdx())
			//{
			//	// 自分自身
			//	pP->SetTypeNext(CCar::TYPE::TYPE_ACTIVE);
			//	pP->SetNextPlayer(pPlayer);
			//}
			//else
			//{
			//	pP->SetTypeNext(CCar::TYPE::TYPE_RECV);
			//	pP->SetNextPlayer(pPlayer);
			//}

			pCar = pP;
		}
			break;

		default:
		{

		}
			break;
		}
	}


	// 仮データを入れ替える
	m_NextList.Clear();
	m_NextList.ResetInCnt();
	m_NextList = m_NextTempList;

	m_bSet = false;
	m_NextTempList.Clear();
	m_NextTempList.ResetInCnt();
}

//==========================================================
// インスタンスを確保
//==========================================================
CCarManager* CCarManager::GetInstance(void)
{
	if (m_pInstance == nullptr) {	// 使われていない
		m_pInstance = DEBUG_NEW CCarManager;
	}

	return m_pInstance;
}

//==========================================================
// インスタンスをリリース
//==========================================================
void CCarManager::Release(void)
{
	if (m_pInstance != nullptr) {	// インスタンスを確保されている
		m_pInstance->Uninit();
	}
}

//==========================================================
// リストに挿入
//==========================================================
void CCarManager::ListIn(CCar* pCar)
{
	GetList()->Regist(pCar);
	IdListIn(pCar);
}

//==========================================================
// リストから外す
//==========================================================
void CCarManager::ListOut(CCar* pCar)
{
	// リストから自分自身を削除する
	GetList()->Delete(pCar);
	IdListOut(pCar);
}

//==========================================================
// 次生成用のリストに挿入
//==========================================================
void CCarManager::CreateListIn(NextCreateInfo& info, int nId)
{
	// 既に保存済み
	if (m_NextList.IdFind(nId) || m_NextTempList.IdFind(nId)) { return; }

	if (!m_bSet)
	{
		m_NextList.Regist(nId, &info);
	}
	else
	{
		m_NextTempList.Regist(nId, &info);
	}
}

//==========================================================
// リストから外す
//==========================================================
void CCarManager::CreateListOut(NextCreateInfo& info, int nId)
{
	// 存在しない
	if (m_NextList.IdFind(nId))
	{
		m_NextList.Delete(nId, &info);
	}
	else if(m_NextTempList.IdFind(nId))
	{
		m_NextTempList.Delete(nId, &info);
	}
}

//==========================================================
// 次の生成
//==========================================================
CCarManager::NextCreateInfo* CCarManager::CreateGet(int nId)
{
	if (m_NextList.IdFind(nId))
	{
		return m_NextList.Get(nId);
	}
	else if (m_NextTempList.IdFind(nId))
	{
		return m_NextTempList.Get(nId);
	}

	return nullptr;
}

//==========================================================
// マップリストに挿入
//==========================================================
void CCarManager::IdListIn(CCar* pCar)
{
	m_List.Regist(pCar->GetId(),pCar);
}

//==========================================================
// マップリストから外す
//==========================================================
void CCarManager::IdListOut(CCar* pCar)
{
	// リストから自分自身を削除する
	m_List.Delete(pCar->GetId(), pCar);
}

//==========================================================
// 攻撃ヒット確認
//==========================================================
bool CCarManager::Hit(D3DXVECTOR3& pos, const float fRange, const float fHeight, const int nDamage)
{
	bool bUse = false;

	//個別判定
	for (int i = 0; i < GetList()->GetNum(); i++)
	{
		//CCar* pCar = GetList()->Get(i);
		//if (pCar->HitCheck(pos, fRange, fHeight, nDamage)) {
			//bUse = true;
		//}
	}

	return bUse;
}
