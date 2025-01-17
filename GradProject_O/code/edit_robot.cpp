//==========================================================
//
// ���{�b�g�z�u�G�f�B�^ [edit_robot.cpp]
// Author : Ryosuke Oohara
//
//==========================================================
#include "edit_robot.h"
#include "input_mouse.h"
#include "input_keyboard.h"
#include "debugproc.h"
#include "goal.h"
#include "goal_manager.h"
#include "edit_handle.h"
#include "robot.h"
#include "robot_manager.h"
#include "effect3D.h"
#include "convenience.h"

namespace
{
	const float MIN_LENGTH = 50.0f;	// �ŏ��ړ���
	const D3DXVECTOR2 SET_SIZE = D3DXVECTOR2(500.0f, 500.0f);
	const float CHANGESIZE = 50.0f;
	const float MAX_SIZE = 1500.0f;
	const float MIN_SIZE = 250.0f;
	const float DEFAULT_DISTANCE = 1000;  // �f�t�H���g�̒���
}

//==========================================================
// �R���X�g���N�^
//==========================================================
CEdit_Robot::CEdit_Robot()
{
	// �l�̃N���A
	m_pSelect = nullptr;
	m_pHandle = nullptr;
	m_pStartPosEff = nullptr;
	m_pEndPosEff = nullptr;
	m_fMouseWheel = 0.0f;
	m_startRotate = VECTOR3_ZERO;
	m_fStartDistance = 0.0f;
}

//==========================================================
// �f�X�g���N�^
//==========================================================
CEdit_Robot::~CEdit_Robot()
{

}

//==========================================================
// ����������
//==========================================================
HRESULT CEdit_Robot::Init(void)
{
	m_fMoveLength = MIN_LENGTH;
	m_fStartDistance = DEFAULT_DISTANCE;

	CRobot* pTop = CRobotManager::GetInstance()->GetTop();

	while (pTop != nullptr)
	{
		CRobot* pNext = pTop->GetNext();

		pTop->SetState(CRobot::STATE_NONE);

		pTop = pNext;
	}

	return S_OK;
}

//==========================================================
// �I������
//==========================================================
void CEdit_Robot::Uninit(void)
{
	// ���I��
	if (m_pHandle != nullptr)
	{
		m_pHandle->Uninit();
		m_pHandle = nullptr;
	}

	m_pSelect = nullptr;

	CEdit::Uninit();
}

//==========================================================
// �X�V����
//==========================================================
void CEdit_Robot::Update(void)
{
	CDebugProc::GetInstance()->Print(" [ ���{�b�g�z�u���[�h ]\n");
	CInputKeyboard* pKey = CInputKeyboard::GetInstance();
	CInputMouse* pMouse = CInputMouse::GetInstance();
	auto* pOld = m_pSelect;

	// �I��
	ClickCheck();

	CDebugProc::GetInstance()->Print("[");

	// �ۑ�
	Save();

	if (pKey->GetPress(DIK_LALT) || pKey->GetPress(DIK_RALT)) { CDebugProc::GetInstance()->Print("]\n"); return; }

	// �I������Ă��Ȃ��A�������͑I����������
	if (m_pSelect == nullptr || pOld == nullptr) {

		// ����
		Create();

		CDebugProc::GetInstance()->Print("]\n");
		return;
	}

	// �I������
	if (pMouse->GetTrigger(CInputMouse::BUTTON_RBUTTON))
	{
		m_pSelect = nullptr;

		// ���I��
		if (m_pHandle != nullptr)
		{
			m_pHandle->Uninit();
			m_pHandle = nullptr;
		}
	}

	 // �I�����ꂽ���̂�F�ς���

	// ���̍X�V
	if (m_pHandle != nullptr && m_pSelect != nullptr)
	{
		m_pHandle->Update();
		m_startRotate = m_pSelect->GetRotation();
	}

	// �n���h���̏�ԕω�
	ModeChange();

	// �ړ�
	Move();

	// ����
	Rotate();

	// ����
	ChangeDistance();

	// �폜
	Delete();

	CRobot* pTop = CRobotManager::GetInstance()->GetTop();

	while (pTop != nullptr)
	{
		CRobot* pNext = pTop->GetNext();

		pTop->Update();
	
		pTop = pNext;
	}

	CDebugProc::GetInstance()->Print("]\n\n");

	// �S�[�����
	if (m_pSelect == nullptr) { return; }
	CDebugProc::GetInstance()->Print("[ ���{�b�g��� : ");
	D3DXVECTOR3 pos = m_pSelect->GetPosition(), rot = m_pSelect->GetRotation();
	CDebugProc::GetInstance()->Print("���W : [ %f, %f, %f ] : ", pos.x, pos.y, pos.z);
	CDebugProc::GetInstance()->Print("���� : [ %f, %f, %f ] : ", rot.x, rot.y, rot.z);
	CDebugProc::GetInstance()->Print("]\n");
}

