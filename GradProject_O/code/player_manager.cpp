//==========================================================
//
// �v���C���[�}�l�[�W���[ [Player_manager.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "player_manager.h"
#include "player.h"

// �ÓI�����o�ϐ��錾
CPlayerManager* CPlayerManager::m_pInstance = nullptr;	// �C���X�^���X

//==========================================================
// �R���X�g���N�^
//==========================================================
CPlayerManager::CPlayerManager()
{
	// �l�̃N���A
	m_List.Clear();
}

//==========================================================
// �f�X�g���N�^
//==========================================================
CPlayerManager::~CPlayerManager()
{

}

//==========================================================
// ����������
//==========================================================
HRESULT CPlayerManager::Init(void)
{
	return S_OK;
}

//==========================================================
// �I������
//==========================================================
void CPlayerManager::Uninit(void)
{
	// ���X�g�}�l�[�W���[�p��
	CListManager::Uninit();

	// �C���X�^���X�p��
	if (m_pInstance != nullptr) {
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

//==========================================================
// �X�V����
//==========================================================
void CPlayerManager::Update(void)
{
	
}

//==========================================================
// �C���X�^���X���m��
//==========================================================
CPlayerManager* CPlayerManager::GetInstance(void)
{
	if (m_pInstance == nullptr) {	// �g���Ă��Ȃ�
		m_pInstance = DEBUG_NEW CPlayerManager;
	}

	return m_pInstance;
}

//==========================================================
// �C���X�^���X�������[�X
//==========================================================
void CPlayerManager::Release(void)
{
	if (m_pInstance != nullptr) {	// �C���X�^���X���m�ۂ���Ă���
		m_pInstance->Uninit();
	}
}

//==========================================================
// ���X�g�ɑ}��
//==========================================================
bool CPlayerManager::ListIn(CPlayer* pPlayer)
{
	if (pPlayer == nullptr) { return false; }

	return m_List.Regist(pPlayer->GetId(), pPlayer);
}

//==========================================================
// ���X�g����O��
//==========================================================
bool CPlayerManager::ListOut(CPlayer* pPlayer)
{
	if (pPlayer == nullptr) { return false; }

	return m_List.Delete(pPlayer->GetId(), pPlayer);
}

//==========================================================
// �w�肳�ꂽID�̃v���C���[�擾
//==========================================================
CPlayer* CPlayerManager::GetPlayer(int nIdx)
{
	return m_List.Get(nIdx);
}

//==========================================================
// �U���q�b�g�m�F
//==========================================================
bool CPlayerManager::Hit(D3DXVECTOR3& pos, const float fRange, const float fHeight, const int nDamage)
{
	bool bUse = false;

	return bUse;
}