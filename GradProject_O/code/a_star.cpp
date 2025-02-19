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
std::vector<CRoad::SSearch*> AStar(CRoad::SSearch* Start, CRoad::SSearch* Goal)
{
	if (Start == nullptr || Goal == nullptr) return {};

	std::vector<CRoad::SSearch*> OpenList;//探索候補リスト
	std::vector<CRoad::SSearch*> CloseList;//探索終了リスト
	
	Start->fGCost = 0.0f;
	D3DXVECTOR3 v = (Start->pos - Goal->pos);
	Start->fHCost = D3DXVec3Length(&v);
	Start->fFCost = Start->fGCost + Start->fHCost;
	OpenList.push_back(Start);

	// オープンリストが空になるまで探索
	while (!OpenList.empty())
	{
		CRoad::SSearch* Current;		// 現在の参照ノード
		Current = *std::min_element(OpenList.begin(), OpenList.end(),
			[](CRoad::SSearch* a, CRoad::SSearch* b) { return a->fFCost < b->fFCost; });		// 総計コストが最少のノードを選ぶ

		if (Current->pRoad == Goal->pRoad)		// ゴール到達時
		{// 親ノードを辿って順序を入れ替えたのち返す

			std::vector<CRoad::SSearch*> path;

			while (Current != nullptr) 
			{
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

			if (std::find(CloseList.begin(), CloseList.end(), neighbor) != CloseList.end() || !neighbor->bActive) { continue; }
			
			// コスト計算
			v = (neighbor->pos - Current->pos);
			neighbor->fGCost = D3DXVec3Length(&v);
			v = (neighbor->pos - Goal->pos);
			neighbor->fHCost = D3DXVec3Length(&v);
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
// AStarでの警察用経路探索
//==========================================================
std::vector<CRoad::SInfoSearch*> AStarPolice(CRoad* Start, CRoad* Goal)
{
	if(Start == nullptr || Goal == nullptr){ return {}; }

	std::vector < CRoad::SInfoSearch * > OpenList;		//探索候補リスト
	std::vector<CRoad::SInfoSearch*> CloseList;			//探索終了リスト

	Start->GetInfoSearch()->fGCost = 0.0f;
	D3DXVECTOR3 v = (Start->GetPosition() - Goal->GetPosition());
	Start->GetInfoSearch()->fHCost = D3DXVec3Length(&v);
	Start->GetInfoSearch()->fFCost = Start->GetInfoSearch()->fGCost + Start->GetInfoSearch()->fHCost;
	OpenList.push_back(Start->GetInfoSearch());

	// オープンリストが空になるまで探索
	while (!OpenList.empty())
	{
		CRoad::SInfoSearch* Current;			// 現在の参照ノード

		Current = *std::min_element(OpenList.begin(), OpenList.end(),
			[](CRoad::SInfoSearch* a, CRoad::SInfoSearch* b) { return a->fFCost < b->fFCost; });		// 総計コストが最少のノードを選ぶ

		// ゴール到達時
		if (Current->pConnectRoad == Goal)
		{// 親ノードを辿って順序を入れ替えたのち返す

			std::vector<CRoad::SInfoSearch*> path;	// 返り値用変数

			while (Current != nullptr) 
			{
				// 親がいるなら親の子に自分を設定
				if (Current->pParent != nullptr)
				{
					Current->pParent->pChild = Current;
				}

				path.push_back(Current);
				Current = Current->pParent;
			}

			// リストを反転させる
			std::reverse(path.begin(), path.end());
			return path;
		}

		// オープンリストから削除してクローズリストに追加
		OpenList.erase(std::remove(OpenList.begin(), OpenList.end(), Current), OpenList.end());
		CloseList.push_back(Current);

		for (int i = 0; i < CRoad::DIC_MAX; i++)
		{// 隣接ノードをコスト計算の後オープンリストに追加

			// 目的の方向に道が存在する
			if (Current->pConnectRoad->GetConnectRoad((CRoad::DIRECTION)i) == nullptr) { continue; }

			// 隣接した道の情報を取得
			CRoad::SInfoSearch* neighborInfo = Current->pConnectRoad->GetInfoSearchDic((CRoad::DIRECTION)i);

			// クローズリストに入っていない事を確認
			auto it = std::find_if
			(
				CloseList.begin(), CloseList.end(), [neighborInfo](const CRoad::SInfoSearch* obj)
			{
					return obj->pConnectRoad == neighborInfo->pConnectRoad;
				}
			);
			if (it != CloseList.end()) { continue; }

			// コスト計算
			neighborInfo->fGCost = Current->pConnectRoad->GetConnectLength((CRoad::DIRECTION)i);
			v = (neighborInfo->pConnectRoad->GetPosition() - Goal->GetPosition());
			neighborInfo->fHCost = D3DXVec3Length(&v);
			neighborInfo->fFCost = neighborInfo->fGCost + neighborInfo->fHCost;

			// 親に設定
			neighborInfo->pParent = Current;

			// オープンリストに追加
			if (std::find(OpenList.begin(), OpenList.end(), neighborInfo) == OpenList.end())
			{
				OpenList.push_back(neighborInfo);
			}
		}
	}

	return {};
}

//==========================================================
// AStarでの警察用迂回経路探索
//==========================================================
std::vector<CRoad::SInfoSearch*> AStarPoliceDetour(CRoad* Start, CRoad* Goal, CRoad* Arg)
{
	if (Start == nullptr || Goal == nullptr) { return {}; }

	std::vector < CRoad::SInfoSearch* > OpenList;		//探索候補リスト
	std::vector<CRoad::SInfoSearch*> CloseList;			//探索終了リスト

	Start->GetInfoSearch()->fGCost = 0.0f;
	D3DXVECTOR3 v = (Start->GetPosition() - Goal->GetPosition());
	Start->GetInfoSearch()->fHCost = D3DXVec3Length(&v);
	Start->GetInfoSearch()->fFCost = Start->GetInfoSearch()->fGCost + Start->GetInfoSearch()->fHCost;
	OpenList.push_back(Start->GetInfoSearch());

	// オープンリストが空になるまで探索
	while (!OpenList.empty())
	{
		CRoad::SInfoSearch* Current;			// 現在の参照ノード

		Current = *std::min_element(OpenList.begin(), OpenList.end(),
			[](CRoad::SInfoSearch* a, CRoad::SInfoSearch* b) { return a->fFCost < b->fFCost; });		// 総計コストが最少のノードを選ぶ

		// ゴール到達時
		if (Current->pConnectRoad == Goal)
		{// 親ノードを辿って順序を入れ替えたのち返す

			std::vector<CRoad::SInfoSearch*> path;	// 返り値用変数

			while (Current != nullptr)
			{
				// 親がいるなら親の子に自分を設定
				if (Current->pParent != nullptr)
				{
					Current->pParent->pChild = Current;
				}

				path.push_back(Current);
				Current = Current->pParent;
			}

			// リストを反転させる
			std::reverse(path.begin(), path.end());
			return path;
		}

		// オープンリストから削除してクローズリストに追加
		OpenList.erase(std::remove(OpenList.begin(), OpenList.end(), Current), OpenList.end());
		CloseList.push_back(Current);

		for (int i = 0; i < CRoad::DIC_MAX; i++)
		{// 隣接ノードをコスト計算の後オープンリストに追加

			// 目的の方向に道が存在する
			if (Current->pConnectRoad->GetConnectRoad((CRoad::DIRECTION)i) == nullptr) { continue; }

			// 隣接した道の情報を取得
			CRoad::SInfoSearch* neighborInfo = Current->pConnectRoad->GetInfoSearchDic((CRoad::DIRECTION)i);

			// クローズリストに入っていない事を確認
			auto it = std::find_if
			(
				CloseList.begin(), CloseList.end(), [neighborInfo](const CRoad::SInfoSearch* obj)
				{
					return obj->pConnectRoad == neighborInfo->pConnectRoad;
				}
			);
			if (it != CloseList.end())
			{ 
				continue;
			}

			//if (std::find(CloseList.begin(), CloseList.end(), neighborInfo) != CloseList.end()) { continue; }

			// コスト計算
			neighborInfo->fGCost = Current->pConnectRoad->GetConnectLength((CRoad::DIRECTION)i);

			if (Arg == neighborInfo->pConnectRoad)
			{
				neighborInfo->fGCost = FLT_MAX * 0.1f;
			}

			v = (neighborInfo->pConnectRoad->GetPosition() - Goal->GetPosition());
			neighborInfo->fHCost = D3DXVec3Length(&v);
			neighborInfo->fFCost = neighborInfo->fGCost + neighborInfo->fHCost;

			// 親に設定
			neighborInfo->pParent = Current;

			// オープンリストに追加
			if (std::find(OpenList.begin(), OpenList.end(), neighborInfo) == OpenList.end())
			{
				OpenList.push_back(neighborInfo);
			}
		}
	}

	return {};
}

//==========================================================
// AStarでの回数制限付き経路探索
//==========================================================
std::vector<CRoad::SInfoSearch*> AStarPoliceLimit(CRoad* Start, CRoad* Goal, int nLimitSearch)
{
	std::vector < CRoad::SInfoSearch* > OpenList;	//探索候補リスト
	std::vector<CRoad::SInfoSearch*> CloseList;		//探索終了リスト
	int nCntSearch = 0;								// 探索回数

	Start->GetInfoSearch()->fGCost = 0.0f;
	D3DXVECTOR3 v = (Start->GetPosition() - Goal->GetPosition());
	Start->GetInfoSearch()->fHCost = D3DXVec3Length(&v);
	Start->GetInfoSearch()->fFCost = Start->GetInfoSearch()->fGCost + Start->GetInfoSearch()->fHCost;
	OpenList.push_back(Start->GetInfoSearch());

	// オープンリストが空になるまで探索
	while (!OpenList.empty())
	{
		CRoad::SInfoSearch* Current;		// 現在の参照ノード
		Current = *std::min_element(OpenList.begin(), OpenList.end(),
			[](CRoad::SInfoSearch* a, CRoad::SInfoSearch* b) { return a->fFCost < b->fFCost; });		// 総計コストが最少のノードを選ぶ

		// ゴール到達時か一定回数探索時
		if (Current->pConnectRoad == Goal || nCntSearch == nLimitSearch)
		{// 親ノードを辿って順序を入れ替えたのち返す

			std::vector<CRoad::SInfoSearch*> path;

			while (Current != nullptr)
			{
				if (Current->pParent != nullptr)
				{
					Current->pParent->pChild = Current;
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

			if (Current->pConnectRoad->GetConnectRoad((CRoad::DIRECTION)i) == nullptr) { continue; }

			CRoad::SInfoSearch* neighborInfo = Current->pConnectRoad->GetInfoSearchDic((CRoad::DIRECTION)i);

			if (std::find(CloseList.begin(), CloseList.end(), neighborInfo) != CloseList.end()) { continue; }

			// コスト計算
			neighborInfo->fGCost = Current->pConnectRoad->GetConnectLength((CRoad::DIRECTION)i);
			v = (neighborInfo->pConnectRoad->GetPosition() - Goal->GetPosition());
			neighborInfo->fHCost = D3DXVec3Length(&v);
			neighborInfo->fFCost = neighborInfo->fGCost + neighborInfo->fHCost;
			neighborInfo->pParent = Current;

			// オープンリストに追加
			if (std::find(OpenList.begin(), OpenList.end(), neighborInfo) == OpenList.end())
			{
				OpenList.push_back(neighborInfo);
			}
		}

		// 探索回数を増加
		nCntSearch++;
	}

	return {};
}
}
