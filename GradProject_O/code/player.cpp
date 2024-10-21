//===============================================
//
// �v���C���[�̏��� [player.cpp]
// Author : Ibuki Okusada
//
//===============================================
#include "player.h"
#include "manager.h"
#include "renderer.h"
#include "input.h"
#include "debugproc.h"
#include "camera.h"
#include "Xfile.h"
#include "slow.h"
#include "texture.h"
#include "meshfield.h"
#include "Xfile.h"
#include "meshorbit.h"
#include "game.h"
#include "filter.h"
#include "object2D.h"
#include "meshwall.h"
#include "objectX.h"
#include "sound.h"
#include <assert.h>
#include "billboard.h"
#include "character.h"
#include "motion.h"
#include "sound.h"
#include "model.h"
#include "particle.h"
#include "effect.h"
#include <math.h>
#include "meshwall.h"
#include "player_manager.h"
#include "fade.h"
#include "input_keyboard.h"
#include "input_gamepad.h"
#include "camera_manager.h"
#include "road.h"
#include "road_manager.h"
#include "collision.h"

//===============================================
// �}�N����`
//===============================================

namespace {
	const float DAMAGE_APPEAR = (110.0f);	// ���G���ԃC���^�[�o��
	const float DEATH_INTERVAL = (120.0f);	// ���S�C���^�[�o��
	const float SPAWN_INTERVAL = (60.0f);	// �����C���^�[�o��
	const float CAMROT_INER = (0.2f);			// �J��������
	const float KICK_LENGTH = (1000.0f);	// �U���͈�
	const int LIFE = (10);
	const float MOVE = (6.2f);		// �ړ���
	const float BRAKE = (0.5f);		// �u���[�L

	const float TURN = (0.006f);		// �����
	const float GRAVITY = (-0.6f);		//�v���C���[�d��
	const float ROT_MULTI = (1.0f);	// �����␳�{��
	const float WIDTH = (20.0f);	// ��
	const float HEIGHT = (80.0f);	// ����
	const float INER = (0.9f);		// ����
	const float ENGINE_INER = (0.01f);		// ����
	const float ENGINE_BRAKE = (0.006f);		// ����
	const float TUURN_INER = (0.9f);		// ����
	const float RES = (1.98f);		// ����
	const float JUMP = (16.0f);
}

// �O���錾

//===============================================
// �R���X�g���N�^
//===============================================
//CPlayer::CPlayer()
//{
//	// �l���N���A����
//	m_nCounterAnim = 0;
//	m_nPatternAnim = 0;
//}

//===============================================
// �R���X�g���N�^(�I�[�o�[���[�h)
//===============================================
CPlayer::CPlayer()
{
	// �l���N���A����
	m_Info.pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_Info.posOld = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_Info.rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_Info.move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_Info.pRoad = nullptr;
	m_fRotMove = 0.0f;
	m_fRotDiff = 0.0f;
	m_fRotDest = 0.0f;
	m_fBrake = 0.0f;
	m_fEngine = 0.0f;
	m_fTurnSpeed = 0.f;
	m_type = TYPE_NONE;
	m_nId = -1;
	m_Info.fSlideMove = 0.0f;
	m_pObj = nullptr;
	m_pPrev = nullptr;
	m_pNext = nullptr;
	m_pTailLamp = CEffekseer::GetInstance()->Create("data\\EFFEKSEER\\taillamp.efkefc", VECTOR3_ZERO, VECTOR3_ZERO, VECTOR3_ZERO, 45.0f, false, false);
	
	CPlayerManager::GetInstance()->ListIn(this);
}

//===============================================
// �f�X�g���N�^
//===============================================
CPlayer::~CPlayer()
{
	
}

//===============================================
// ����������
//===============================================
HRESULT CPlayer::Init(void)
{
	// ���̐���
	m_Info.state = STATE_APPEAR;
	m_type = TYPE_NONE;

	return S_OK;
}

