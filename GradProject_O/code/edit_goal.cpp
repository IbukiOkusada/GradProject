//==========================================================
//
// �G�f�B�^�[�S�[�� [edit_goal.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "edit_goal.h"
#include "input_mouse.h"
#include "input_keyboard.h"
#include "debugproc.h"
#include "goal.h"
#include "goal_manager.h"
#include "edit_handle.h"

namespace
{
	const float MIN_LENGTH = 50.0f;	// �ŏ��ړ���
	const D3DXVECTOR2 SET_SIZE = D3DXVECTOR2(500.0f, 500.0f);
	const float CHANGESIZE = 50.0f;
	const float MAX_SIZE = 1500.0f;
	const float MIN_SIZE = 250.0f;
}

//==========================================================
// �R���X�g���N�^
//==========================================================
CEdit_Goal::CEdit_Goal()
{
	// �l�̃N���A
	m_pSelect = nullptr;
	m_pHandle = nullptr;
	m_fMouseWheel = 0.0f;
}

//==========================================================
// �f�X�g���N�^
//==========================================================
CEdit_Goal::~CEdit_Goal()
{

}

//==========================================================
// ����������
//==========================================================
HRESULT CEdit_Goal::Init(void)
{
	m_fMoveLength = MIN_LENGTH;

	auto mgr = CGoalManager::GetInstance();
	auto list = mgr->GetList();
	auto infolist = mgr->GetInfoList();
	int num = list.GetNum();

	std::vector<int> id;
	id.clear();

	for (const auto& ite : *list.GetList())
	{
		id.push_back(ite.first);
	}

	for (int i = 0; i < num; i++)
	{
		CGoal* pGoal = list.Get(id[i]);
		if (pGoal == nullptr) { continue; }
		pGoal->Uninit();
	}

	// �S�[�������ׂč폜���ĉ��߂Đ�������
	for (int i = 0; i < infolist->size(); i++)
	{
		auto it = (*infolist)[i];
		CGoal::Create(it.pos, it.fRange, it.fLimit, i);
	}

	mgr->ResetGoal();

	return S_OK;
}

//==========================================================
// �I������
//==========================================================
void CEdit_Goal::Uninit(void)
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
void CEdit_Goal::Update(void)
{
	CDebugProc::GetInstance()->Print(" [ �S�[���z�u���[�h ]\n");
	CInputKeyboard* pKey = CInputKeyboard::GetInstance();
	CGoal* pOld = m_pSelect;

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

	// �I�����ꂽ���̂�F�ς���

	// ���̍X�V
	if (m_pHandle != nullptr)
	{
		m_pHandle->Update();
	}

	// �ړ�
	Move();

	// �폜
	Delete();

	CDebugProc::GetInstance()->Print("]\n\n");

	// �S�[�����
	if (m_pSelect == nullptr) { return; }
	CDebugProc::GetInstance()->Print("[ �S�[����� : ");
	D3DXVECTOR3 pos = m_pSelect->GetPos();
	CDebugProc::GetInstance()->Print("���W : [ %f, %f, %f ] : ", pos.x, pos.y, pos.z);
	CDebugProc::GetInstance()->Print("]\n");
}

//==========================================================
// �I��
//==========================================================
void CEdit_Goal::ClickCheck()
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

	m_pSelect = nullptr;
	auto mgr = CGoalManager::GetInstance()->GetList();

	// �S�[����S�Ċm�F
	for (auto ite = mgr.GetBegin(); ite != mgr.GetEnd(); ite++)
	{
		CGoal* pGoal = ite->second;

		// �Փ˂���
		if (CursorCollision(pGoal))
		{
			m_pSelect = pGoal;

			if (m_pHandle == nullptr)
			{
				m_pHandle = CEdit_Handle::Create(m_pSelect->GetPos(), CEdit_Handle::TYPE_MOVE);
			}

			m_pHandle->SetPosition(m_pSelect->GetPos());

			return;
		}
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
bool CEdit_Goal::CursorCollision(CGoal* pGoal)
{
	// �}�E�X���
	CInputMouse* pMouse = CInputMouse::GetInstance();
	CInputMouse::SRayInfo info = pMouse->GetRayInfo();

	// �����
	D3DXVECTOR3 pos = pGoal->GetPos();
	float range = pGoal->GetRange();

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
bool CEdit_Goal::TriangleCollision(const D3DXVECTOR3& rayOrigin, const D3DXVECTOR3& rayDir,
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
void CEdit_Goal::Delete()
{
	CDebugProc::GetInstance()->Print(" �S�[���폜 : Delete or BackSpace, ");
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
void CEdit_Goal::Move()
{
	if (m_pSelect == nullptr) { return; }
	if (m_pHandle == nullptr) { return; }

	D3DXVECTOR3 pos = m_pSelect->GetPos();	// ���W
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
	m_pSelect->SetPos(pos);
}

//==========================================================
// �ۑ�
//==========================================================
void CEdit_Goal::Save()
{
	CDebugProc::GetInstance()->Print(" �ۑ� : F7, ");
	CInputKeyboard* pKey = CInputKeyboard::GetInstance();

	// ���͊m�F
	if (!pKey->GetTrigger(DIK_F7)) { return; }

	// �t�@�C�����J��
	std::ofstream File(EDITFILENAME::GOAL, std::ios::binary);
	if (!File.is_open()) {
		return;
	}

	std::vector<CGoal::SInfo> savedata;

	auto mgr = CGoalManager::GetInstance()->GetList();

	// �S�[����S�Ċm�F
	for (auto ite = mgr.GetBegin(); ite != mgr.GetEnd(); ite++)
	{
		savedata.push_back(*ite->second->GetInfo());
	}

	int size = savedata.size();

	// �x�N�g���̃T�C�Y���Z�[�u
	File.write(reinterpret_cast<const char*>(&size), sizeof(size));

	// �f�[�^���o�C�i���t�@�C���ɏ����o��
	File.write(reinterpret_cast<char*>(savedata.data()), size * sizeof(CGoal::SInfo));

	// �t�@�C�������
	File.close();
}

//==========================================================
// �ۑ�
//==========================================================
void CEdit_Goal::Create()
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

	D3DXVECTOR3 pos;

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

	CGoal::Create(pos, 600.0f, 20.0f, CGoalManager::GetInstance()->GetList().GetInCnt());
}