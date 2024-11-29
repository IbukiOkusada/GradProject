//==========================================================
//
// AStar [a_star.cpp]
// Author : �O�엳�V��
//
//==========================================================
#include "a_star.h"

namespace AStar
{
//==========================================================
// AStar�ł̌o�H�T��
//==========================================================
std::vector<CRoad::SSearch*> AStar(CRoad::SSearch* State, CRoad::SSearch* Gole)
{
	std::vector<CRoad::SSearch*> OpenList;//�T����⃊�X�g
	std::vector<CRoad::SSearch*> CloseList;//�T���I�����X�g
	
	State->fGCost = 0.0f;
	State->fHCost = D3DXVec3Length(&(State->pos - Gole->pos));
	State->fFCost = State->fGCost + State->fHCost;
	OpenList.push_back(State);

	// �I�[�v�����X�g����ɂȂ�܂ŒT��
	while (!OpenList.empty())
	{
		CRoad::SSearch* Current;		// ���݂̎Q�ƃm�[�h
		Current = *std::min_element(OpenList.begin(), OpenList.end(),
			[](CRoad::SSearch* a, CRoad::SSearch* b) { return a->fFCost < b->fFCost; });		// ���v�R�X�g���ŏ��̃m�[�h��I��

		if (Current->pRoad == Gole->pRoad)		// �S�[�����B��
		{// �e�m�[�h��H���ď��������ւ����̂��Ԃ�

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

		// �I�[�v�����X�g����폜���ăN���[�Y���X�g�ɒǉ�
		OpenList.erase(std::remove(OpenList.begin(), OpenList.end(), Current), OpenList.end());
		CloseList.push_back(Current);

		for (int i = 0; i < CRoad::DIC_MAX; i++)
		{// �אڃm�[�h���R�X�g�v�Z�̌�I�[�v�����X�g�ɒǉ�

			CRoad::SSearch* neighbor = Current->pRoad->GetSearchRoad((CRoad::DIRECTION)i);

			if (std::find(CloseList.begin(), CloseList.end(), neighbor) != CloseList.end() || !neighbor->bActive) { continue; }
			
			// �R�X�g�v�Z
			neighbor->fGCost = D3DXVec3Length(&(neighbor->pos - Current->pos));
			neighbor->fHCost = D3DXVec3Length(&(neighbor->pos - Gole->pos));
			neighbor->fFCost = neighbor->fGCost + neighbor->fHCost;
			neighbor->pParent = Current;

			// �I�[�v�����X�g�ɒǉ�
			if (std::find(OpenList.begin(), OpenList.end(), neighbor) == OpenList.end())
			{
				OpenList.push_back(neighbor);
			}
		}
	}
	return {};
}

//==========================================================
// AStar�ł̌x�@�p�o�H�T��
//==========================================================
std::vector<CRoad*> AStarPolice(CRoad* Start, CRoad* Goal)
{
	std::vector < CRoad::SInfoSearch * > OpenList;//�T����⃊�X�g
	std::vector<CRoad::SInfoSearch*> CloseList;//�T���I�����X�g

	Start->GetInfoSearch()->fGCost = 0.0f;
	Start->GetInfoSearch()->fHCost = D3DXVec3Length(&(Start->GetPosition() - Goal->GetPosition()));
	Start->GetInfoSearch()->fFCost = Start->GetInfoSearch()->fGCost + Start->GetInfoSearch()->fHCost;
	OpenList.push_back(Start->GetInfoSearch());

	// �I�[�v�����X�g����ɂȂ�܂ŒT��
	while (!OpenList.empty())
	{
		CRoad::SInfoSearch* Current;		// ���݂̎Q�ƃm�[�h
		Current = *std::min_element(OpenList.begin(), OpenList.end(),
			[](CRoad::SInfoSearch* a, CRoad::SInfoSearch* b) { return a->fFCost < b->fFCost; });		// ���v�R�X�g���ŏ��̃m�[�h��I��

		if (Current->pRoad == Goal)		// �S�[�����B��
		{// �e�m�[�h��H���ď��������ւ����̂��Ԃ�

			std::vector<CRoad*> path;

			while (Current != nullptr) 
			{
				if (Current->pParent != nullptr)
				{
					Current->pParent->GetInfoSearch()->pChaild = Current->pRoad;
				}
				path.push_back(Current->pRoad);
				Current = Current->pParent->GetInfoSearch();
			}

			std::reverse(path.begin(), path.end());
			return path;
		}

		// �I�[�v�����X�g����폜���ăN���[�Y���X�g�ɒǉ�
		OpenList.erase(std::remove(OpenList.begin(), OpenList.end(), Current), OpenList.end());
		CloseList.push_back(Current);

		for (int i = 0; i < CRoad::DIC_MAX; i++)
		{// �אڃm�[�h���R�X�g�v�Z�̌�I�[�v�����X�g�ɒǉ�

			if (Current->pRoad->GetConnectRoad((CRoad::DIRECTION)i) == nullptr) { continue; }

			CRoad::SInfoSearch* neighborInfo = Current->pRoad->GetInfoSearchDic((CRoad::DIRECTION)i);

			if (std::find(CloseList.begin(), CloseList.end(), neighborInfo) != CloseList.end()) { continue; }

			// �R�X�g�v�Z
			neighborInfo->fGCost = Current->pRoad->GetConnectLength((CRoad::DIRECTION)i);
			neighborInfo->fHCost = D3DXVec3Length(&(neighborInfo->pRoad->GetPosition() - Goal->GetPosition()));
			neighborInfo->fFCost = neighborInfo->fGCost + neighborInfo->fHCost;
			neighborInfo->pParent = Current->pRoad;

			// �I�[�v�����X�g�ɒǉ�
			if (std::find(OpenList.begin(), OpenList.end(), neighborInfo) == OpenList.end())
			{
				OpenList.push_back(neighborInfo);
			}
		}
	}
	return {};
}

//==========================================================
// AStar�ł̉񐔐����t���o�H�T��
//==========================================================
std::vector<CRoad::SSearch*> AStarLimit(CRoad::SSearch* State, CRoad::SSearch* Gole, int nLimitSearch)
{
	std::vector<CRoad::SSearch*> OpenList;		// �T����⃊�X�g
	std::vector<CRoad::SSearch*> CloseList;		// �T���I�����X�g
	int nCntSearch = 0;							// �T����

	State->fGCost = 0.0f;
	State->fHCost = D3DXVec3Length(&(State->pos - Gole->pos));
	State->fFCost = State->fGCost + State->fHCost;
	OpenList.push_back(State);

	// �I�[�v�����X�g����ɂȂ�܂ŒT��
	while (!OpenList.empty())
	{
		CRoad::SSearch* Current;		// ���݂̎Q�ƃm�[�h
		Current = *std::min_element(OpenList.begin(), OpenList.end(),
			[](CRoad::SSearch* a, CRoad::SSearch* b) { return a->fFCost < b->fFCost; });		// ���v�R�X�g���ŏ��̃m�[�h��I��

		if (Current->pRoad == Gole->pRoad || nCntSearch == nLimitSearch)		// �S�[�����B��
		{// �e�m�[�h��H���ď��������ւ����̂��Ԃ�

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

		// �I�[�v�����X�g����폜���ăN���[�Y���X�g�ɒǉ�
		OpenList.erase(std::remove(OpenList.begin(), OpenList.end(), Current), OpenList.end());
		CloseList.push_back(Current);

		for (int i = 0; i < CRoad::DIC_MAX; i++)
		{// �אڃm�[�h���R�X�g�v�Z�̌�I�[�v�����X�g�ɒǉ�

			CRoad::SSearch* neighbor = Current->pRoad->GetSearchRoad((CRoad::DIRECTION)i);

			if (std::find(CloseList.begin(), CloseList.end(), neighbor) != CloseList.end() || !neighbor->bActive) { continue; }

			// �R�X�g�v�Z
			neighbor->fGCost = Current->pRoad->GetConnectLength((CRoad::DIRECTION)i);
			neighbor->fHCost = D3DXVec3Length(&(neighbor->pos - Gole->pos));
			neighbor->fFCost = neighbor->fGCost + neighbor->fHCost;
			neighbor->pParent = Current;

			// �I�[�v�����X�g�ɒǉ�
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
