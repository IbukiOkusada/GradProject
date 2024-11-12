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
#include "deltatime.h"
#include "baggage.h"
#include "road_manager.h"
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
	m_bEnd = false;
	m_pPeople = nullptr;
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
	float range = static_cast<float>(rand() % 629 - 314) * 0.01f;
	D3DXVECTOR3 pos = m_pos;
	pos.x += sinf(range) * 800.0f;
	pos.z += cosf(range) * 800.0f;
	m_pPeople = CObjectX::Create(pos, VECTOR3_ZERO, "data\\MODEL\\cube.x");
	m_pPeople->SetScale(D3DXVECTOR3(10.0f, 20.0f, 10.0f));
	m_pPeople->SetColAdd(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));


	Clist<CRoad*>* List = CRoadManager::GetInstance()->GetList();
	float fDis = FLT_MAX;
	m_pRoad = nullptr;
	for (int i = 0; i < List->GetNum(); i++)
	{
		float F = GetDistance(List->Get(i)->GetPosition(), m_pos);
		if (F < fDis && List->Get(i)->GetType() !=CRoad::TYPE_NONE)
		{
			fDis = F;
			m_pRoad = List->Get(i);
		}
	}



	return S_OK;
}

//==========================================================
// �I������
//==========================================================
void CGole::Uninit(void)
{
	SAFE_UNINIT(m_pPeople);
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

	if (CheckRange() && CheckSpeed() && !m_bEnd)
	{
		// �J�����A�N�V���������
		CPlayer* pPlayer = CPlayerManager::GetInstance()->GetTop();
		m_bEnd = true;
		pPlayer->ThrowBaggage(m_pPeople->GetPos());
		pPlayer->AddDeliveryCount();
		SAFE_DELETE(pEffect);
	}

	if (m_bEnd && CBaggage::GetList()->GetNum() == 0)
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

//==========================================================
// ��ʏ�ɃG�t�F�N�g�̕\��
//==========================================================
void CGole::ScreenEffect()
{
	if (m_bEnd) { return; }
	if (CDeltaTime::GetInstance()->GetSlow() < 1.0f) { return; }
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
