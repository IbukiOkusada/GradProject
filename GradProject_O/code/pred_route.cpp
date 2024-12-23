//==========================================================
//
// 道予測処理 [pred_route.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "pred_route.h"
#include "player.h"
#include "road.h"
#include "a_star.h"
#include "deltatime.h"
#include "debugproc.h"

// 名前空間定義
namespace
{
	const int MIN_PRED_NUM = 3;	// 最低限の道数
	const float STOP_TIME = 5.0f;	// 停止
}

//==========================================================
// コンストラクタ
//==========================================================
CPredRoute::CPredRoute()
{
	// 値のクリア
	m_PassRoad.clear();
	m_pPlayer = nullptr;
	m_pOldRoad = nullptr;
	m_pPredPrevRoad = nullptr;
	m_pPredRoad = nullptr;
	m_fStopCount = 0.0f;

	memset(&m_aTurnCount[0], 0, sizeof(m_aTurnCount));
}

//==========================================================
// デストラクタ
//==========================================================
CPredRoute::~CPredRoute()
{

}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CPredRoute::Init(void)
{
	m_fStopCount = 0.0f;
	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CPredRoute::Uninit(void)
{
	// 開放
	Release();
}

//==========================================================
// 更新処理
//==========================================================
void CPredRoute::Update(void)
{
	if (m_pPlayer == nullptr) { return; }

	// 移動した
	if (m_pPlayer->GetRoad() != m_pOldRoad)
	{
		m_pOldRoad = m_pPlayer->GetRoad();

		// 道路の上
		if (m_pOldRoad != nullptr)
		{
			// 間を補間
			Interp();

			// 確認
			TurnCheck();

			// 予測地点を立てる
			Pred();
		}
		else
		{
			m_vecOld = VEC::VEC_OVER;
			m_aTurnCount[VEC::VEC_OVER]++;
		}
	}
	else
	{
		// 停止カウント増加
		m_fStopCount += CDeltaTime::GetInstance()->GetDeltaTime();

		// 停止し続けている
		if (m_fStopCount >= STOP_TIME && m_pPlayer->GetEngine() <= 0.3f) {
			m_vecOld = VEC::VEC_STOP;
			m_aTurnCount[VEC::VEC_STOP]++;
			m_fStopCount = 0.0f;
		}
	}

	// デバッグ表示
	CDebugProc::GetInstance()->Print("直進 [ %d ] : 左回転 [ %d ] : 右回転 [ %d ] : 旋回 [ %d ] : 停止 [ %d ] : 道外 [ %d ]\n",
		m_aTurnCount[VEC::VEC_STRAIGHT], m_aTurnCount[VEC::VEC_TURNLEFT], m_aTurnCount[VEC::VEC_TURNRIGHT], m_aTurnCount[VEC::VEC_TURNING],
		m_aTurnCount[VEC::VEC_STOP], m_aTurnCount[VEC::VEC_OVER]);
}

//==========================================================
// 生成
//==========================================================
CPredRoute* CPredRoute::Create(CPlayer* pPlayer)
{
	CPredRoute* pPredRoute = nullptr;

	pPredRoute = DEBUG_NEW CPredRoute;

	if (pPredRoute != nullptr)
	{
		// 初期化処理
		pPredRoute->m_pPlayer = pPlayer;
		pPredRoute->Init();
	}

	return pPredRoute;
}

//==========================================================
// 回転確認
//==========================================================
void CPredRoute::TurnCheck()
{
	// 確認数に満たない
	if (m_PassRoad.size() < MIN_PRED_NUM) { return; }

	// 現在、前回、前々回から回転を取得する
	CRoad* pNow = m_PassRoad[m_PassRoad.size() - 1];
	CRoad* pOld = m_PassRoad[m_PassRoad.size() - 2];
	CRoad* pOlder = m_PassRoad[m_PassRoad.size() - 3];

	// 同じ場所の場合は旋回していない
	if (pNow == pOld) {

		// 停止し続けている
		if (m_fStopCount >= STOP_TIME && m_pPlayer->GetEngine() <= 0.3f) { 
			m_vecOld = VEC::VEC_STOP;
			m_aTurnCount[VEC::VEC_STOP]++;
			m_fStopCount = 0.0f;
		}
		return;
	}
	// 前々回と一緒
	else if (pNow == pOlder)
	{
		m_vecOld = VEC::VEC_TURNING;
		m_aTurnCount[VEC::VEC_TURNING]++;
		m_fStopCount = 0.0f;
		return;
	}

	// 方向を求める
	float vecnow = atan2f(pNow->GetPosition().x - pOld->GetPosition().x, 
		pNow->GetPosition().z - pOld->GetPosition().z);
	Adjust(&vecnow);

	// 前回の方向を求める
	float vecold = atan2f(pOld->GetPosition().x - pOlder->GetPosition().x,
		pOld->GetPosition().z - pOlder->GetPosition().z);
	Adjust(&vecold);

	// 差分を求める
	float diff = vecnow - vecold;
	Adjust(&diff);

	float RANGE = D3DX_PI * 0.25f;
	// 差分から曲がった方向を求める
	if (diff <= RANGE && diff >= -RANGE)
	{// 直線
		if (m_vecOld == VEC::VEC_STRAIGHT) { return; }
		m_vecOld = VEC::VEC_STRAIGHT;
		m_fStopCount = 0.0f;
		m_aTurnCount[VEC::VEC_STRAIGHT]++;
	}
	else if (diff <= -RANGE && diff >= -RANGE * 3)
	{// 左に曲がった
		m_vecOld = VEC::VEC_TURNLEFT;
		m_aTurnCount[VEC::VEC_TURNLEFT]++;
	}
	else if (diff <= RANGE * 3 && diff >= RANGE)
	{// 右に曲がった
		m_vecOld = VEC::VEC_TURNRIGHT;
		m_aTurnCount[VEC::VEC_TURNRIGHT]++;
	}
	else if (diff >= RANGE * 3 && diff <= -RANGE * 3)
	{// 旋回した
		m_vecOld = VEC::VEC_TURNING;
		m_aTurnCount[VEC::VEC_TURNING]++;
	}
	else
	{
		m_vecOld = VEC::VEC_STOP;
		m_aTurnCount[VEC::VEC_STOP]++;
	}
}


//==========================================================
// 間を補完する
//==========================================================
void CPredRoute::Interp()
{
	// まだ未登録
	if (m_PassRoad.size() == 0) {
		// 通った道を覚える
		m_PassRoad.push_back(m_pOldRoad);
		return;
	}

	CRoad* pOld = m_PassRoad[m_PassRoad.size() - 1];

	// 間を補完する
	std::vector<CRoad::SSearch*> path = AStar::AStar(pOld->GetSearchSelf(), m_pOldRoad->GetSearchSelf());

	// 直線移動だった場合
	if (pOld->GetPosition().x == m_pOldRoad->GetPosition().x ||
		pOld->GetPosition().z == m_pOldRoad->GetPosition().z)
	{
		// 通った道を覚える
		m_PassRoad.push_back(m_pOldRoad);
		return;
	}

	// 最短経路で補間する
	if (path.size() <= MIN_PRED_NUM)
	{
		for (int i = 1; i < path.size(); i++)
		{
			m_PassRoad.push_back(path[i]->pRoad);
		}
	}

	// 通った道を覚える
	m_PassRoad.push_back(m_pOldRoad);
}

//==========================================================
// 次を予測立てる
//==========================================================
void CPredRoute::Pred()
{
	// 確認数に満たない
	if (m_PassRoad.size() < MIN_PRED_NUM) { return; }

	// 次に向かう方向を取得する
	for (int i = 0; i < VEC::VEC_MAX; i++)
	{
		float rot = GetNextRot(GetSelectRankVec(i));

		// 現在の位置を取得する
		CRoad* pRoad = m_pOldRoad;

		bool flag = pRoad->GetJunctionRoad(rot, &m_pPredRoad, &m_pPredPrevRoad);

		if (flag) { 
			break;
		}
	}
}

//==========================================================
// 次の回転方向
//==========================================================
float CPredRoute::GetNextRot(const VEC& vec)
{
	float rot = GetRot(vec);

	// 前回と今回から値を取得
	CRoad* pNow = m_PassRoad[m_PassRoad.size() - 1];
	CRoad* pOld = m_PassRoad[m_PassRoad.size() - 2];

	// 方向を求める
	float vecnow = atan2f(pNow->GetPosition().x - pOld->GetPosition().x,
		pNow->GetPosition().z - pOld->GetPosition().z);
	Adjust(&vecnow);

	vecnow += rot;
	Adjust(&vecnow);

	return vecnow;
}

//==========================================================
// 指定された順位の回転方向を取得
//==========================================================
CPredRoute::VEC CPredRoute::GetSelectRankVec(const int nRank)
{
	std::vector<VEC> ranklist;
	bool flag[VEC::VEC_MAX] = {false};

	// 全順位分確認
	for (int j = 0; j < VEC::VEC_MAX; j++)
	{
		VEC vec = VEC::VEC_STRAIGHT;

		for (int i = 0; i < VEC::VEC_MAX; i++)
		{
			auto result = find(ranklist.begin(), ranklist.end(), vec);

			if (m_aTurnCount[i] > m_aTurnCount[vec])
			{
				vec = static_cast<VEC>(i);
			}
			else if (result != ranklist.end())
			{
				vec = static_cast<VEC>(i);
			}
		}

		ranklist.push_back(vec);
		flag[vec] = true;
	}

	return ranklist[nRank];
}

//==========================================================
// 方向を取得
//==========================================================
float CPredRoute::GetRot(const VEC& vec)
{
	switch (vec)
	{
	case VEC::VEC_STRAIGHT:
	{
		return 0.0f;
	}
	break;

	case VEC::VEC_TURNLEFT:
	{
		return -D3DX_PI * 0.5f;
	}
	break;

	case VEC::VEC_TURNRIGHT:
	{
		return D3DX_PI * 0.5f;
	}
		break;

	case VEC::VEC_TURNING:
	{
		return D3DX_PI;
	}
		break;

	case VEC::VEC_OVER:
	{
		return -D3DX_PI;
	}
		break;

	case VEC::VEC_STOP:
	{
		return D3DX_PI;
	}
		break;

	default:
	{

	}
		break;
	}

	return 0.0f;
}