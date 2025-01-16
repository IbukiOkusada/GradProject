//==========================================================
//
// �G�f�B�^�[�I�u�W�F�N�g [edit_object.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "edit_gimmick.h"
#include "manager.h"
#include "debugproc.h"
#include "edit_handle.h"
#include "map_obstacle.h"
#include "objectX.h"
#include "input_mouse.h"
#include "input_keyboard.h"
#include "map_manager.h"
#include "gimmick.h"

namespace
{
	
}

//==========================================================
// �R���X�g���N�^
//==========================================================
CEdit_Gimmick::CEdit_Gimmick()
{
	// �l�̃N���A
	m_pSelect = nullptr;
	m_pHandle = nullptr;
	m_fMouseWheel = 0.0f;
	m_startScale = VECTOR3_ZERO;
	m_startRotate = VECTOR3_ZERO;
}

//==========================================================
// �f�X�g���N�^
//==========================================================
CEdit_Gimmick::~CEdit_Gimmick()
{

}

//==========================================================
// ����������
//==========================================================
HRESULT CEdit_Gimmick::Init(void)
{
	return S_OK;
}

//==========================================================
// �I������
//==========================================================
void CEdit_Gimmick::Uninit(void)
{
	if (m_pHandle != nullptr)
	{
		m_pHandle->Uninit();
		m_pHandle = nullptr;
	}

	CEdit::Uninit();
}

//==========================================================
// �X�V����
//==========================================================
void CEdit_Gimmick::Update(void)
{
	CDebugProc::GetInstance()->Print(" [ �M�~�b�N�z�u���[�h ]\n");
	CInputKeyboard* pKey = CInputKeyboard::GetInstance();
	CGimmick* pOld = m_pSelect;

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
	m_pSelect->SetColMulti(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f));

	// ���̍X�V
	if (m_pHandle != nullptr)
	{
		m_pHandle->Update();

		if (m_pHandle->GetOldHold() == nullptr && m_pHandle->GetHold() != nullptr)
		{
			m_startScale = m_pSelect->GetScale();
			m_startRotate = m_pSelect->GetRot();
		}
	}

	// ���ύX
	ModeChange();

	// ���f���ύX
	ModelChange();

	// �ړ�
	Move();

	// �X�P�[��
	Scale();

	// ��]
	Rotate();

	// ��]���Z�b�g
	RotateReset();

	// �폜
	Delete();

	CDebugProc::GetInstance()->Print("]\n\n");

	// ��Q�����
	if (m_pSelect == nullptr) { return; }
	CDebugProc::GetInstance()->Print("[ ��� : ");
	D3DXVECTOR3 pos = m_pSelect->GetPos();
	CDebugProc::GetInstance()->Print("���W : [ %f, %f, %f] : ", pos.x, pos.y, pos.z);
	D3DXVECTOR3 rot = m_pSelect->GetRot();
	CDebugProc::GetInstance()->Print("���� : [ %f, %f, %f] : ", rot.x, rot.y, rot.z);
	CDebugProc::GetInstance()->Print("]\n");
}

