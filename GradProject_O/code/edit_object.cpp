//==========================================================
//
// �G�f�B�^�[�I�u�W�F�N�g [edit_object.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "edit_object.h"
#include "manager.h"
#include "debugproc.h"
#include "edit_arrow.h"
#include "map_obstacle.h"
#include "objectX.h"
#include "input_mouse.h"
#include "input_keyboard.h"
#include "map_manager.h"

namespace
{
	const std::string FILENAME = "data\\FILE\\map\\obstacle.bin";
}

//==========================================================
// �R���X�g���N�^
//==========================================================
CEdit_Obj::CEdit_Obj()
{
	// �l�̃N���A
	m_pSelect = nullptr;
	m_pArrow = nullptr;
	m_fMouseWheel = 0.0f;
}

//==========================================================
// �f�X�g���N�^
//==========================================================
CEdit_Obj::~CEdit_Obj()
{

}

//==========================================================
// ����������
//==========================================================
HRESULT CEdit_Obj::Init(void)
{
	return S_OK;
}

//==========================================================
// �I������
//==========================================================
void CEdit_Obj::Uninit(void)
{
	if (m_pArrow != nullptr)
	{
		m_pArrow->Uninit();
		m_pArrow = nullptr;
	}

	CEdit::Uninit();
}

//==========================================================
// �X�V����
//==========================================================
void CEdit_Obj::Update(void)
{
	CDebugProc::GetInstance()->Print(" [ ��Q���z�u���[�h ]\n");
	CInputKeyboard* pKey = CInputKeyboard::GetInstance();
	CMapObstacle* pOld = m_pSelect;

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
	m_pSelect->GetObj()->SetColMulti(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f));

	// ���̍X�V
	if (m_pArrow != nullptr)
	{
		m_pArrow->Update();
	}

	// ���f���ύX
	ModelChange();

	// �ړ�
	Move();

	// �폜
	Delete();

	CDebugProc::GetInstance()->Print("]\n\n");

	// ��Q�����
	if (m_pSelect == nullptr) { return; }
	CDebugProc::GetInstance()->Print("[ ����� : ");
	D3DXVECTOR3 pos = m_pSelect->GetPosition();
	CDebugProc::GetInstance()->Print("���W : [ %f, %f, %f] : ", pos.x, pos.y, pos.z);
	D3DXVECTOR3 rot = m_pSelect->GetRotation();
	CDebugProc::GetInstance()->Print("���� : [ %f, %f, %f] : ", rot.x, rot.y, rot.z);
	CDebugProc::GetInstance()->Print("]\n");
}