//==========================================================
// �I��
//==========================================================
void CEdit_Robot::ClickCheck()
{
	CInputMouse* pMouse = CInputMouse::GetInstance();
	auto list = CGoalManager::GetInstance()->GetList();

	// ���͂�����Ίm�F����
	if (!pMouse->GetTrigger(CInputMouse::BUTTON_LBUTTON))
	{
		return;
	}

	// ���I��
	if (m_pHandle != nullptr)
	{
		if (m_pHandle->GetHold() != nullptr)
		{
			return;
		}
	}

	//m_pSelect = nullptr;
	auto mgr = CGoalManager::GetInstance()->GetList();

	CRobot* pTop = CRobotManager::GetInstance()->GetTop();

	while (pTop != nullptr)
	{
		CRobot* pNext = pTop->GetNext();

		// �Փ˔���
		if (CursorCollision(pTop))
		{
			m_pSelect = pTop;

			if (m_pHandle == nullptr)
			{
				m_pHandle = CEdit_Handle::Create(m_pSelect->GetPosition(), CEdit_Handle::TYPE_MOVE);
			}

			// �ʒu�ݒ�
			m_pHandle->SetPosition(m_pSelect->GetPosition());

			return;
		}

		pTop = pNext;
	}

	// �S�[�����I�΂�Ă��Ȃ�
	if (m_pSelect != nullptr) { return; }

	// ���g�p��
	if (m_pHandle == nullptr) { return; }

	m_pHandle->Uninit();
	m_pHandle = nullptr;
}

//==========================================================
// �I���������̂Ƃ̏Փ˔���
//==========================================================
bool CEdit_Robot::CursorCollision(CRobot* pRobot)
{
	// �}�E�X���
	CInputMouse* pMouse = CInputMouse::GetInstance();
	CInputMouse::SRayInfo info = pMouse->GetRayInfo();

	// ���{�b�g�̏��
	D3DXVECTOR3 pos = pRobot->GetPosition();
	float range = 50.0f;

	D3DXVECTOR3 pos0 = pos + D3DXVECTOR3(-range, 0.0f, range), pos1 = pos + D3DXVECTOR3(range, 0.0f, range),
		pos2 = pos + D3DXVECTOR3(-range, 0.0f, -range);

	// �O�p�`�Ɛ����̏Փˊm�F
	if (TriangleCollision(info.origin, info.vec, pos0, pos1, pos2))
	{
		return true;
	}

	// ���΂̎O�p��
	pos0 = pos + D3DXVECTOR3(range, 0.0f, -range);

	// �������Փˊm�F
	if (TriangleCollision(info.origin, info.vec, pos0, pos1, pos2))
	{
		return true;
	}

	return false;
}

