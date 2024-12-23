//==========================================================
//
// �v���C���[�}�l�[�W���[ [car_manager.cpp]
// Author : Riku Nakamura
//
//==========================================================
#include "car_manager.h"
#include "car.h"
#include "police.h"
#include "add_police.h"
#include "player_manager.h"
#include "network.h"

// �ÓI�����o�ϐ��錾
CCarManager* CCarManager::m_pInstance = nullptr;	// �C���X�^���X

//==========================================================
// �R���X�g���N�^
//==========================================================
CCarManager::CCarManager() : 
m_pList(nullptr),
m_nNum(0),
m_List(),
m_NextList(),
m_NextTempList()
{
	// �l�̃N���A
}

//==========================================================
// �f�X�g���N�^
//==========================================================
CCarManager::~CCarManager()
{

}

//==========================================================
// ����������
//==========================================================
HRESULT CCarManager::Init(void)
{
	return S_OK;
}

//==========================================================
// �I������
//==========================================================
void CCarManager::Uninit(void)
{
	// �e�N���X�̏I������
	CListManager::Uninit();

	// ���X�g�p��
	ListRelease();

	m_List.Clear();

	m_NextList.Clear();
	m_NextTempList.Clear();

	// �C���X�^���X�̔p��
	if (m_pInstance != nullptr) {	// �C���X�^���X���m�ۂ���Ă���
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

//==========================================================
// �X�V����
//==========================================================
void CCarManager::Update(void)
{
	m_bSet = true;

	auto list = *m_NextList.GetList();

	for (const auto& it : list)
	{
		// ���ɑ��݂���ꍇ�͐������Ȃ�
		CCar* pCar = m_List.Get(it.first);
		if (pCar != nullptr) { continue; }

		auto info = it.second;

		// ��ނ��Ƃɐ���
		switch (it.second->type)
		{
		case CCar::CAR_TYPE::CAR_TYPE_CAR:
		{
			pCar = CCar::Create(info->pos, info->rot, info->move, it.first);
			pCar->SetType(CCar::TYPE::TYPE_RECV);
		}
			break;

		case CCar::CAR_TYPE::CAR_TYPE_POLICE:
		{
			// �����̌x�@�𐶐�
			CPolice* pP = CPolice::Create(info->pos, info->rot, info->move, it.first);
			CPlayer* pPlayer = CPlayerManager::GetInstance()->GetPlayer(info->nChaseId);

			pP->SetType(CCar::TYPE::TYPE_RECV);
			//pP->SetChaseNext(info->chase);

			//if (info->nChaseId == CNetWork::GetInstance()->GetIdx())
			//{
			//	// �������g
			//	pP->SetTypeNext(CCar::TYPE::TYPE_ACTIVE);
			//	pP->SetNextPlayer(pPlayer);
			//}
			//else
			//{
			//	pP->SetTypeNext(CCar::TYPE::TYPE_RECV);
			//	pP->SetNextPlayer(pPlayer);
			//}

			pCar = pP;
		}
			break;

		case CCar::CAR_TYPE::CAR_TYPE_ADDPOLICE:
		{
			// �����̌x�@�𐶐�
			CAddPolice* pP = CAddPolice::Create(info->pos, info->rot, info->move, it.first);
			CPlayer* pPlayer = CPlayerManager::GetInstance()->GetPlayer(info->nChaseId);

			if (pP == nullptr) { continue; }

			pP->SetType(CCar::TYPE::TYPE_RECV);

			//if (info->chase == CPolice::CHASE::CHASE_BEGIN)
			//{
			//	// �ǐՏ�ԂɕύX
			//	pP->SetChase(true);
			//	pP->GetAi()->BeginChase(pPlayer);

			//	// �����̌x�@�͉������Ă΂Ȃ��悤�ɂ���
			//	pP->GetAi()->SetCall(true);
			//	pP->SetChaseNext(info->chase);
			//}
			//else if (info->chase == CPolice::CHASE::CHASE_END)
			//{
			//	pP->SetChaseNext(info->chase);
			//}

			//// �����̌x�@�̃^�C�v��ݒ�
			//pP->SetTypeAI((CPoliceAI::TYPE)(rand() % CPoliceAI::TYPE_MAX));
			//pP->SetTypeAI(CPoliceAI::TYPE_ELITE);
			//pP->SetType(CCar::TYPE::TYPE_ACTIVE);

			//// �ړI�n�ݒ�
			//pP->SetRoadTarget(nullptr);

			//if (info->nChaseId == CNetWork::GetInstance()->GetIdx())
			//{
			//	// �������g
			//	pP->SetTypeNext(CCar::TYPE::TYPE_ACTIVE);
			//	pP->SetNextPlayer(pPlayer);
			//}
			//else
			//{
			//	pP->SetTypeNext(CCar::TYPE::TYPE_RECV);
			//	pP->SetNextPlayer(pPlayer);
			//}

			pCar = pP;
		}
			break;

		default:
		{

		}
			break;
		}
	}


	// ���f�[�^�����ւ���
	m_NextList.Clear();
	m_NextList.ResetInCnt();
	m_NextList = m_NextTempList;

	m_bSet = false;
	m_NextTempList.Clear();
	m_NextTempList.ResetInCnt();
}

//==========================================================
// �C���X�^���X���m��
//==========================================================
CCarManager* CCarManager::GetInstance(void)
{
	if (m_pInstance == nullptr) {	// �g���Ă��Ȃ�
		m_pInstance = DEBUG_NEW CCarManager;
	}

	return m_pInstance;
}

//==========================================================
// �C���X�^���X�������[�X
//==========================================================
void CCarManager::Release(void)
{
	if (m_pInstance != nullptr) {	// �C���X�^���X���m�ۂ���Ă���
		m_pInstance->Uninit();
	}
}

//==========================================================
// ���X�g�ɑ}��
//==========================================================
void CCarManager::ListIn(CCar* pCar)
{
	GetList()->Regist(pCar);
	IdListIn(pCar);
}

//==========================================================
// ���X�g����O��
//==========================================================
void CCarManager::ListOut(CCar* pCar)
{
	// ���X�g���玩�����g���폜����
	GetList()->Delete(pCar);
	IdListOut(pCar);
}

//==========================================================
// �������p�̃��X�g�ɑ}��
//==========================================================
void CCarManager::CreateListIn(NextCreateInfo& info, int nId)
{
	// ���ɕۑ��ς�
	if (m_NextList.IdFind(nId) || m_NextTempList.IdFind(nId)) { return; }

	if (!m_bSet)
	{
		m_NextList.Regist(nId, &info);
	}
	else
	{
		m_NextTempList.Regist(nId, &info);
	}
}

//==========================================================
// ���X�g����O��
//==========================================================
void CCarManager::CreateListOut(NextCreateInfo& info, int nId)
{
	// ���݂��Ȃ�
	if (m_NextList.IdFind(nId))
	{
		m_NextList.Delete(nId, &info);
	}
	else if(m_NextTempList.IdFind(nId))
	{
		m_NextTempList.Delete(nId, &info);
	}
}

//==========================================================
// ���̐���
//==========================================================
CCarManager::NextCreateInfo* CCarManager::CreateGet(int nId)
{
	if (m_NextList.IdFind(nId))
	{
		return m_NextList.Get(nId);
	}
	else if (m_NextTempList.IdFind(nId))
	{
		return m_NextTempList.Get(nId);
	}

	return nullptr;
}

//==========================================================
// �}�b�v���X�g�ɑ}��
//==========================================================
void CCarManager::IdListIn(CCar* pCar)
{
	m_List.Regist(pCar->GetId(),pCar);
}

//==========================================================
// �}�b�v���X�g����O��
//==========================================================
void CCarManager::IdListOut(CCar* pCar)
{
	// ���X�g���玩�����g���폜����
	m_List.Delete(pCar->GetId(), pCar);
}

//==========================================================
// �U���q�b�g�m�F
//==========================================================
bool CCarManager::Hit(D3DXVECTOR3& pos, const float fRange, const float fHeight, const int nDamage)
{
	bool bUse = false;

	//�ʔ���
	for (int i = 0; i < GetList()->GetNum(); i++)
	{
		//CCar* pCar = GetList()->Get(i);
		//if (pCar->HitCheck(pos, fRange, fHeight, nDamage)) {
			//bUse = true;
		//}
	}

	return bUse;
}
