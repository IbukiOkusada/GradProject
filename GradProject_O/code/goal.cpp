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
#include "particle2D.h"
#include "camera_manager.h"
#include "camera.h"
#include "renderer.h"
#include "manager.h"
#include "debugproc.h"
#include "camera_action.h"

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
	// �G�t�F�N�g�\��
	ScreenEffect();

	if (CheckRange() && CheckSpeed())
	{
		// �J�����A�N�V���������
		CPlayer* pPlayer = CPlayerManager::GetInstance()->GetTop();
		CCamera* pCamera = CCameraManager::GetInstance()->GetTop();
		pCamera->GetAction()->Set(pCamera, D3DXVECTOR3(0.0f, pPlayer->GetRotation().y + D3DX_PI, D3DX_PI* 0.4f), 500.0f, 2.0f, 2.0f, CCameraAction::MOVE_POSV);
		pPlayer->AddDeliveryCount();
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

//==========================================================
// ��ʏ�ɃG�t�F�N�g�̕\��
//==========================================================
void CGole::ScreenEffect()
{
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();
	CCamera* pCamera = CCameraManager::GetInstance()->GetTop();
	D3DXMATRIX mtxProjection, mtxView, mtxWorld;
	D3DVIEWPORT9 Viewport;
	D3DXVECTOR3 pos = VECTOR3_ZERO;

	// �K�v�ȏ��擾
	pDevice->GetTransform(D3DTS_PROJECTION, &mtxProjection);
	pDevice->GetTransform(D3DTS_VIEW, &mtxView);
	pDevice->GetViewport(&Viewport);

	//���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&mtxWorld);

	// �X�N���[�����W�擾
	D3DXVec3Project(&pos, &m_pos, &Viewport, &mtxProjection, &mtxView, &mtxWorld);

	// ��ʓ��Ȃ�o���Ȃ�
	if(pos.x > 0.0f && pos.x < SCREEN_WIDTH &&
		pos.y > 0.0f && pos.y < SCREEN_HEIGHT) {
		return;
	}

	if (pos.z > 1.0f) { pos *= -1.0f; }

	// �␳
	if (pos.x > SCREEN_WIDTH) { pos.x = SCREEN_WIDTH; }
	else if (pos.x < 0.0f) { pos.x = 0.0f; }
	if (pos.y > SCREEN_HEIGHT) { pos.y = SCREEN_HEIGHT; }
	else if (pos.y < 0.0f) { pos.y = 0.0f; }

	CDebugProc::GetInstance()->Print("�S�[���̉�ʍ��W : [ %f, %f, %f ] \n", pos.x, pos.y, pos.z);

	CParticle2D::Create(pos, CEffect2D::TYPE_TARGET);
}