//==========================================================
// �I���������̂Ƃ̏Փ˔���
//==========================================================
bool CEdit_Robot::TriangleCollision(const D3DXVECTOR3& rayOrigin, const D3DXVECTOR3& rayDir,
	const D3DXVECTOR3& pos0, const D3DXVECTOR3& pos1, const D3DXVECTOR3& pos2)
{
	// �O�p�`�̃G�b�W�x�N�g�����v�Z
	D3DXVECTOR3 edge1 = pos1 - pos0;
	D3DXVECTOR3 edge2 = pos2 - pos0;

	// ���C������ edge2 �̊O�ς��v�Z
	D3DXVECTOR3 pvec;
	D3DXVec3Cross(&pvec, &rayDir, &edge2);

	float det = D3DXVec3Dot(&edge1, &pvec);  // �s��

	// ���C���O�p�`�ɕ��s�ȏꍇ�͌������Ȃ�
	if (fabs(det) < 1e-8f) return false;

	float invDet = 1.0f / det;

	// ���C�̎n�_����O�p�`��1�ڂ̒��_�ւ̃x�N�g��
	D3DXVECTOR3 tvec = rayOrigin - pos0;

	// u�p�����[�^�̌v�Z�i�O�p�`�̓�������j
	float u = D3DXVec3Dot(&tvec, &pvec) * invDet;
	if (u < 0.0f || u > 1.0f) return false;

	// qvec���v�Z����v�p�����[�^�����߂�
	D3DXVECTOR3 qvec;
	D3DXVec3Cross(&qvec, &tvec, &edge1);
	float v = D3DXVec3Dot(&rayDir, &qvec) * invDet;
	if (v < 0.0f || u + v > 1.0f) return false;

	return true;  // �Փ˂����������ꍇ
}

//==========================================================
// �폜
//==========================================================
void CEdit_Robot::Delete()
{
	CDebugProc::GetInstance()->Print(" ���{�b�g�폜 : Delete or BackSpace, ");
	CInputKeyboard* pKey = CInputKeyboard::GetInstance();

	// ���͊m�F
	if (!pKey->GetTrigger(DIK_DELETE) && !pKey->GetTrigger(DIK_BACKSPACE)) { return; }

	// �Đڑ�
	m_pSelect->Uninit();
	m_pSelect = nullptr;

	// ���g�p��
	if (m_pHandle == nullptr) { return; }

	m_pHandle->Uninit();
	m_pHandle = nullptr;
}

//==========================================================
// �ړ�
//==========================================================
void CEdit_Robot::Move()
{
	if (m_pSelect == nullptr) { return; }
	if (m_pHandle->GetType() != CEdit_Handle::TYPE_MOVE) { return; }
	if (m_pHandle == nullptr) { return; }

	D3DXVECTOR3 pos = m_pSelect->GetPosition();	// ���W
	D3DXVECTOR3 Handlepos = m_pHandle->GetPosition();	// �����W

	// X���W
	float length = fabsf(Handlepos.x - pos.x);
	int setpos = static_cast<int>(Handlepos.x);
	int movelength = static_cast<int>(m_fMoveLength);

	// �傫��
	if (length >= m_fMoveLength)
	{
		pos.x = static_cast<float>(setpos - (setpos % movelength));
	}

	// Y���W
	length = fabsf(Handlepos.y - pos.y);
	setpos = static_cast<int>(Handlepos.y);
	movelength = static_cast<int>(m_fMoveLength);

	// �傫��
	if (length >= m_fMoveLength)
	{
		pos.y = static_cast<float>(setpos - (setpos % movelength));
	}

	// Z���W
	length = fabsf(Handlepos.z - pos.z);
	setpos = static_cast<int>(Handlepos.z);
	movelength = static_cast<int>(m_fMoveLength);

	// �傫��
	if (length >= m_fMoveLength)
	{
		pos.z = static_cast<float>(setpos - (setpos % movelength));
	}

	// �I�������S�[���̍��W�ݒ�
	m_pSelect->SetPosition(pos);
}

