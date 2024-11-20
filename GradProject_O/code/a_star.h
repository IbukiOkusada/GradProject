//==========================================================
//
// AStar [a_star.cpp]
// Author : 丹野竜之介
//
//==========================================================
#ifndef _Astar_H_		// このマクロが定義されていない場合
#define _Astar_H_		// 二重インクルード防止用マクロを定義
#include "road.h"
#include "convenience.h"

namespace AStar
{
	// AStarでの経路探索
	std::vector<CRoad::SSearch*> AStar(CRoad::SSearch* State, CRoad::SSearch* Gole);

	// AStarでの制限付き経路探索
	std::vector<CRoad::SSearch*> AStarLimit(CRoad::SSearch* State, CRoad::SSearch* Gole, int nCntSearch);
}

#endif