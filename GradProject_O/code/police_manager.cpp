//==========================================================
//
// 警察マネージャー [police_manager.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "police_manager.h"
#include "police.h"
#include "player_manager.h"
#include "road.h"
#include "inspection.h"
#include "add_police.h"
#include "deltatime.h"
#include "debugproc.h"
#include "pred_route.h"
#include "network.h"
#include "inspection_manager.h"
#include "input_keyboard.h"
#include "car_manager.h"

// 名前空間
namespace
{
	const int MAX_NEAR = 2;			// 近い警察の確認数
	const float INTERVAL = 5.0f;	// インターバル
	const int MAX_POLICE = (15);	// 警察の最大値
}

// 静的メンバ変数宣言
CPoliceManager* CPoliceManager::m_pInstance = nullptr;	// インスタンス

//==========================================================
// コンストラクタ
//==========================================================
CPoliceManager::CPoliceManager()
{
	// 値のクリア
	m_pList = nullptr;
	m_InspInfo = SInspInfo();
	m_nNum = 0;
	m_maplist.Clear();
	m_NearPoliceList.clear();
	m_Siren.clear();
}

//==========================================================
// デストラクタ
//==========================================================
CPoliceManager::~CPoliceManager()
{

}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CPoliceManager::Init(void)
{
	m_InspInfo.fTime = INTERVAL;

	m_Siren.clear();
	for (int i = 0; i < MAX_NEAR; i++)
	{
		m_Siren.push_back(CMasterSound::CObjectSound::Create("data\\SE\\siren.wav", -1));
		m_Siren[i]->SetVolume(0.0f);
		m_Siren[i]->Stop();
	}

	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CPoliceManager::Uninit(void)
{
	// 親クラスの終了処理
	CListManager::Uninit();

	// リスト廃棄
	ListRelease();

	m_maplist.Clear();
	m_NearPoliceList.clear();

	
	{
		for (int i = 0; i < m_Siren.size(); i++)
		{
			m_Siren[i]->SetVolume(0.0f);
			m_Siren[i]->Stop();
			SAFE_UNINIT_DELETE(m_Siren[i])
		}

		m_Siren.clear();
	}
	

	// インスタンスの廃棄
	if (m_pInstance != nullptr) {	// インスタンスを確保されている
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

//==========================================================
// 更新処理
//==========================================================
void CPoliceManager::Update(void)
{
	m_InspInfo.fInterval += CDeltaTime::GetInstance()->GetDeltaTime();

	CDebugProc::GetInstance()->Print("警察の総数 : [ %d ]\n", m_pList->GetNum());

	// 近い警察を取得する
	CPlayer* pPlayer = CPlayerManager::GetInstance()->GetPlayer();
	if (pPlayer == nullptr) { return; }
	std::vector<int> nearid = {};

	// 前回を覚える
	std::vector<CPolice*> old = m_NearPoliceList;
	m_NearPoliceList.clear();

	// 近い警察取得
	while (nearid.size() < MAX_NEAR)
	{
		float minlength = 1000000.0f;
		int id = -1;
		for (int i = 0; i < GetList()->GetNum(); i++)
		{
			// もう手前側にいる
			auto it = find(nearid.begin(), nearid.end(), i);
			if (it != nearid.end()) { continue; }

			CPolice* pPolice = GetList()->Get(i);
			D3DXVECTOR3 vec = pPlayer->GetPosition() - pPolice->GetPosition();
			float length = D3DXVec3Length(&vec);

			// より近い
			if (length < minlength)
			{
				id = i;
				minlength = length;
			}

		}

		// まだいない
		auto it = find(nearid.begin(), nearid.end(), id);
		if (it == nearid.end()&& id != -1)
		{
			nearid.push_back(id);
			it = find(nearid.begin(), nearid.end(), id);
			m_NearPoliceList.push_back(GetList()->Get(id));
		}
		else
		{
			break;
		}
	}

	// 今回使われない場合消す
	std::vector<int> activesoundid = {};
	for (int i = 0; i < old.size(); i++)
	{
		auto it = find(m_NearPoliceList.begin(), m_NearPoliceList.end(), old[i]);

		// 使用されている場合は使用しているIDを取得する
		if (it != m_NearPoliceList.end())
		{
			if (old[i]->GetSound() != nullptr)
			{
				activesoundid.push_back(std::distance(m_Siren.begin(), find(m_Siren.begin(), m_Siren.end(), old[i]->GetSound())));
			}
		}
		else
		{
			old[i]->SetSound(nullptr);
		}
	}

	// 音を設定
	{
		int soundcnt = 0;
		for (int i = 0; i < m_NearPoliceList.size(); i++)
		{
			CPolice* pPolice = m_NearPoliceList[i];
			// 曲をすでに使用している
			if (pPolice->GetSound() != nullptr) { continue; }

			for (int j = 0; j < m_Siren.size(); j++)
			{
				// 使われている
				if (activesoundid.end() != find(activesoundid.begin(), activesoundid.end(), j)) { continue; }

				// あてはめる
				if (pPolice->GetChase())
				{
					pPolice->SetSound(GetSound(j));
				}
				else
				{
					pPolice->SetSound(m_Siren[j]);
				}
				
				activesoundid.push_back(j);
			}

		}
	}

	// 消すよん
	nearid.clear();
}

//==========================================================
// インスタンスを確保
//==========================================================
CPoliceManager* CPoliceManager::GetInstance(void)
{
	if (m_pInstance == nullptr) {	// 使われていない
		m_pInstance = DEBUG_NEW CPoliceManager;
		m_pInstance->Init();
	}

	return m_pInstance;
}

//==========================================================
// インスタンスをリリース
//==========================================================
void CPoliceManager::Release(void)
{
	if (m_pInstance != nullptr) {	// インスタンスを確保されている
		m_pInstance->Uninit();
	}
}

//==========================================================
// リストに挿入
//==========================================================
void CPoliceManager::ListIn(CPolice* pPolice)
{
	GetList()->Regist(pPolice);
	IdListIn(pPolice);
}

//==========================================================
// リストから外す
//==========================================================
void CPoliceManager::ListOut(CPolice* pPolice)
{
	// リストから自分自身を削除する
	GetList()->Delete(pPolice);
	IdListOut(pPolice);
}

//==========================================================
// Idリストに挿入
//==========================================================
void CPoliceManager::IdListIn(CPolice* pPolice)
{
	m_maplist.Regist(pPolice->GetId(), pPolice);
}

//==========================================================
// Idリストから外す
//==========================================================
void CPoliceManager::IdListOut(CPolice* pPolice)
{
	// リストから自分自身を削除する
	m_maplist.Delete(pPolice->GetId(), pPolice);
}

//==========================================================
// 攻撃ヒット確認
//==========================================================
bool CPoliceManager::Hit(D3DXVECTOR3& pos, const float fRange, const float fHeight, const int nDamage)
{
	bool bUse = false;

	//個別判定
	for (int i = 0; i < GetList()->GetNum(); i++)
	{
		//CPolice* pPolice = GetList()->Get(i);
		//if (pPolice->HitCheck(pos, fRange, fHeight, nDamage)) {
			//bUse = true;
		//}
	}

	return bUse;
}

//==========================================================
// 全員を警戒状態にする
//==========================================================
void CPoliceManager::Warning(CPolice* pPolice)
{
	if (pPolice->GetState() == CPolice::STATE::STATE_FADEOUT) { return; }

	for (int i = 0; i < GetList()->GetNum(); i++)
	{
		// 警察と自分が一致してたら駄目
		CPolice* pP = GetList()->Get(i);
		if (pP == pPolice) { continue; }

		// 現在追跡中も駄目
		CPolice::STATE outstate = CPolice::STATE::STATE_CHASE;
		if (pP->GetState() == outstate) { continue; }

		// 停止中も駄目
		outstate = CPolice::STATE::STATE_STOP;
		if (pP->GetState() == outstate) { continue; }

		// 警戒状態にする
		pP->SetState(CPolice::STATE::STATE_SEARCH);
	}

	if (m_InspInfo.fInterval >= m_InspInfo.fTime && pPolice->IsActive())
	{
		m_InspInfo.fInterval = 0.0f;
		// 検問を配置する
		SetInspection();
	}
}

//==========================================================
// 検問を配置する
//==========================================================
void CPoliceManager::SetInspection()
{
	CPlayer* pPlayer = CPlayerManager::GetInstance()->GetPlayer();
	if (pPlayer == nullptr) { return; }

	// プレイヤーの予測を取得する
	CPredRoute* pRoute = pPlayer->GetPredRoute();
	if (pRoute == nullptr) { return; }

	// 目標地点とひとつ前
	CRoad* pRoad = pRoute->GetPredRoad();
	CRoad* pPrev = pRoute->GetPredPrevRoad();

	// 道が見つからなかった
	if (pRoad == nullptr) { return; }

	// 方向を取得する
	D3DXVECTOR3 rot = VECTOR3_ZERO;
	float targetrot = atan2f(pPrev->GetPosition().x - pRoad->GetPosition().x,
		pPrev->GetPosition().z - pRoad->GetPosition().z);
	rot.y = targetrot + D3DX_PI * 0.5f;
	Adjust(&rot.y);

	// 設置
	D3DXVECTOR3 pos = pRoad->GetPosition();
	pos.x += sinf(targetrot) * pRoad->GetInfo()->size.x;
	pos.z += cosf(targetrot) * pRoad->GetInfo()->size.y;

	int startid = CCarManager::GetInstance()->GetMapList()->GetInCnt();

	// 検問生成
	CInspectionManager::GetInstance()->SetNextInspection(pos, rot, pRoad, CInspectionManager::GetInstance()->GetCreateCnt(), startid);

	// 検問送信
	auto net = CNetWork::GetInstance();
	net->SendSetInspection(CInspectionManager::GetInstance()->GetCreateCnt(), pos, rot, pRoad->GetIdx(), startid);
}