//==========================================================
// ��]
//==========================================================
void CEdit_Robot::Rotate()
{
	if (m_pSelect == nullptr) { return; }
	if (m_pHandle == nullptr) { return; }
	if (m_pHandle->GetType() != CEdit_Handle::TYPE_ROT) { return; }
	if (m_pHandle->GetHold() == nullptr) { return; }

	// �n���h���̈ړ�����ύX�X�P�[���擾
	D3DXVECTOR3 handlerotate = m_pHandle->GetDiffRotation();	// ���W

	// ����
	D3DXVECTOR3 rotate = m_startRotate;
	rotate.x += (handlerotate.x * 0.1f);
	rotate.y += (handlerotate.y * 0.1f);
	rotate.z += (handlerotate.z * 0.1f);

	// �l�␳
	Adjust(rotate);

	// �I��������Q���̌����ݒ�
	m_pSelect->SetRotation(rotate);
}

//==========================================================
// ��]���Z�b�g
//==========================================================
void CEdit_Robot::RotateReset()
{
	if (m_pSelect == nullptr) { return; }
	CDebugProc::GetInstance()->Print(" ��]���Z�b�g : Enter, ");

	CInputKeyboard* pKey = CInputKeyboard::GetInstance();

	// ���͊m�F
	if (!pKey->GetTrigger(DIK_RETURN)) { return; }

	m_pSelect->SetRotation(VECTOR3_ZERO);
}

//==========================================================
// �����̕ύX
//==========================================================
void CEdit_Robot::ChangeDistance()
{
	if (m_pSelect == nullptr) { return; }
	if (m_pHandle == nullptr) { return; }
	if (m_pHandle->GetType() != CEdit_Handle::TYPE_SCALE) { return; }
	//if (m_pHandle->GetHold() == nullptr) { return; }

	// �n���h���̈ړ�����ύX�X�P�[���擾
	D3DXVECTOR3 handlescale = m_pHandle->GetDiffPosition();	// ���W
	handlescale *= 0.001f;
	//handlescale += D3DXVECTOR3(1.0f, 0.0f, 1.0f);

	float distance = m_pSelect->GetDistace();
	distance += handlescale.x;
	distance += handlescale.z;

	m_pSelect->SetDistance(distance);

	m_pSelect->SetPosTerget(distance);

	D3DXVECTOR3 RobotPos = m_pSelect->GetPosition();
	D3DXVECTOR3 RobotRot = m_pSelect->GetRotation();
	D3DXVECTOR3 StartPosEff = D3DXVECTOR3(RobotPos.x + (sinf(RobotRot.y) * distance), 0.0f, RobotPos.z + (cosf(RobotRot.y) * distance));
	D3DXVECTOR3 EndPosEff = D3DXVECTOR3(RobotPos.x + (sinf(RobotRot.y) * -distance), 0.0f, RobotPos.z + (cosf(RobotRot.y) * -distance));

	// �܂�Ԃ��n�_�ɃG�t�F�N�g
	if (m_pStartPosEff == nullptr)
	{
		m_pStartPosEff = CEffect3D::Create(StartPosEff, VECTOR3_ZERO, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), 40.0f, 5.0f, CEffect3D::TYPE::TYPE_NONE);
	}

	if (m_pEndPosEff == nullptr)
	{
		m_pEndPosEff = CEffect3D::Create(EndPosEff, VECTOR3_ZERO, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), 40.0f, 5.0f, CEffect3D::TYPE::TYPE_NONE);
	}

	if (m_pStartPosEff != nullptr)
	{
		m_pStartPosEff->Update();

		if (m_pStartPosEff->GetLife() < 0.0f)
		{
			m_pStartPosEff = nullptr;
		}
	}

	if (m_pEndPosEff != nullptr)
	{
		m_pEndPosEff->Update();

		if (m_pEndPosEff->GetLife() < 0.0f)
		{
			m_pEndPosEff = nullptr;
		}
	}

	CDebugProc::GetInstance()->Print("�n���h���̈ړ��ʁF%f, %f, %f", handlescale.x, handlescale.y, handlescale.z);
	CDebugProc::GetInstance()->Print("�����F%f", distance);
}