//===============================================
// ����������(�I�[�o�[���[�h)
//===============================================
HRESULT CPlayer::Init(const char *pBodyName, const char *pLegName)
{
	m_pObj = CObjectX::Create(D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), "data\\MODEL\\car000.x");
	SetMatrix();

	return S_OK;
}

//===============================================
// �I������
//===============================================
void CPlayer::Uninit(void)
{	
	SAFE_UNINIT(m_pObj)
	SAFE_DELETE(m_pTailLamp);
	CPlayerManager::GetInstance()->ListOut(this);

	// �p��
	Release();
}

//===============================================
// �X�V����
//===============================================
void CPlayer::Update(void)
{	
	// �O��̍��W���擾
	m_Info.posOld = GetPosition();

	StateSet();	

	if (m_type == TYPE_ACTIVE)
	{
		
		// �v���C���[����
		Controller();

		// �Ŋ��̓�����
		SearchRoad();

		// �����蔻��
		Collision();

		// �I�����C�����M
		CManager::GetInstance()->GetScene()->SendPosition(m_Info.pos);
		CManager::GetInstance()->GetScene()->SendRotation(m_Info.rot);
	}
	else {

	}
	
	SetMatrix();

	if (m_pObj != nullptr)
	{
		D3DXVECTOR3 rot = GetRotation();
		rot.y += D3DX_PI * 0.5f;
		m_pObj->SetPosition(GetPosition());
		m_pObj->SetRotation(rot);
	}

	{
		D3DXVECTOR3 rot = GetRotation();
		rot.y -= D3DX_PI * 0.5f;
		CCamera* pCamera = CCameraManager::GetInstance()->GetTop();
		pCamera->Pursue(GetPosition(), rot, 3000.0f);
	}

	// �G�t�F�N�g
	{
		m_pTailLamp->m_pos = GetPosition();
		m_pTailLamp->m_rot = m_pObj->GetRotation();
	
	}
}

//===============================================
// ����
//===============================================
CPlayer *CPlayer::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move, const char *pBodyName, const char *pLegName)
{
	CPlayer *pPlayer = nullptr;

	// �I�u�W�F�N�g2D�̐���
	pPlayer = DEBUG_NEW CPlayer;

	if (nullptr != pPlayer)
	{// �����ł����ꍇ
		// ����������
		pPlayer->Init(pBodyName, pLegName);

		// ���W�ݒ�
		pPlayer->SetPosition(pos);

		// �����ݒ�
		pPlayer->SetRotation(rot);

		pPlayer->m_fRotDest = rot.y;

		// �ړ��ʐݒ�
		pPlayer->SetMove(move);
	}
	else
	{// �����Ɏ��s�����ꍇ
		return nullptr;
	}

	return pPlayer;
}

//===============================================
// ���쏈��
//===============================================
void CPlayer::Controller(void)
{
	D3DXVECTOR3 rot = GetRotation();	// �������擾
	m_fRotMove = rot.y;	//���݂̌������擾

	// �ړ�
	Move();

	// ��]
	Rotate();

	// �����␳
	Adjust();
}

