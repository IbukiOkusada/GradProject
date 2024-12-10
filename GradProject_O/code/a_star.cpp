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
std::vector<CRoad::SSearch*> AStar(CRoad::SSearch* Start, CRoad::SSearch* Goal)
{
	if (Start == nullptr || Goal == nullptr) return {};

	std::vector<CRoad::SSearch*> OpenList;//�T����⃊�X�g
	std::vector<CRoad::SSearch*> CloseList;//�T���I�����X�g
	
	Start->fGCost = 0.0f;
	Start->fHCost = D3DXVec3Length(&(Start->pos - Goal->pos));
	Start->fFCost = Start->fGCost + Start->fHCost;
	OpenList.push_back(Start);

	// �I�[�v�����X�g����ɂȂ�܂ŒT��
	while (!OpenList.empty())
	{
		CRoad::SSearch* Current;		// ���݂̎Q�ƃm�[�h
		Current = *std::min_element(OpenList.begin(), OpenList.end(),
			[](CRoad::SSearch* a, CRoad::SSearch* b) { return a->fFCost < b->fFCost; });		// ���v�R�X�g���ŏ��̃m�[�h��I��

		if (Current->pRoad == Goal->pRoad)		// �S�[�����B��
		{// �e�m�[�h��H���ď��������ւ����̂��Ԃ�

			std::vector<CRoad::SSearch*> path;

			while (Current != nullptr) 
			{
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
			neighbor->fHCost = D3DXVec3Length(&(neighbor->pos - Goal->pos));
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
std::vector<CRoad::SInfoSearch*> AStarPolice(CRoad* Start, CRoad* Goal)
{
	if(Start == nullptr || Goal == nullptr){ return {}; }

	std::vector < CRoad::SInfoSearch * > OpenList;		//�T����⃊�X�g
	std::vector<CRoad::SInfoSearch*> CloseList;			//�T���I�����X�g

	Start->GetInfoSearch()->fGCost = 0.0f;
	Start->GetInfoSearch()->fHCost = D3DXVec3Length(&(Start->GetPosition() - Goal->GetPosition()));
	Start->GetInfoSearch()->fFCost = Start->GetInfoSearch()->fGCost + Start->GetInfoSearch()->fHCost;
	OpenList.push_back(Start->GetInfoSearch());

	// �I�[�v�����X�g����ɂȂ�܂ŒT��
	while (!OpenList.empty())
	{
		CRoad::SInfoSearch* Current;			// ���݂̎Q�ƃm�[�h

		Current = *std::min_element(OpenList.begin(), OpenList.end(),
			[](CRoad::SInfoSearch* a, CRoad::SInfoSearch* b) { return a->fFCost < b->fFCost; });		// ���v�R�X�g���ŏ��̃m�[�h��I��

		// �S�[�����B��
		if (Current->pConnectRoad == Goal)
		{// �e�m�[�h��H���ď��������ւ����̂��Ԃ�

			std::vector<CRoad::SInfoSearch*> path;	// �Ԃ�l�p�ϐ�

			while (Current != nullptr) 
			{
				// �e������Ȃ�e�̎q�Ɏ�����ݒ�
				if (Current->pParent != nullptr)
				{
					Current->pParent->pChild = Current;
				}

				path.push_back(Current);
				Current = Current->pParent;
			}

			// ���X�g�𔽓]������
			std::reverse(path.begin(), path.end());
			return path;
		}

		// �I�[�v�����X�g����폜���ăN���[�Y���X�g�ɒǉ�
		OpenList.erase(std::remove(OpenList.begin(), OpenList.end(), Current), OpenList.end());
		CloseList.push_back(Current);

		for (int i = 0; i < CRoad::DIC_MAX; i++)
		{// �אڃm�[�h���R�X�g�v�Z�̌�I�[�v�����X�g�ɒǉ�

			// �ړI�̕����ɓ������݂���
			if (Current->pConnectRoad->GetConnectRoad((CRoad::DIRECTION)i) == nullptr) { continue; }

			// �אڂ������̏����擾
			CRoad::SInfoSearch* neighborInfo = Current->pConnectRoad->GetInfoSearchDic((CRoad::DIRECTION)i);

			// �N���[�Y���X�g�ɓ����Ă��Ȃ������m�F
			if (std::find(CloseList.begin(), CloseList.end(), neighborInfo) != CloseList.end()) { continue; }

			// �R�X�g�v�Z
			neighborInfo->fGCost = Current->pConnectRoad->GetConnectLength((CRoad::DIRECTION)i);
			neighborInfo->fHCost = D3DXVec3Length(&(neighborInfo->pConnectRoad->GetPosition() - Goal->GetPosition()));
			neighborInfo->fFCost = neighborInfo->fGCost + neighborInfo->fHCost;

			// �e�ɐݒ�
			neighborInfo->pParent = Current;

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
// AStar�ł̉I��o�H�T��
//==========================================================
std::vector<CRoad::SInfoSearch*> AStarPoliceDetour(CRoad* Start, CRoad* Relay, CRoad* Goal)
{
	std::vector<CRoad::SInfoSearch*> pathRelay;
	std::vector<CRoad::SInfoSearch*> pathGoal;

	// �����n�_���璆�p�n�_�܂ŒT��
	pathRelay = AStarPolice(Start, Relay);

	// ���p�n�_����ڕW�n�_�܂ŒT��
	pathGoal = AStarPolice(Relay, Goal);

	pathRelay.back()->pChild = pathGoal.front();
	pathGoal.front()->pParent = pathRelay.back();
	std::copy(pathGoal.begin(), pathGoal.end(), std::back_inserter(pathRelay));

	if (pathRelay.empty()) { return {}; }

	return pathRelay;
}

//==========================================================
// AStar�ł̉񐔐����t���o�H�T��
//==========================================================
std::vector<CRoad::SInfoSearch*> AStarPoliceLimit(CRoad* Start, CRoad* Goal, int nLimitSearch)
{
	std::vector < CRoad::SInfoSearch* > OpenList;	//�T����⃊�X�g
	std::vector<CRoad::SInfoSearch*> CloseList;		//�T���I�����X�g
	int nCntSearch = 0;								// �T����

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

		// �S�[�����B�������񐔒T����
		if (Current->pConnectRoad == Goal || nCntSearch == nLimitSearch)
		{// �e�m�[�h��H���ď��������ւ����̂��Ԃ�

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

		// �I�[�v�����X�g����폜���ăN���[�Y���X�g�ɒǉ�
		OpenList.erase(std::remove(OpenList.begin(), OpenList.end(), Current), OpenList.end());
		CloseList.push_back(Current);

		for (int i = 0; i < CRoad::DIC_MAX; i++)
		{// �אڃm�[�h���R�X�g�v�Z�̌�I�[�v�����X�g�ɒǉ�

			if (Current->pConnectRoad->GetConnectRoad((CRoad::DIRECTION)i) == nullptr) { continue; }

			CRoad::SInfoSearch* neighborInfo = Current->pConnectRoad->GetInfoSearchDic((CRoad::DIRECTION)i);

			if (std::find(CloseList.begin(), CloseList.end(), neighborInfo) != CloseList.end()) { continue; }

			// �R�X�g�v�Z
			neighborInfo->fGCost = Current->pConnectRoad->GetConnectLength((CRoad::DIRECTION)i);
			neighborInfo->fHCost = D3DXVec3Length(&(neighborInfo->pConnectRoad->GetPosition() - Goal->GetPosition()));
			neighborInfo->fFCost = neighborInfo->fGCost + neighborInfo->fHCost;
			neighborInfo->pParent = Current;

			// �I�[�v�����X�g�ɒǉ�
			if (std::find(OpenList.begin(), OpenList.end(), neighborInfo) == OpenList.end())
			{
				OpenList.push_back(neighborInfo);
			}
		}

		// �T���񐔂𑝉�
		nCntSearch++;
	}

	return {};
}
}