//==========================================================
// �ۑ�
//==========================================================
void CEdit_Robot::Save()
{
	CDebugProc::GetInstance()->Print(" �ۑ� : F7, ");
	CInputKeyboard* pKey = CInputKeyboard::GetInstance();

	// ���͊m�F
	if (!pKey->GetTrigger(DIK_F7)) { return; }

	// �t�@�C�����J��
	std::ofstream File(EDITFILENAME::ROBOT, std::ios::binary);
	if (!File.is_open()) {
		return;
	}

	std::vector<CRobot::SInfo> savedata;

	CRobot* pTop = CRobotManager::GetInstance()->GetTop();

	while (pTop != nullptr)
	{
		CRobot* pNext = pTop->GetNext();

		savedata.push_back(pTop->GetInfo());

		pTop = pNext;
	}

	int size = savedata.size();

	// �x�N�g���̃T�C�Y���Z�[�u
	File.write(reinterpret_cast<const char*>(&size), sizeof(size));

	// �f�[�^���o�C�i���t�@�C���ɏ����o��
	File.write(reinterpret_cast<char*>(savedata.data()), size * sizeof(CRobot::SInfo));

	// �t�@�C�������
	File.close();
}

//==========================================================
// �ۑ�
//==========================================================
void CEdit_Robot::Create()
{
	CDebugProc::GetInstance()->Print(" ���� : �}�E�X�z�C�[���N���b�N, ");
	CInputMouse* pMouse = CInputMouse::GetInstance();

	// ���͊m�F
	if (!pMouse->GetTrigger(CInputMouse::BUTTON_WHEEL)) { return; }

	D3DXVECTOR3 rayDir = pMouse->GetRayInfo().vec;
	D3DXVECTOR3 rayOrigin = pMouse->GetRayInfo().origin;

	// ���C�� Y = 0 �̕��ʂƕ��s�ȏꍇ�i�����x�N�g���� Y ������ 0�j
	if (fabs(rayDir.y) < 1e-6f) {
		return;  // �������Ȃ�
	}

	// �����܂ł̋��� t ���v�Z
	float t = -rayOrigin.y / rayDir.y;

	// t �����̏ꍇ�A���C�̋t�����ɂ���̂Ō������Ȃ�
	if (t < 0.0f) {
		return;
	}

	D3DXVECTOR3 pos, rot;

	// ��_�̍��W���v�Z�iY = 0 ���ʏ�j
	pos.x = rayOrigin.x + t * rayDir.x;
	pos.y = 0.0f;  // Y = 0 �Ȃ̂ŌŒ�
	pos.z = rayOrigin.z + t * rayDir.z;

	// �[���؎̂�
	int setpos = static_cast<int>(pos.x);
	int movelength = static_cast<int>(MIN_LENGTH);
	pos.x -= setpos % movelength;
	setpos = static_cast<int>(pos.z);
	pos.z -= setpos % movelength;
	rot = VECTOR3_ZERO;

	CRobot* pRobot = CRobot::Create(pos, rot, DEFAULT_DISTANCE);
	pRobot->SetState(CRobot::STATE::STATE_NONE);
	pRobot->Update();
}

//==========================================================
// ��ԕύX
//==========================================================
void CEdit_Robot::ModeChange()
{
	if (m_pHandle == nullptr) { return; }

	CInputMouse* pMouse = CInputMouse::GetInstance();
	CDebugProc::GetInstance()->Print("��ԕύX : [ �}�E�X�z�C�[���N���b�N ] : ");

	// ���͊m�F
	if (!pMouse->GetTrigger(CInputMouse::BUTTON_WHEEL)) { return; }

	// ��ޕύX
	int type = m_pHandle->GetType();
	type = (type + 1) % CEdit_Handle::TYPE_MAX;

	// �p��
	m_pHandle->Uninit();
	m_pHandle = nullptr;
	m_pHandle = CEdit_Handle::Create(m_pSelect->GetPosition(), static_cast<CEdit_Handle::TYPE>(type));
}