//===============================================
// �ړ�
//===============================================
void CPlayer::Move(void)
{
	CInputKeyboard *pInputKey = CInputKeyboard::GetInstance();	// �L�[�{�[�h�̃|�C���^
	CInputPad *pInputPad = CInputPad::GetInstance();
	float fThrottle = 0.0f;
	m_fBrake = 0.0f;
	if (pInputKey->GetPress(DIK_W))
	{
		fThrottle = 0.8f;
	}
	else
	{
		fThrottle = (float)pInputPad->GetRightTriggerPress(0) / 255;
	}
	Engine(fThrottle);
	if (pInputKey->GetPress(DIK_S))
	{
		
		m_Info.move*=BRAKE;
	
		m_fBrake = 1.0f;
	}
	else
	{
		float fSpeed = (float)pInputPad->GetLeftTriggerPress(0) / 255;
		m_fBrake = fSpeed;
		float b = 1.0f - (1.0f - BRAKE) * fSpeed;
	}
	// ���͑��u�m�F
	if (nullptr == pInputKey){
		return;
	}


	m_Info.pos += m_Info.move;

	m_Info.move *= INER;//�ړ��ʂ̌���
}
//===============================================
//�ȈՃG���W���V�~��
//===============================================
void  CPlayer::Engine(float fThrottle)
{
	m_fEngine +=  (1.0f - m_fEngine) * fThrottle * ENGINE_INER;
	if (fThrottle <= 0)
	{
		m_fEngine -= ENGINE_BRAKE;
		if (m_fEngine <= 0)
		{
			m_fEngine = 0;
		}
	}
	m_Info.move.z += MOVE * sinf(-m_Info.rot.y) * m_fEngine;
	m_Info.move.x += MOVE * cosf(-m_Info.rot.y) * m_fEngine;
	CManager::GetInstance()->GetDebugProc()->Print("fThrottle:%f\nm_fEngine%f\n", fThrottle, m_fEngine);
}
//===============================================
// ����
//===============================================
void CPlayer::Rotate(void)
{
	CInputKeyboard* pInputKey = CInputKeyboard::GetInstance();	// �L�[�{�[�h�̃|�C���^
	CInputPad* pInputPad = CInputPad::GetInstance();
	if (pInputKey->GetPress(DIK_D))
	{
		m_fTurnSpeed += TURN * (1.0f);
	}
	else if (pInputKey->GetPress(DIK_A))
	{
		m_fTurnSpeed -= TURN * (1.0f );
	}
	else
	{
		m_fTurnSpeed += TURN *pInputPad->GetLStick(0, 0.1f).x;
	}
	m_fTurnSpeed*= TUURN_INER;
	// ���͑��u�m�F
	if (nullptr == pInputKey) {
		return;
	}

	// m_fRotDiff�̒l�ς��Ă����Ε␳������
}

//===============================================
// ����
//===============================================
void CPlayer::Adjust(void)
{

	m_Info.rot.y += m_fTurnSpeed;

	while (1)
	{
		if (m_Info.rot.y > D3DX_PI || m_Info.rot.y < -D3DX_PI)
		{//-3.14�`3.14�͈̔͊O�̏ꍇ
			if (m_Info.rot.y > D3DX_PI)
			{
				m_Info.rot.y += (-D3DX_PI * 2);
			}
			else if (m_Info.rot.y < -D3DX_PI)
			{
				m_Info.rot.y += (D3DX_PI * 2);
			}
		}
		else
		{
			break;
		}
	}
}

//===============================================
// �Ŋ��̓�����
//===============================================
void CPlayer::SearchRoad()
{
	CRoadManager* pRoadManager = CRoadManager::GetInstance();

	CRoad* pRoad = pRoadManager->GetTop();
	CRoad* pRoadClose = pRoadManager->GetTop();
	float length = D3DXVec3Length(&(pRoadClose->GetPosition() - m_Info.pos));
	float lengthClose = 0.0f;

	while (pRoad != nullptr)
	{// �g�p����Ă��Ȃ���Ԃ܂�

		CRoad* pRoadNext = pRoad->GetNext();	// ���̃I�u�W�F�N�g�ւ̃|�C���^���擾

		// �������菈��
		lengthClose = D3DXVec3Length(&(pRoad->GetPosition() - m_Info.pos));

		if (length > lengthClose)
		{
			length = lengthClose;
			pRoadClose = pRoad;
		}

		pRoad = pRoadNext;	// ���̃I�u�W�F�N�g�Ɉړ�
	}

	m_Info.pRoad = pRoadClose;
}

