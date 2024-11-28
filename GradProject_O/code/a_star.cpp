//==========================================================
//
// AStar [a_star.cpp]
// Author : 丹野竜之介
//
//==========================================================
#include "a_star.h"

namespace AStar
{
//==========================================================
// AStarでの経路探索
//==========================================================
std::vector<CRoad::SSearch*> AStar(CRoad::SSearch* State, CRoad::SSearch* Gole)
{
	std::vector<CRoad::SSearch*> OpenList;//探索候補リスト
	std::vector<CRoad::SSearch*> CloseList;//探索終了リスト
	
	State->fGCost = 0.0f;
	State->fHCost = D3DXVec3Length(&(State->pos - Gole->pos));
	State->fFCost = State->fGCost + State->fHCost;
	OpenList.push_back(State);

	// オープンリストが空になるまで探索
	while (!OpenList.empty())
	{
		CRoad::SSearch* Current;		// 現在の参照ノード
		Current = *std::min_element(OpenList.begin(), OpenList.end(),
			[](CRoad::SSearch* a, CRoad::SSearch* b) { return a->fFCost < b->fFCost; });		// 総計コストが最少のノードを選ぶ

		if (Current->pRoad == Gole->pRoad)		// ゴール到達時
		{// 親ノードを辿って順序を入れ替えたのち返す

			std::vector<CRoad::SSearch*> path;
			CRoad::SSearch* temp;

			while (Current != nullptr) 
			{
				if (Current->pParent != nullptr)
				{
					Current->pParent->pChaild = Current;
				}
				path.push_back(Current);
				Current = Current->pParent;
			}

			std::reverse(path.begin(), path.end());
			return path;
		}

		// オープンリストから削除してクローズリストに追加
		OpenList.erase(std::remove(OpenList.begin(), OpenList.end(), Current), OpenList.end());
		CloseList.push_back(Current);

		for (int i = 0; i < CRoad::DIC_MAX; i++)
		{// 隣接ノードをコスト計算の後オープンリストに追加

			CRoad::SSearch* neighbor = Current->pRoad->GetSearchRoad((CRoad::DIRECTION)i);

			if (std::find(CloseList.begin(), CloseList.end(), neighbor) != CloseList.end() || !neighbor->bActive) continue;
			
			// コスト計算
			neighbor->fGCost = D3DXVec3Length(&(neighbor->pos - Current->pos));
			neighbor->fHCost = D3DXVec3Length(&(neighbor->pos - Gole->pos));
			neighbor->fFCost = neighbor->fGCost + neighbor->fHCost;
			neighbor->pParent = Current;

			// オープンリストに追加
			if (std::find(OpenList.begin(), OpenList.end(), neighbor) == OpenList.end())
			{
				OpenList.push_back(neighbor);
			}
		}
	}
	return {};
}

// AStarでの経路探索
std::vector<CRoad::SSearch*> AStarPolice(CRoad::SSearch* State, CRoad::SSearch* Gole)
{
	std::vector<CRoad::SSearch*> OpenList;//探索候補リスト
	std::vector<CRoad::SSearch*> CloseList;//探索終了リスト

	State->fGCost = 0.0f;
	State->fHCost = D3DXVec3Length(&(State->pos - Gole->pos));
	State->fFCost = State->fGCost + State->fHCost;
	OpenList.push_back(State);

	// オープンリストが空になるまで探索
	while (!OpenList.empty())
	{
		CRoad::SSearch* Current;		// 現在の参照ノード
		Current = *std::min_element(OpenList.begin(), OpenList.end(),
			[](CRoad::SSearch* a, CRoad::SSearch* b) { return a->fFCost < b->fFCost; });		// 総計コストが最少のノードを選ぶ

		if (Current->pRoad == Gole->pRoad)		// ゴール到達時
		{// 親ノードを辿って順序を入れ替えたのち返す

			std::vector<CRoad::SSearch*> path;
			CRoad::SSearch* temp;

			while (Current != nullptr) 
			{
				if (Current->pParent != nullptr)
				{
					Current->pParent->pChaild = Current;
				}
				path.push_back(Current);
				Current = Current->pParent;
			}

			std::reverse(path.begin(), path.end());
			return path;
		}

		// オープンリストから削除してクローズリストに追加
		OpenList.erase(std::remove(OpenList.begin(), OpenList.end(), Current), OpenList.end());
		CloseList.push_back(Current);

		for (int i = 0; i < CRoad::DIC_MAX; i++)
		{// 隣接ノードをコスト計算の後オープンリストに追加

			if (Current->pRoad->GetConnectRoad((CRoad::DIRECTION)i) == nullptr)
				continue;

			CRoad::SSearch* neighbor = Current->pRoad->GetSearchRoad((CRoad::DIRECTION)i);

			if (std::find(CloseList.begin(), CloseList.end(), neighbor) != CloseList.end()) continue;

			// コスト計算
			neighbor->fGCost = Current->pRoad->GetConnectLength((CRoad::DIRECTION)i);
			neighbor->fHCost = D3DXVec3Length(&(neighbor->pRoad->GetPosition() - Gole->pos));
			neighbor->fFCost = neighbor->fGCost + neighbor->fHCost;
			neighbor->pParent = Current;

			// オープンリストに追加
			if (std::find(OpenList.begin(), OpenList.end(), neighbor) == OpenList.end())
			{
				OpenList.push_back(neighbor);
			}
		}
	}
	return {};
}

//==========================================================
// AStarでの回数制限付き経路探索
//==========================================================
std::vector<CRoad::SSearch*> AStarLimit(CRoad::SSearch* State, CRoad::SSearch* Gole, int nLimitSearch)
{
	std::vector<CRoad::SSearch*> OpenList;		// 探索候補リスト
	std::vector<CRoad::SSearch*> CloseList;		// 探索終了リスト
	int nCntSearch = 0;							// 探索回数

	State->fGCost = 0.0f;
	State->fHCost = D3DXVec3Length(&(State->pos - Gole->pos));
	State->fFCost = State->fGCost + State->fHCost;
	OpenList.push_back(State);

	// オープンリストが空になるまで探索
	while (!OpenList.empty())
	{
		CRoad::SSearch* Current;		// 現在の参照ノード
		Current = *std::min_element(OpenList.begin(), OpenList.end(),
			[](CRoad::SSearch* a, CRoad::SSearch* b) { return a->fFCost < b->fFCost; });		// 総計コストが最少のノードを選ぶ

		if (Current->pRoad == Gole->pRoad || nCntSearch == nLimitSearch)		// ゴール到達時
		{// 親ノードを辿って順序を入れ替えたのち返す

			std::vector<CRoad::SSearch*> path;

			while (Current != nullptr) 
			{
				if (Current->pParent != nullptr)
				{
					Current->pParent->pChaild = Current;
				}
				path.push_back(Current);
				Current = Current->pParent;
			}

			std::reverse(path.begin(), path.end());
			return path;
		}

		// オープンリストから削除してクローズリストに追加
		OpenList.erase(std::remove(OpenList.begin(), OpenList.end(), Current), OpenList.end());
		CloseList.push_back(Current);

		for (int i = 0; i < CRoad::DIC_MAX; i++)
		{// 隣接ノードをコスト計算の後オープンリストに追加

			CRoad::SSearch* neighbor = Current->pRoad->GetSearchRoad((CRoad::DIRECTION)i);

			if (std::find(CloseList.begin(), CloseList.end(), neighbor) != CloseList.end() || !neighbor->bActive) continue;

			// コスト計算
			neighbor->fGCost = Current->pRoad->GetConnectLength((CRoad::DIRECTION)i);
			neighbor->fHCost = D3DXVec3Length(&(neighbor->pos - Gole->pos));
			neighbor->fFCost = neighbor->fGCost + neighbor->fHCost;
			neighbor->pParent = Current;

			// オープンリストに追加
			if (std::find(OpenList.begin(), OpenList.end(), neighbor) == OpenList.end())
			{
				OpenList.push_back(neighbor);
			}
		}

		nCntSearch++;
	}
	return {};
}
}