//==========================================================
// �I��
//==========================================================
void CEdit_Gimmick::ClickCheck()
{
	CInputMouse* pMouse = CInputMouse::GetInstance();
	Clist<CGimmick*>* pList = CGimmick::GetList();

	if (CGimmick::GetList() == nullptr) { return; }

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

	if (m_pSelect != nullptr)
	{
		m_pSelect->SetColMulti(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	}

	m_pSelect = nullptr;

	for (int i = 0; i < pList->GetNum(); i++)
	{
		CGimmick* pGimmick = pList->Get(i);

		// �Փ˂���
		if (CursorCollision(pGimmick))
		{
			m_pSelect = pGimmick;

			if (m_pHandle == nullptr)
			{
				m_pHandle = CEdit_Handle::Create(m_pSelect->GetPos(), CEdit_Handle::TYPE_MOVE);
			}

			m_pHandle->SetPosition(m_pSelect->GetPos());
		}
	}

	// �����I�΂�Ă��Ȃ�
	if (m_pSelect != nullptr) { return; }

	// ���g�p��
	if (m_pHandle == nullptr) { return; }

	m_pHandle->Uninit();
	m_pHandle = nullptr;
}

//==========================================================
// �I���������̂Ƃ̏Փ˔���
//==========================================================
bool CEdit_Gimmick::CursorCollision(CGimmick* pGimmick)
{
	// �}�E�X���
	CInputMouse* pMouse = CInputMouse::GetInstance();
	CInputMouse::SRayInfo info = pMouse->GetRayInfo();

	// �����
	D3DXVECTOR3 pos = pGimmick->GetPos();
	D3DXVECTOR3 rot = pGimmick->GetRot();
	D3DXVECTOR3 vtxmax = pGimmick->GetVtxMax();
	D3DXVECTOR3 vtxmin = pGimmick->GetVtxMin();
	D3DXVECTOR3 touchpos = VECTOR3_ZERO;
	D3DXVECTOR3 origin = pMouse->GetRayInfo().origin;
	D3DXVECTOR3 vec = pMouse->GetRayInfo().vec;

	// �g�p����Ă��Ȃ�������Ԃ�
	if (collision::CollideRayToOBB(&touchpos, origin, vec, pos, rot, vtxmax, vtxmin))
	{
		if (m_pSelect != nullptr)
		{
			vec = touchpos - origin;
			float nowlength = D3DXVec3Length(&vec);

			vec = m_pSelect->GetPos() - origin;
			float length = D3DXVec3Length(&vec);

			if (nowlength > length)
			{
				return false;
			}
		}

		return true;
	}

	return false;
}

//==========================================================
// �폜
//==========================================================
void CEdit_Gimmick::Delete()
{
	CDebugProc::GetInstance()->Print(" �폜 : Delete or BackSpace, ");
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
void CEdit_Gimmick::Move()
{
	if (m_pSelect == nullptr) { return; }
	if (m_pHandle == nullptr) { return; }
	if (m_pHandle->GetType() != CEdit_Handle::TYPE_MOVE) { return; }

	D3DXVECTOR3 pos = m_pHandle->GetPosition();	// ���W

	// �I���������̍��W�ݒ�
	m_pSelect->SetPos(pos);
}

//==========================================================
// �X�P�[��
//==========================================================
void CEdit_Gimmick::Scale()
{
	if (m_pSelect == nullptr) { return; }
	if (m_pHandle == nullptr) { return; }
	if (m_pHandle->GetType() != CEdit_Handle::TYPE_SCALE) { return; }
	if (m_pHandle->GetHold() == nullptr) { return; }

	// �n���h���̈ړ�����ύX�X�P�[���擾
	D3DXVECTOR3 handlescale = m_pHandle->GetDiffPosition();	// ���W
	handlescale *= 0.001f;
	handlescale += D3DXVECTOR3(1.0f, 1.0f, 1.0f);

	// ����
	D3DXVECTOR3 scale = m_startScale;
	scale.x *= handlescale.x;
	scale.y *= handlescale.y;
	scale.z *= handlescale.z;

	// �I���������̍��W�ݒ�
	m_pSelect->SetScale(scale);
}

//==========================================================
// ��]
//==========================================================
void CEdit_Gimmick::Rotate()
{
	if (m_pSelect == nullptr) { return; }
	if (m_pHandle == nullptr) { return; }
	if (m_pHandle->GetType() != CEdit_Handle::TYPE_ROT) { return; }
	if (m_pHandle->GetHold() == nullptr) { return; }

	// �n���h���̈ړ�����ύX�X�P�[���擾
	D3DXVECTOR3 handlerotate = m_pHandle->GetDiffRotation();	// ���W

	// ����
	D3DXVECTOR3 rotate = m_startRotate;
	rotate.x += handlerotate.x;
	rotate.y += handlerotate.y;
	rotate.z += handlerotate.z;

	// �I��������Q���̌����ݒ�
	m_pSelect->SetRot(rotate);
}

//==========================================================
// ��]���Z�b�g
//==========================================================
void CEdit_Gimmick::RotateReset()
{
	if (m_pSelect == nullptr) { return; }
	CDebugProc::GetInstance()->Print(" ��]���Z�b�g : Enter, ");

	CInputKeyboard* pKey = CInputKeyboard::GetInstance();

	// ���͊m�F
	if (!pKey->GetTrigger(DIK_RETURN)) { return; }

	m_pSelect->SetRot(VECTOR3_ZERO);
}

//==========================================================
// ���f���ύX
//==========================================================
void CEdit_Gimmick::ModelChange()
{
	if (m_pSelect == nullptr) { return; }
	CInputMouse* pMouse = CInputMouse::GetInstance();

	float old = m_fMouseWheel;
	m_fMouseWheel += pMouse->GetCousorMove().z;

	// �}�E�X�z�C�[��
	if (m_fMouseWheel == old) { return; }

	if (static_cast<int>(m_fMouseWheel) % 20 != 0) { return; }

	int idx = m_pSelect->GetInfo().type;

	if (m_fMouseWheel >= old)
	{
		idx = (idx + 1) % CGimmick::TYPE::TYPE_MAX;
	}
	else
	{
		idx = (idx + CGimmick::TYPE::TYPE_MAX - 1) % CGimmick::TYPE::TYPE_MAX;
	}

	CGimmick::SInfo info = m_pSelect->GetInfo();
	m_pSelect->Uninit();
	m_pSelect = CGimmick::Create(info.pos, info.rot, info.scale, static_cast<CGimmick::TYPE>(idx));
}

//==========================================================
// �ۑ�
//==========================================================
void CEdit_Gimmick::Save()
{
	CDebugProc::GetInstance()->Print(" �ۑ� : F7, ");
	CInputKeyboard* pKey = CInputKeyboard::GetInstance();

	Clist<CGimmick*>* pList = CGimmick::GetList();
	if (pList == nullptr) { return; }

	// ���͊m�F
	if (!pKey->GetTrigger(DIK_F7)) { return; }

	// �t�@�C�����J��
	std::ofstream File(EDITFILENAME::EFFECT, std::ios::binary);
	if (!File.is_open()) {
		return;
	}

	std::vector<CGimmick::SInfo> savedata;

	for (int i = 0; i < pList->GetNum(); i++)
	{
		savedata.push_back(pList->Get(i)->GetInfo());
	}

	int size = savedata.size();

	// �x�N�g���̃T�C�Y���Z�[�u
	File.write(reinterpret_cast<const char*>(&size), sizeof(size));

	// �f�[�^���o�C�i���t�@�C���ɏ����o��
	File.write(reinterpret_cast<char*>(savedata.data()), size * sizeof(CGimmick::SInfo));

	// �t�@�C�������
	File.close();
}

//==========================================================
// ����
//==========================================================
void CEdit_Gimmick::Create()
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

	CGimmick::SInfo info = CGimmick::SInfo();

	// ��_�̍��W���v�Z�iY = 0 ���ʏ�j
	info.pos.x = rayOrigin.x + t * rayDir.x;
	info.pos.y = 0.0f;  // Y = 0 �Ȃ̂ŌŒ�
	info.pos.z = rayOrigin.z + t * rayDir.z;
	info.rot = VECTOR3_ZERO;
	info.scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

	CGimmick::Create(info.pos, info.rot, info.scale, CGimmick::TYPE::TYPE_FIREHYDRANT);
}

//==========================================================
// ��ԕύX
//==========================================================
void CEdit_Gimmick::ModeChange()
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
	m_pHandle = CEdit_Handle::Create(m_pSelect->GetPos(), static_cast<CEdit_Handle::TYPE>(type));
}