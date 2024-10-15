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
#include "convenience.h"
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
	const float MOVE = (1.2f);		// �ړ���
	const float BRAKE = (0.98f);		// �u���[�L

	const float TURN = (0.025f);		// �����
	const float GRAVITY = (-0.6f);		//�v���C���[�d��
	const float ROT_MULTI = (1.0f);	// �����␳�{��
	const float WIDTH = (20.0f);	// ��
	const float HEIGHT = (80.0f);	// ����
	const float INER = (0.98f);		// ����
	const float RES = (1.98f);		// ����
	const float JUMP = (16.0f);

	const float STIFFNESS = (11.0f); //�����W�� ��ʓI�Ȑݒ�l�͈̔�: 10 �` 15�i�o�C�N�̃^�C���j�A15 �` 30�i���[�X�p�^�C���j
	const float SHAPE = (1.2f); //�`��W�� ��ʓI�ȃo�C�N�̃^�C���ł́A1.2 �` 1.5���炢�ɐݒ肷��
	const float PEAK_FORCE = (1000.0f);  //�ő�� �d�ʂ̌y���o�C�N�̃^�C���́A��ʓI�� 1000 �` 1500 N �͈̔͂ɐݒ�
	const float CURVATURE = (0.9f); //�ȗ������W�� 0�`1�B�ʏ�� 0.9�`1.0

	const float MAX_RPM = (6750.0f); //�ő��]��
	const float IDOL_RPM = (1.0f); //�Œ��]��
	const float ENGINE_INR = (0.1f); //�G���W���̊���
	const float TIRERADIUS = (0.35f); // �^�C���̔��a�im�j
	const float WHEEL_CIRCUMFERENCE = (2.0f * D3DX_PI * TIRERADIUS); // �^�C���̉~���im�j
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
	m_Info.pos = VECTOR3_ZERO;
	m_Info.posOld = VECTOR3_ZERO;
	m_Info.rot = VECTOR3_ZERO;
	m_Info.move = VECTOR3_ZERO;
	Tire_rot = VECTOR3_ZERO;
	RotSpeed = VECTOR3_ZERO;
	m_fRotMove = 0.0f;
	m_fRotDiff = 0.0f;
	m_fRotDest = 0.0f;
	m_fBrake = 0.0f;
	m_fAccel = 0.0f;
	m_fRPM = 0.0f;
	m_type = TYPE_NONE;
	m_nId = -1;
	m_Info.fSlideMove = 0.0f;
	m_pObj = nullptr;
	m_pPrev = nullptr;
	m_pNext = nullptr;

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
	m_pObj = CObjectX::Create(VECTOR3_ZERO, VECTOR3_ZERO, "data\\MODEL\\car000.x");
	SetMatrix();

	return S_OK;
}

//===============================================
// �I������
//===============================================
void CPlayer::Uninit(void)
{
	if (m_pObj != nullptr)
	{
		m_pObj->Uninit();
		m_pObj = nullptr;
	}

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
		D3DXVECTOR3 rot = GetRotation();
		rot.y -= D3DX_PI * 0.5f;
		D3DXVECTOR3 pos = GetPosition();
		pos.x += sinf(rot.y) * 100.0f;
		pos.z += cosf(rot.y) * 100.0f;
		CParticle::Create(pos, CEffect::TYPE_SMAKE);
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

	//// �����␳
	//Adjust();
}

