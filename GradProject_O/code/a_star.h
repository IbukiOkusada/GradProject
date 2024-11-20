//==========================================================
//
// AStar [a_star.cpp]
// Author : �O�엳�V��
//
//==========================================================
#ifndef _Astar_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _Astar_H_		// ��d�C���N���[�h�h�~�p�}�N�����`
#include "road.h"
#include "convenience.h"

namespace AStar
{
	// AStar�ł̌o�H�T��
	std::vector<CRoad::SSearch*> AStar(CRoad::SSearch* State, CRoad::SSearch* Gole);

	// AStar�ł̐����t���o�H�T��
	std::vector<CRoad::SSearch*> AStarLimit(CRoad::SSearch* State, CRoad::SSearch* Gole, int nCntSearch);
}

#endif