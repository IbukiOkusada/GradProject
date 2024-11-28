//==========================================================
//
// ���}�l�[�W���[ [road_manager.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "road_manager.h"
#include "road.h"
#include "gimmick_firehydrant.h"

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
	m_pList = nullptr;
	m_IdList.clear();
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

	ListRelease();
	m_IdList.clear();

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
	GetList()->Regist(pRoad);
	m_InfoList.Regist(pRoad->GetInfo());
}

//==========================================================
// ���X�g����O��
//==========================================================
void CRoadManager::ListOut(CRoad* pRoad)
{
	GetList()->Delete(pRoad);
	m_InfoList.Delete(pRoad->GetInfo());
}

//==========================================================
// ID�Ǘ��̃��X�g�ɑ}��
//==========================================================
void CRoadManager::IdListIn(CRoad* pRoad)
{
	m_IdList[pRoad->GetIdx()];
}

//==========================================================
// ID�Ǘ��̃��X�g�ɑ}��
//==========================================================
void CRoadManager::IdListOut(CRoad* pRoad)
{
	GetList()->Delete(pRoad);
	m_InfoList.Delete(pRoad->GetInfo());
}

//==========================================================
// �U���q�b�g�m�F
//==========================================================
bool CRoadManager::Hit(D3DXVECTOR3& pos, const float fRange, const float fHeight, const int nDamage)
{
	bool bUse = false;

	//�ʔ���
	for (int i = 0; i < GetList()->GetNum(); i++)
	{
		//CRoad* pRoad = GetList()->Get(i);
		//if (pRoad->HitCheck(pos, fRange, fHeight, nDamage)) {
			//bUse = true;
		//}
	}

	return bUse;
}

//==========================================================
// �S�ĘA��
//==========================================================
void CRoadManager::AllConnect(void)
{

	// ��x�Y��ɂ���
	for (int i = 0; i < GetList()->GetNum(); i++)
	{
		CRoad* pRoad = GetList()->Get(i);

		// �N���A
		pRoad->Connect(nullptr, CRoad::DIC_UP);
		pRoad->Connect(nullptr, CRoad::DIC_DOWN);
		pRoad->Connect(nullptr, CRoad::DIC_LEFT);
		pRoad->Connect(nullptr, CRoad::DIC_RIGHT);
	}

	// �S�Ċm�F
	for (int i = 0; i < GetList()->GetNum(); i++)
	{
		CRoad* pRoad = GetList()->Get(i);
		D3DXVECTOR3 pos = pRoad->GetPosition();	// ���W

		// ���ȍ~�Ōq�����Ă��邩���m�F����
		for (int j = i + 1; j < GetList()->GetNum(); j++)
		{
			CRoad* pCheck = GetList()->Get(j);
			D3DXVECTOR3 checkpos = pCheck->GetPosition();	// �m�F��̍��W

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
		}

		// �e�N�X�`���ݒ�
		pRoad->BindTexture();
	}

	// �o�H�T���p�����ݒ�
	SearchRoadConnect();
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

//==========================================================
// �o�H�T���p�A��
//==========================================================
void CRoadManager::SearchRoadConnect(void)
{
	for (int i = 0; i < GetList()->GetNum(); i++)
	{

		CRoad* pRoad = GetList()->Get(i);
		D3DXVECTOR3 pos = pRoad->GetPosition();	// ���W

		// �S���m�F
		for (int dic = 0; dic < CRoad::DIRECTION::DIC_MAX; dic++)
		{
			CRoad::DIRECTION direction = static_cast<CRoad::DIRECTION>(dic);
			CRoad* pConnect = pRoad->GetConnectRoad(direction);

			// �����A�����Ă��Ȃ�
			if(pConnect == nullptr)
			{
				continue;
			}

			// �A�����Ă���ꍇ�����_�������͍s���~�܂�܂Ŋm�F
			while (pConnect != nullptr)
			{
				// �A�����Ă���Ƃ���ŕ��򂷂�
				if (pConnect->GetType() != CRoad::TYPE::TYPE_NONE)
				{
					pRoad->SearchConnect(pConnect, direction);
					break;
				}

				// �X�ɐ�
				CRoad* pConnectNext = pConnect->GetConnectRoad(direction);
				pConnect = pConnectNext;
			}
		}
	}
}

//==========================================================
// �ł��߂����擾
//==========================================================
CRoad* CRoadManager::GetNearRoad(const D3DXVECTOR3& pos)
{
	float length = 1000000.0f;
	CRoad* pRoad = nullptr;
	// �������J��Ԃ�
	for (int i = 0; i < GetList()->GetNum(); i++)
	{
		// �m�F
		CRoad* pCheck = GetList()->Get(i);
		D3DXVECTOR3 vec = pCheck->GetPosition() - pos;
		float temp = D3DXVec3Length(&vec);
		
		// �������߂�
		if (temp <= length)
		{
			length = temp;
			pRoad = pCheck;
		}
	}

	return pRoad;
}