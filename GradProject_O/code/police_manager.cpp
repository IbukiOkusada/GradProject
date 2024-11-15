//==========================================================
//
// �x�@�}�l�[�W���[ [police_manager.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "police_manager.h"
#include "police.h"
#include "player_manager.h"
#include "road.h"
#include "inspection.h"

// ���O���
namespace
{
	
}

// �ÓI�����o�ϐ��錾
CPoliceManager* CPoliceManager::m_pInstance = nullptr;	// �C���X�^���X

//==========================================================
// �R���X�g���N�^
//==========================================================
CPoliceManager::CPoliceManager()
{
	// �l�̃N���A
	m_pList = nullptr;
	m_nNum = 0;
}

//==========================================================
// �f�X�g���N�^
//==========================================================
CPoliceManager::~CPoliceManager()
{

}

//==========================================================
// ����������
//==========================================================
HRESULT CPoliceManager::Init(void)
{
	return S_OK;
}

//==========================================================
// �I������
//==========================================================
void CPoliceManager::Uninit(void)
{
	// �e�N���X�̏I������
	CListManager::Uninit();

	// ���X�g�p��
	ListRelease();

	// �C���X�^���X�̔p��
	if (m_pInstance != nullptr) {	// �C���X�^���X���m�ۂ���Ă���
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

//==========================================================
// �X�V����
//==========================================================
void CPoliceManager::Update(void)
{

}

//==========================================================
// �C���X�^���X���m��
//==========================================================
CPoliceManager* CPoliceManager::GetInstance(void)
{
	if (m_pInstance == nullptr) {	// �g���Ă��Ȃ�
		m_pInstance = DEBUG_NEW CPoliceManager;
	}

	return m_pInstance;
}

//==========================================================
// �C���X�^���X�������[�X
//==========================================================
void CPoliceManager::Release(void)
{
	if (m_pInstance != nullptr) {	// �C���X�^���X���m�ۂ���Ă���
		m_pInstance->Uninit();
	}
}

//==========================================================
// ���X�g�ɑ}��
//==========================================================
void CPoliceManager::ListIn(CPolice* pPolice)
{
	GetList()->Regist(pPolice);
}

//==========================================================
// ���X�g����O��
//==========================================================
void CPoliceManager::ListOut(CPolice* pPolice)
{
	// ���X�g���玩�����g���폜����
	GetList()->Delete(pPolice);
}

//==========================================================
// �U���q�b�g�m�F
//==========================================================
bool CPoliceManager::Hit(D3DXVECTOR3& pos, const float fRange, const float fHeight, const int nDamage)
{
	bool bUse = false;

	//�ʔ���
	for (int i = 0; i < GetList()->GetNum(); i++)
	{
		CPolice* pPolice = GetList()->Get(i);
		//if (pPolice->HitCheck(pos, fRange, fHeight, nDamage)) {
			//bUse = true;
		//}
	}

	return bUse;
}

//==========================================================
// �S�����x����Ԃɂ���
//==========================================================
void CPoliceManager::Warning(CPolice* pPolice)
{
	for (int i = 0; i < GetList()->GetNum(); i++)
	{
		// �x�@�Ǝ�������v���Ă���ʖ�
		CPolice* pP = GetList()->Get(i);
		if (pP == pPolice) { continue; }

		// ���ݒǐՒ����ʖ�
		CPolice::STATE outstate = CPolice::STATE::STATE_CHASE;
		if (pP->GetState() == outstate) { continue; }

		// �x����Ԃɂ���
		pP->SetState(CPolice::STATE::STATE_SEARCH);
	}

	// �����z�u����
	SetInspection();
}

//==========================================================
// �����z�u����
//==========================================================
void CPoliceManager::SetInspection()
{
	CPlayer* pPlayer = CPlayerManager::GetInstance()->GetTop();
	if (pPlayer == nullptr) { return; }

	// �v���C���[�̂��铹���擾����
	CRoad* pRoad = pPlayer->GetRoad();
	if (pRoad == nullptr) { return; }

	// �v���C���[�̌������擾���Ď��̓��H�̕����𓱂��o��
	float rotY = pPlayer->GetObj()->GetRotation().y;
	Adjust(rotY);

	float range = D3DX_PI * 0.25f;	// �΂ߗp�͈�
	CRoad* pPredRoad = nullptr;	// ���̓�

	// �����ɂ���Ď��̓���������
	if (rotY >= -range && rotY <= range)
	{// ���ړ�
		pPredRoad = pRoad->GetSearchRoad(CRoad::DIRECTION::DIC_DOWN)->pRoad;
	}
	else if (rotY >= range && rotY <= range * 3)
	{// ���ړ�
		pPredRoad = pRoad->GetSearchRoad(CRoad::DIRECTION::DIC_LEFT)->pRoad;
	}
	else if (rotY >= -range * 3 && rotY <= -range)
	{// �E�ړ�
		pPredRoad = pRoad->GetSearchRoad(CRoad::DIRECTION::DIC_RIGHT)->pRoad;
	}
	else
	{// ��ړ�
		pPredRoad = pRoad->GetSearchRoad(CRoad::DIRECTION::DIC_UP)->pRoad;
	}

	// ����������Ȃ�����
	if (pPredRoad == nullptr) { return; }

	// �������擾����
	D3DXVECTOR3 rot = VECTOR3_ZERO;
	rot.y = rotY;
	float targetrot = atan2f(pRoad->GetPosition().x - pPredRoad->GetPosition().x, 
		pRoad->GetPosition().z - pPredRoad->GetPosition().z);

	// �ݒu
	D3DXVECTOR3 pos = pPredRoad->GetPosition();
	pos.x += sinf(targetrot) * pPredRoad->GetInfo()->size.x;
	pos.z += cosf(targetrot) * pPredRoad->GetInfo()->size.y;

	CInstpection::Create(pos, rot);
}