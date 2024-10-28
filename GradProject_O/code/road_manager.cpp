//==========================================================
//
// ���}�l�[�W���[ [road_manager.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "road_manager.h"
#include "road.h"

// �ÓI�����o�ϐ��錾
CRoadManager* CRoadManager::m_pInstance = nullptr;	// �C���X�^���X

namespace
{
	const float HIT_SIZE = 1.0f;	// �A������덷���e�����
}

//==========================================================
// �R���X�g���N�^
//==========================================================
CRoadManager::CRoadManager()
{
	// �l�̃N���A
	m_pCur = nullptr;
	m_pTop = nullptr;
	m_nNum = 0;
}

//==========================================================
// �f�X�g���N�^
//==========================================================
CRoadManager::~CRoadManager()
{

}

//==========================================================
// ����������
//==========================================================
HRESULT CRoadManager::Init(void)
{
	return S_OK;
}

//==========================================================
// �I������
//==========================================================
void CRoadManager::Uninit(void)
{
	// �e�N���X�̏I������
	CListManager::Uninit();

	m_InfoList.Clear();

	// �C���X�^���X�̔p��
	if (m_pInstance != nullptr) {	// �C���X�^���X���m�ۂ���Ă���
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

//==========================================================
// �X�V����
//==========================================================
void CRoadManager::Update(void)
{

}

//==========================================================
// �C���X�^���X���m��
//==========================================================
CRoadManager* CRoadManager::GetInstance(void)
{
	if (m_pInstance == nullptr) {	// �g���Ă��Ȃ�
		m_pInstance = DEBUG_NEW CRoadManager;
	}

	return m_pInstance;
}

//==========================================================
// �C���X�^���X�������[�X
//==========================================================
void CRoadManager::Release(void)
{
	if (m_pInstance != nullptr) {	// �C���X�^���X���m�ۂ���Ă���
		m_pInstance->Uninit();
	}
}

//==========================================================
// ���X�g�ɑ}��
//==========================================================
void CRoadManager::ListIn(CRoad* pRoad)
{
	if (m_pTop != nullptr)
	{// �擪�����݂��Ă���ꍇ
		m_pCur->SetNext(pRoad);	// ���ݍŌ���̃I�u�W�F�N�g�̃|�C���^�ɂȂ���
		pRoad->SetPrev(m_pCur);
		m_pCur = pRoad;	// �������g���Ō���ɂȂ�
	}
	else
	{// ���݂��Ȃ��ꍇ
		m_pTop = pRoad;	// �������g���擪�ɂȂ�
		m_pCur = pRoad;	// �������g���Ō���ɂȂ�
	}

	m_InfoList.Regist(pRoad->GetInfo());

	m_nNum++;
}

//==========================================================
// ���X�g����O��
//==========================================================
void CRoadManager::ListOut(CRoad* pRoad)
{
	// ���X�g���玩�����g���폜����
	if (m_pTop == pRoad)
	{// ���g���擪
		if (pRoad->GetNext() != nullptr)
		{// �������݂��Ă���
			m_pTop = pRoad->GetNext();	// ����擪�ɂ���
			m_pTop->SetPrev(nullptr);	// ���̑O�̃|�C���^���o���Ă��Ȃ��悤�ɂ���
		}
		else
		{// ���݂��Ă��Ȃ�
			m_pTop = nullptr;	// �擪���Ȃ���Ԃɂ���
			m_pCur = nullptr;	// �Ō�����Ȃ���Ԃɂ���
		}
	}
	else if (m_pCur == pRoad)
	{// ���g���Ō��
		if (pRoad->GetPrev() != nullptr)
		{// �������݂��Ă���
			m_pCur = pRoad->GetPrev();		// �O���Ō���ɂ���
			m_pCur->SetNext(nullptr);			// �O�̎��̃|�C���^���o���Ă��Ȃ��悤�ɂ���
		}
		else
		{// ���݂��Ă��Ȃ�
			m_pTop = nullptr;	// �擪���Ȃ���Ԃɂ���
			m_pCur = nullptr;	// �Ō�����Ȃ���Ԃɂ���
		}
	}
	else
	{
		if (pRoad->GetNext() != nullptr)
		{
			pRoad->GetNext()->SetPrev(pRoad->GetPrev());	// ���g�̎��ɑO�̃|�C���^���o��������
		}
		if (pRoad->GetPrev() != nullptr)
		{
			pRoad->GetPrev()->SetNext(pRoad->GetNext());	// ���g�̑O�Ɏ��̃|�C���^���o��������
		}
	}

	m_nNum--;
}

//==========================================================
// �U���q�b�g�m�F
//==========================================================
bool CRoadManager::Hit(D3DXVECTOR3& pos, const float fRange, const float fHeight, const int nDamage)
{
	CRoad* pRoad = m_pTop;
	bool bUse = false;

	//�ʔ���
	while (pRoad != nullptr) {
		CRoad* pRoadNext = pRoad->GetNext();
		//if (pRoad->HitCheck(pos, fRange, fHeight, nDamage)) {
			//bUse = true;
		//}
		pRoad = pRoadNext;
	}

	return bUse;
}

//==========================================================
// �S�ĘA��
//==========================================================
void CRoadManager::AllConnect(void)
{
	CRoad* pRoad = GetTop();

	// ��x�Y��ɂ���
	while (pRoad != nullptr)
	{
		CRoad* pNext = pRoad->GetNext();	// �ێ�

		// �N���A
		pRoad->Connect(nullptr, CRoad::DIC_UP);
		pRoad->Connect(nullptr, CRoad::DIC_DOWN);
		pRoad->Connect(nullptr, CRoad::DIC_LEFT);
		pRoad->Connect(nullptr, CRoad::DIC_RIGHT);

		// ���Ɉړ�
		pRoad = pNext;
	}

	pRoad = GetTop();

	// �S�Ċm�F
	while (pRoad != nullptr)
	{
		D3DXVECTOR3 pos = pRoad->GetPosition();	// ���W

		CRoad* pNext = pRoad->GetNext();	// �ێ�
		CRoad* pCheck = pNext;	// �m�F�p��������ɂ���

		// ���ȍ~�Ōq�����Ă��邩���m�F����
		while (pCheck != nullptr)
		{
			D3DXVECTOR3 checkpos = pCheck->GetPosition();	// �m�F��̍��W

			CRoad* pCheckNext = pCheck->GetNext();	// �ێ�

			// ���W�m�F
			if (pos.x + HIT_SIZE >= checkpos.x && pos.x - HIT_SIZE <= checkpos.x)
			{ // x���W��v
				// �c�m�F
				VerticalConnectCheck(pRoad, pCheck);
			}
			else if (pos.z + HIT_SIZE >= checkpos.z && pos.z - HIT_SIZE <= checkpos.z)
			{// z���W��v
				// ���m�F
				SideConnectCheck(pRoad, pCheck);
			}

			pCheck = pCheckNext;
		}

		// �e�N�X�`���ݒ�
		pRoad->BindTexture();

		// ���Ɉړ�
		pRoad = pNext;
	}

	int a = 0;
}

//==========================================================
// ���A���m�F
//==========================================================
void CRoadManager::SideConnectCheck(CRoad* pRoad, CRoad* pCheckRoad)
{
	// ���W�擾
	D3DXVECTOR3 pos = pRoad->GetPosition();	// ���W
	D3DXVECTOR3 checkpos = pCheckRoad->GetPosition();	// �m�F��̍��W

	// �T�C�Y�擾
	float size = pRoad->GetSize().x;
	float checksize = pCheckRoad->GetSize().x;

	// ���������߂�
	float allsize = size + checksize;
	float diff = fabsf(pos.x - checkpos.x);

	// ������̎��̂݋L��������
	if (diff + HIT_SIZE < allsize || diff - HIT_SIZE > allsize) { return; }
	
	if (pos.x >= checkpos.x)
	{// �{�̂̕������W���傫��
		pRoad->Connect(pCheckRoad, CRoad::DIC_LEFT);
		pCheckRoad->Connect(pRoad, CRoad::DIC_RIGHT);
	}
	else
	{// ������
		pRoad->Connect(pCheckRoad, CRoad::DIC_RIGHT);
		pCheckRoad->Connect(pRoad, CRoad::DIC_LEFT);
	}
}

//==========================================================
// �c�A���m�F
//==========================================================
void CRoadManager::VerticalConnectCheck(CRoad* pRoad, CRoad* pCheckRoad)
{
	// ���W�擾
	D3DXVECTOR3 pos = pRoad->GetPosition();	// ���W
	D3DXVECTOR3 checkpos = pCheckRoad->GetPosition();	// �m�F��̍��W

	// �T�C�Y�擾
	float size = pRoad->GetSize().y;
	float checksize = pCheckRoad->GetSize().y;

	// ���������߂�
	float allsize = size + checksize;
	float diff = fabsf(pos.z - checkpos.z);

	// ������̎��̂݋L��������
	if (diff + HIT_SIZE < allsize || diff - HIT_SIZE > allsize) { return; }

	if (pos.z >= checkpos.z)
	{// �{�̂̕������W���傫��
		pRoad->Connect(pCheckRoad, CRoad::DIC_DOWN);
		pCheckRoad->Connect(pRoad, CRoad::DIC_UP);
	}
	else
	{// ������
		pRoad->Connect(pCheckRoad, CRoad::DIC_UP);
		pCheckRoad->Connect(pRoad, CRoad::DIC_DOWN);
	}
}