//==========================================================
// �I��
//==========================================================
void CEdit_Obj::ClickCheck()
{
	CInputMouse* pMouse = CInputMouse::GetInstance();
	Clist<CMapObstacle*>* pList = CMapObstacle::GetList();
	float length = 10000000.0f;

	if (CMapObstacle::GetList() == nullptr) { return; }

	// ���͂�����Ίm�F����
	if (!pMouse->GetTrigger(CInputMouse::BUTTON_LBUTTON))
	{
		return;
	}

	// ���I��
	if (m_pArrow != nullptr)
	{
		if (m_pArrow->GetHold() != nullptr)
		{
			return;
		}
	}

	if (m_pSelect != nullptr)
	{
		m_pSelect->GetObj()->SetColMulti(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	}

	m_pSelect = nullptr;

	for (int i = 0; i < pList->GetNum(); i++)
	{
		CMapObstacle* pObj = pList->Get(i);

		// �Փ˂���
		if (CursorCollision(pObj))
		{
			m_pSelect = pObj;

			if (m_pArrow == nullptr)
			{
				m_pArrow = CEdit_Arrow::Create(m_pSelect->GetPosition());
			}

			m_pArrow->SetPosition(m_pSelect->GetPosition());
		}
	}

	// �����I�΂�Ă��Ȃ�
	if (m_pSelect != nullptr) { return; }

	// ���g�p��
	if (m_pArrow == nullptr) { return; }

	m_pArrow->Uninit();
	m_pArrow = nullptr;
}

//==========================================================
// �I���������̂Ƃ̏Փ˔���
//==========================================================
bool CEdit_Obj::CursorCollision(CMapObstacle* pObj)
{
	// �}�E�X���
	CInputMouse* pMouse = CInputMouse::GetInstance();
	CInputMouse::SRayInfo info = pMouse->GetRayInfo();

	// �����
	D3DXVECTOR3 pos = pObj->GetPosition();
	D3DXVECTOR3 rot = pObj->GetRotation();
	D3DXVECTOR3 vtxmax = pObj->GetObj()->GetVtxMax();
	D3DXVECTOR3 vtxmin = pObj->GetObj()->GetVtxMin();
	D3DXVECTOR3 touchpos = VECTOR3_ZERO;
	D3DXVECTOR3 origin = pMouse->GetRayInfo().origin;
	D3DXVECTOR3 vec = pMouse->GetRayInfo().vec;

	// �g�p����Ă��Ȃ�������Ԃ�
	if (collision::CollideRayToOBB(&touchpos, origin, vec, pos, rot, vtxmax, vtxmin))
	{
		if (m_pSelect != nullptr)
		{
			D3DXVECTOR3 vec = touchpos - origin;
			float nowlength = D3DXVec3Length(&vec);

			vec = m_pSelect->GetPosition() - origin;
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
void CEdit_Obj::Delete()
{
	CDebugProc::GetInstance()->Print(" ���폜 : Delete or BackSpace, ");
	CInputKeyboard* pKey = CInputKeyboard::GetInstance();

	// ���͊m�F
	if (!pKey->GetTrigger(DIK_DELETE) && !pKey->GetTrigger(DIK_BACKSPACE)) { return; }

	// �Đڑ�
	m_pSelect->Uninit();
	m_pSelect = nullptr;

	// ���g�p��
	if (m_pArrow == nullptr) { return; }

	m_pArrow->Uninit();
	m_pArrow = nullptr;
}

//==========================================================
// �ړ�
//==========================================================
void CEdit_Obj::Move()
{
	if (m_pSelect == nullptr) { return; }
	if (m_pArrow == nullptr) { return; }

	D3DXVECTOR3 pos = m_pArrow->GetPosition();	// ���W

	// �I���������̍��W�ݒ�
	m_pSelect->SetPosition(pos);
}

//==========================================================
// ���f���ύX
//==========================================================
void CEdit_Obj::ModelChange()
{
	if (m_pSelect == nullptr) { return; }
	CInputMouse* pMouse = CInputMouse::GetInstance();

	float old = m_fMouseWheel;
	m_fMouseWheel += pMouse->GetCousorMove().z;

	// �}�E�X�z�C�[��
	if (m_fMouseWheel == old) { return; }

	if (static_cast<int>(m_fMouseWheel) % 20 != 0) { return; }

	int idx = m_pSelect->GetInfo().fileidx;

	if (m_fMouseWheel >= old)
	{
		idx = (idx + 1) % CMapManager::GetInstance()->GetFileNameList().size();
	}
	else
	{
		idx = (idx + CMapManager::GetInstance()->GetFileNameList().size() - 1) % CMapManager::GetInstance()->GetFileNameList().size();
	}

	m_pSelect->BindModel(idx);
}

//==========================================================
// �ۑ�
//==========================================================
void CEdit_Obj::Save()
{
	CDebugProc::GetInstance()->Print(" �ۑ� : F7, ");
	CInputKeyboard* pKey = CInputKeyboard::GetInstance();

	Clist<CMapObstacle*>* pList = CMapObstacle::GetList();
	if (pList == nullptr) { return; }

	// ���͊m�F
	if (!pKey->GetTrigger(DIK_F7)) { return; }

	// �t�@�C�����J��
	std::ofstream File(FILENAME, std::ios::binary);
	if (!File.is_open()) {
		return;
	}

	std::vector<CMapObstacle::SInfo> savedata;

	for (int i = 0; i < pList->GetNum(); i++)
	{
		savedata.push_back(pList->Get(i)->GetInfo());
	}

	int size = savedata.size();

	// �x�N�g���̃T�C�Y���Z�[�u
	File.write(reinterpret_cast<const char*>(&size), sizeof(size));

	// �f�[�^���o�C�i���t�@�C���ɏ����o��
	File.write(reinterpret_cast<char*>(savedata.data()), size * sizeof(CMapObstacle::SInfo));

	// �t�@�C�������
	File.close();
}

//==========================================================
// �ۑ�
//==========================================================
void CEdit_Obj::Create()
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

	CMapObstacle::SInfo info = CMapObstacle::SInfo();

	// ��_�̍��W���v�Z�iY = 0 ���ʏ�j
	info.pos.x = rayOrigin.x + t * rayDir.x;
	info.pos.y = 0.0f;  // Y = 0 �Ȃ̂ŌŒ�
	info.pos.z = rayOrigin.z + t * rayDir.z;
	info.fileidx = 0;
	info.rot = VECTOR3_ZERO;

	CMapObstacle::Create(info);
}