//===============================================
// �ړ�
//===============================================
void CPlayer::Move(void)
{
	CInputKeyboard *pInputKey = CInputKeyboard::GetInstance();	// �L�[�{�[�h�̃|�C���^
	CInputPad *pInputPad = CInputPad::GetInstance();
	m_fBrake = 0.0f;
	if (pInputKey->GetPress(DIK_W))
	{
		m_fAccel = 1.0f;
	/*	m_Info.move.z += MOVE * sinf(-m_Info.rot.y);
		m_Info.move.x += MOVE * cosf(-m_Info.rot.y);*/
	}
	else
	{
		m_fAccel = (float)pInputPad->GetRightTriggerPress(0) / 255;
	}
	Engine(m_fAccel);
	LateralMove();
	LongitudinalMove();

	// ���͑��u�m�F
	if (nullptr == pInputKey){
		return;
	}


	m_Info.pos += m_Info.move;
	m_Info.move *= INER;//�ړ��ʂ̌���
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
		Tire_rot.y += TURN * (1.0f+m_fBrake);
	}
	else if (pInputKey->GetPress(DIK_A))
	{
		Tire_rot.y -= TURN * (1.0f + m_fBrake);
	}
	else
	{
		Tire_rot.y += TURN *pInputPad->GetLStick(0, 0.1f).x * (1.0f + m_fBrake);
	}
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
	while (1)
	{
		if (m_fRotDiff > D3DX_PI || m_fRotDiff < -D3DX_PI)
		{//-3.14�`3.14�͈̔͊O�̏ꍇ
			if (m_fRotDiff > D3DX_PI)
			{
				m_fRotDiff += (-D3DX_PI * 2);
			}
			else if (m_fRotDiff < -D3DX_PI)
			{
				m_fRotDiff += (D3DX_PI * 2);
			}
		}
		else
		{
			break;
		}
	}

	m_fRotDest = m_fRotDiff - m_fRotMove;	//�ڕW�܂ł̈ړ������̍���

	while (1)
	{
		if (m_fRotDest > D3DX_PI || m_fRotDest < -D3DX_PI)
		{//-3.14�`3.14�͈̔͊O�̏ꍇ
			if (m_fRotDest > D3DX_PI)
			{
				m_fRotDest += (-D3DX_PI * 2);
			}
			else if (m_fRotDest < -D3DX_PI)
			{
				m_fRotDest += (D3DX_PI * 2);
			}
		}
		else
		{
			break;
		}
	}

	m_Info.rot.y += m_fRotDest * ROT_MULTI;

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
// �G���W���̃V�~�����[�V����
//===============================================
float CPlayer::Engine(float fAccel)
{
	float targetRPM = fAccel * MAX_RPM;
	m_fRPM = m_fRPM + (targetRPM - m_fRPM) * ENGINE_INR / 60;
	if (fAccel == 0.0f) {
		m_fRPM -= 1000.0f / 60;
		if (m_fRPM < IDOL_RPM) {
			m_fRPM = IDOL_RPM;  // �A�C�h����]���ȉ��ɂ͉�����Ȃ�
		}
	}
	return m_fRPM;
}
//===============================================
// ���͌v�Z
//===============================================
float CPlayer::LateralForce(float fSlipAngle)
{
	float fY = PEAK_FORCE * sinf(SHAPE * atanf(STIFFNESS * fSlipAngle - CURVATURE * (STIFFNESS * fSlipAngle - atanf(STIFFNESS * fSlipAngle))));
	return fY;
}
//===============================================
// �c�͌v�Z
//===============================================
float CPlayer::LongitudinalForce(float fSlipRatio)
{
	float fX = PEAK_FORCE * sinf(SHAPE * atanf(STIFFNESS * fSlipRatio - CURVATURE * (STIFFNESS * fSlipRatio - atanf(STIFFNESS * fSlipRatio))));
	return fX;
}
//===============================================
// ���͈ړ�
//===============================================
void CPlayer::LateralMove()
{
	
	// Pacejka���f���ŉ��͂��v�Z
	float lateralForce = LateralForce(CalculateSlipAngle());

	// �ԗ��̃��[�����x���v�Z (�������[�����g�Ɖ��͂��g�p)
	float yawAcceleration = lateralForce * 3 / 12;

	// ���[�p���x���X�V
	RotSpeed.y += yawAcceleration / 60;

	// ���[�p���X�V�i�o�C�N�̌�����ς���j
	m_Info.rot.y += RotSpeed.y / 60;
}
//===============================================
// �c�͈ړ�
//===============================================
void CPlayer::LongitudinalMove()
{
	// Pacejka���f���ŏc�͂��v�Z
	float longitudinalForce = LongitudinalForce(CalculateSlipRatio());

	// �ԗ��̉����x���v�Z
	float acceleration = longitudinalForce / 140.0f;

	// ���x���X�V
	m_Info.move.z += acceleration /60;

	// �ʒu���X�V
	m_Info.pos += m_Info.move;
}
//===============================================
// �X���b�v���̌v�Z
//===============================================
float CPlayer::CalculateSlipRatio()
{
	// �^�C���̉�]���x���v�Z
	float wheelSpeed = (m_fRPM * WHEEL_CIRCUMFERENCE) / 60.0f; // m/s
	float vehicleSpeed = GetDistance(VECTOR3_ZERO, GetMove());
	// �X���b�v�����v�Z
	float slipRatio = (wheelSpeed - vehicleSpeed) / wheelSpeed;

	return slipRatio;
}
//===============================================
// �X���b�v�p�̌v�Z
//===============================================
float CPlayer::CalculateSlipAngle()
{

	// �^�C���̐i�s�������x
	D3DXVECTOR3 Move = GetMove();
	D3DXMATRIX RotMtx;
	D3DXMatrixIdentity((&RotMtx));
	D3DXMatrixRotationY(&RotMtx, Tire_rot.y);
	D3DXVec3TransformCoord(&Move, &Move, &RotMtx);
	
	// �X���b�v�p���v�Z
	float slipAngle = atan2(Move.x, fabs(Move.z));  // atan2���g�p���Đ��m�Ɋp�x���擾
	return slipAngle;
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
void CPlayer::SetMotion(int nMotion) {
	
	
}

//===============================================
// �v���C���[�̕`��ݒ�
//===============================================
void CPlayer::SetDraw(bool bDraw)
{
	
}

//===============================================
// 
//===============================================
void CPlayer::Collision(void)
{

}