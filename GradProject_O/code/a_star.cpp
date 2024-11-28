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

		// �I�[�v�����X�g����폜���ăN���[�Y���X�g�ɒǉ�
		OpenList.erase(std::remove(OpenList.begin(), OpenList.end(), Current), OpenList.end());
		CloseList.push_back(Current);

		for (int i = 0; i < CRoad::DIC_MAX; i++)
		{// �אڃm�[�h���R�X�g�v�Z�̌�I�[�v�����X�g�ɒǉ�

			CRoad::SSearch* neighbor = Current->pRoad->GetSearchRoad((CRoad::DIRECTION)i);

			if (std::find(CloseList.begin(), CloseList.end(), neighbor) != CloseList.end() || !neighbor->bActive) continue;
			
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

// AStar�ł̌o�H�T��
std::vector<CRoad::SSearch*> AStarPolice(CRoad::SSearch* State, CRoad::SSearch* Gole)
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

		// �I�[�v�����X�g����폜���ăN���[�Y���X�g�ɒǉ�
		OpenList.erase(std::remove(OpenList.begin(), OpenList.end(), Current), OpenList.end());
		CloseList.push_back(Current);

		for (int i = 0; i < CRoad::DIC_MAX; i++)
		{// �אڃm�[�h���R�X�g�v�Z�̌�I�[�v�����X�g�ɒǉ�

			if (Current->pRoad->GetConnectRoad((CRoad::DIRECTION)i) == nullptr)
				continue;

			CRoad::SSearch* neighbor = Current->pRoad->GetSearchRoad((CRoad::DIRECTION)i);

			if (std::find(CloseList.begin(), CloseList.end(), neighbor) != CloseList.end()) continue;

			// �R�X�g�v�Z
			neighbor->fGCost = Current->pRoad->GetConnectLength((CRoad::DIRECTION)i);
			neighbor->fHCost = D3DXVec3Length(&(neighbor->pRoad->GetPosition() - Gole->pos));
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

			if (std::find(CloseList.begin(), CloseList.end(), neighbor) != CloseList.end() || !neighbor->bActive) continue;

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