//===============================================
// �����蔻�菈��
//===============================================
bool CPlayer::Collision(void)
{
	CObjectX* pObjectX = CObjectX::GetTop();	// �擪���擾

	while (pObjectX != nullptr)
	{// �g�p����Ă��Ȃ���Ԃ܂�

		CObjectX* pObjectXNext = pObjectX->GetNext();	// ���̃I�u�W�F�N�g�ւ̃|�C���^���擾

		D3DXVECTOR3 posObjectX = pObjectX->GetPosition();
		D3DXVECTOR3 rotObjectX = pObjectX->GetRotation();
		D3DXVECTOR3 sizeMax = pObjectX->GetVtxMax();
		D3DXVECTOR3 sizeMin = pObjectX->GetVtxMin();

		bool bCollision = collision::CollidePointToOBB(&m_Info.pos, m_Info.posOld, posObjectX, rotObjectX, (sizeMax - sizeMin) * 0.5f);

		if (bCollision)
		{
			return true;
		}

		pObjectX = pObjectXNext;	// ���̃I�u�W�F�N�g�Ɉړ�
	}

	return false;
}

//===============================================
// ��ԊǗ�
//===============================================
void CPlayer::StateSet(void)
{
	float fSlawMul = CManager::GetInstance()->GetSlow()->Get();
	switch (m_Info.state)
	{
	case STATE_APPEAR:
	{
		m_Info.fStateCounter -= fSlawMul;

		if (m_Info.fStateCounter <= 0.0f)
		{
			m_Info.fStateCounter = 0.0f;
			m_Info.state = STATE_NORMAL;
		}
	}
		break;

	case STATE_NORMAL:
	{

	}
		break;

	case STATE_DAMAGE:
	{
		m_Info.fStateCounter -= fSlawMul;

		if (m_Info.fStateCounter <= 0.0f)
		{
			m_Info.fStateCounter = DAMAGE_APPEAR;
			m_Info.state = STATE_APPEAR;
		}
	}
		break;

	case STATE_DEATH:
	{
		m_Info.fStateCounter -= fSlawMul;

		if (m_Info.fStateCounter <= 0.0f)
		{
			m_Info.fStateCounter = DAMAGE_APPEAR;
			m_Info.state = STATE_SPAWN;
		}
	}
		break;

	case STATE_SPAWN:
	{
		m_Info.fStateCounter -= fSlawMul;

		if (m_Info.fStateCounter <= 0.0f)
		{
			m_Info.fStateCounter = SPAWN_INTERVAL;
			m_Info.state = STATE_APPEAR;

			// ���̃p�[�e�B�N������
			CParticle::Create(m_Info.pos, CEffect::TYPE_SMAKE);
		}
	}
		break;

	}
}

//===============================================
// �̗͐ݒ�
//===============================================
void CPlayer::SetType(TYPE type)
{
	m_type = type;
}

//===============================================
// �}�g���b�N�X�ݒ�
//===============================================
void CPlayer::SetMatrix(void)
{
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();	//�f�o�C�X�ւ̃|�C���^���擾
	D3DXMATRIX mtxRot, mtxTrans;	//�v�Z�p�}�g���b�N�X

	//���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&m_Info.mtxWorld);

	//�����𔽉f
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_Info.rot.y, m_Info.rot.x, m_Info.rot.z);
	D3DXMatrixMultiply(&m_Info.mtxWorld, &m_Info.mtxWorld, &mtxRot);

	//�ʒu�𔽉f
	D3DXMatrixTranslation(&mtxTrans, m_Info.pos.x, m_Info.pos.y, m_Info.pos.z);
	D3DXMatrixMultiply(&m_Info.mtxWorld, &m_Info.mtxWorld, &mtxTrans);

	//���[���h�}�g���b�N�X�̐ݒ�
	pDevice->SetTransform(D3DTS_WORLD, &m_Info.mtxWorld);
}

//===============================================
// �w�胂�[�V�����ɐݒ�
//===============================================
void CPlayer::SetMotion(int nMotion)
{
	
	
}

//===============================================
// �v���C���[�̕`��ݒ�
//===============================================
void CPlayer::SetDraw(bool bDraw)
{
	
}