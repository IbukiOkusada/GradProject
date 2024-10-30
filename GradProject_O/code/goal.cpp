//==========================================================
//
// �S�[�� [goal.cpp]
// Author : �O�엳�V��
//
//==========================================================
#include "goal.h"
#include "player_manager.h"
#include "player.h"
#include "convenience.h"
#include "meshcylinder.h"
// �}�N����`
Clist<CGole*> * CGole::pList = nullptr;
//==========================================================
// �R���X�g���N�^
//==========================================================
CGole::CGole()
{
	pEffect = nullptr;
	m_fRange = 0.0f;
	m_fLimit = 0.0f;
	//���g�����X�g�ɓo�^
	GetInstance()->Regist(this);
}

//==========================================================
// �f�X�g���N�^
//==========================================================
CGole::~CGole()
{
	//���g�����X�g����폜
	GetInstance()->Delete(this);
	if (GetInstance()->Empty())
	{//���X�g�̒��g����Ȃ烊�X�g��j��
		ListRelease();
	}
}

//==========================================================
// ����������
//==========================================================
HRESULT CGole::Init(void)
{
	pEffect = CEffekseer::GetInstance()->Create("data\\EFFEKSEER\\goal_radius.efkefc", m_pos, VECTOR3_ZERO, VECTOR3_ZERO, m_fRange, false, false);
	
	return S_OK;
}

//==========================================================
// �I������
//==========================================================
void CGole::Uninit(void)
{
	SAFE_DELETE(pEffect);
	Release();
}

//==========================================================
// �X�V����
//==========================================================
void CGole::Update(void)
{
	if (CheckRange() && CheckSpeed())
	{
		Uninit();
	}

}
//==========================================================
// �����̃`�F�b�N
//==========================================================
bool CGole::CheckRange()
{
	CPlayer* pPlayer = CPlayerManager::GetInstance()->GetTop();
	if (pPlayer != NULL)
	{
		float fDis = GetDistance(m_pos, pPlayer->GetPosition());
		return (m_fRange >= fDis);
	}
	return false;
}
//==========================================================
// ���x�̃`�F�b�N
//==========================================================
bool CGole::CheckSpeed()
{
	CPlayer* pPlayer = CPlayerManager::GetInstance()->GetTop();
	if (pPlayer != NULL)
	{
		float fDis = GetDistance(VECTOR3_ZERO, pPlayer->GetMove());
		return (m_fLimit >= fDis);
	}
	return false;
}
//==========================================================
// ����
//==========================================================
CGole* CGole::Create(D3DXVECTOR3 pos, float fRange, float fLimit)
{
	CGole* pGoal = nullptr;

	pGoal = DEBUG_NEW CGole;

	if (pGoal != nullptr)
	{
		pGoal->m_pos = pos;
		pGoal->m_fRange = fRange;
		pGoal->m_fLimit = fLimit;
		// ����������
		pGoal->Init();
	}

	return pGoal;
}
