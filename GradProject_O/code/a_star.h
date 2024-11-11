#ifndef _Astar_H_		// このマクロが定義されていない場合
#define _Astar_H_		// 二重インクルード防止用マクロを定義
#include "road.h"
#include "convenience.h"

std::vector<CRoad::SSearch*> AStar(CRoad::SSearch* State, CRoad::SSearch* Gole);
#endif