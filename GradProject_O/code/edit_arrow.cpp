//==========================================================
//
// �G�f�B�^�[��� [edit_arrow.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "edit_arrow.h"
#include "objectX.h"
#include "input_mouse.h"
#include "debugproc.h"

// �������O���
namespace
{
	const char* FILENAME[CEdit_Arrow::TYPE_MAX] = {	// ���f���t�@�C����
		"data\\MODEL\\edit\\cube.x",
		"data\\MODEL\\edit\\x_arrow.x",
		"data\\MODEL\\edit\\y_arrow.x",
		"data\\MODEL\\edit\\z_arrow.x",
	};

	const D3DXVECTOR3 SETPOS[CEdit_Arrow::TYPE_MAX] = {	// �ݒu���W
		{0.0f, 0.0f, 0.0f},
		{180.0f, 0.0f, 0.0f},
		{0.0f, 180.0f, 0.0f},
		{0.0f, 0.0f, -180.0f},
	};
}

//==========================================================
// �R���X�g���N�^
//==========================================================
CEdit_Arrow::CEdit_Arrow()
{
	// �l�̃N���A
	for (int i = 0; i < TYPE_MAX; i++)
	{
		m_aObj[i].pObj = nullptr;
		m_aObj[i].type = static_cast<TYPE>(i);
	}

	m_pos = VECTOR3_ZERO;
	m_Info = SInfo();
	m_pHold = nullptr;
}

//==========================================================
// �f�X�g���N�^
//==========================================================
CEdit_Arrow::~CEdit_Arrow()
{

}

//==========================================================
// ����
//==========================================================
CEdit_Arrow* CEdit_Arrow::Create(const D3DXVECTOR3& pos)
{
	CEdit_Arrow* pArrow = DEBUG_NEW CEdit_Arrow;

	if (pArrow != nullptr)
	{
		pArrow->Init();
		pArrow->SetPosition(pos);
		pArrow->m_Info.startpos = pos;
		pArrow->m_Info.touchpos = pos;
	}

	return pArrow;
}

//==========================================================
// ����������
//==========================================================
HRESULT CEdit_Arrow::Init(void)
{
	m_pos = VECTOR3_ZERO;

	// ���f������
	for (int i = 0; i < TYPE_MAX; i++)
	{
		m_aObj[i].pObj = CObjectX::Create(m_pos + SETPOS[i], VECTOR3_ZERO, FILENAME[i], 7);
		m_aObj[i].pObj->SetType(CObject::TYPE_EDIT);
	}

	return S_OK;
}

//==========================================================
// �I������
//==========================================================
void CEdit_Arrow::Uninit(void)
{
	// �l�̃N���A
	for (int i = 0; i < TYPE_MAX; i++)
	{
		if (m_aObj[i].pObj == nullptr) { continue; }

		m_aObj[i].pObj->Uninit();
		m_aObj[i].pObj = nullptr;
	}

	delete this;
}

//==========================================================
// �X�V����
//==========================================================
void CEdit_Arrow::Update(void)
{
	// �I��
	Select();

	// �ړ�
	Move();

	// ����
	Release();

	// ���W�̍X�V
	for (int i = 0; i < TYPE_MAX; i++)
	{
		if (m_aObj[i].pObj == nullptr) { continue; }

		m_aObj[i].pObj->SetPosition(m_pos + SETPOS[i]);
	}
}

//==========================================================
// ���W�ݒ�
//==========================================================
void CEdit_Arrow::SetPosition(const D3DXVECTOR3& pos)
{
	m_pos = pos;

	// ���W�̍X�V
	for (int i = 0; i < TYPE_MAX; i++)
	{
		if (m_aObj[i].pObj == nullptr) { continue; }

		m_aObj[i].pObj->SetPosition(m_pos + SETPOS[i]);
	}
}

//==========================================================
// ���W�ݒ�
//==========================================================
void CEdit_Arrow::Move()
{
	// ����ł���Ƃ��̂�
	if (m_pHold == nullptr) { return; }

	// ���͊m�F
	CInputMouse* pMouse = CInputMouse::GetInstance();
	bool press = pMouse->GetPress(CInputMouse::BUTTON_LBUTTON);
	if (!press) { return; }

	D3DXVECTOR3 worldpos = pMouse->GetWorldInfo().pos;
	D3DXVECTOR3 move = worldpos - m_Info.touchworldpos;
	
	// 4. �V�����ʒu���v�Z
	D3DXVECTOR3 newpos = m_Info.touchpos;

	// ��ގw��
	switch (m_pHold->type)
	{
	case TYPE_ALL:
	{
		newpos = m_Info.touchpos + move;
		newpos.y = m_Info.touchpos.y;
		CDebugProc::GetInstance()->Print("�S���I\n");
	}
	break;

	case TYPE_X:
	{
		newpos.x = m_Info.touchpos.x + move.x;
		CDebugProc::GetInstance()->Print("X�I\n");
	}
	break;

	case TYPE_Y:
	{
		newpos.y = m_Info.touchpos.y + move.y;
		CDebugProc::GetInstance()->Print("Y�I\n");
	}
	break;

	case TYPE_Z:
	{
		newpos.z = m_Info.touchpos.z + move.z;
		CDebugProc::GetInstance()->Print("Z�I\n");
	}
	break;

	default:
		break;
	}

	m_pos = newpos - (m_Info.touchpos - m_Info.startpos);
}

//==========================================================
// �I��
//==========================================================
void CEdit_Arrow::Select()
{
	// ���͊m�F
	CInputMouse* pMouse = CInputMouse::GetInstance();
	bool trigger = pMouse->GetTrigger(CInputMouse::BUTTON_LBUTTON);

	// �}�E�X���C���擾
	D3DXVECTOR3 origin = pMouse->GetRayInfo().origin;
	D3DXVECTOR3 vec = pMouse->GetRayInfo().vec;
	D3DXVECTOR3 touchpos = m_Info.startpos;
	float length = 1000000.0f;

	if (!trigger) { return; }

	for (int i = 0; i < TYPE_MAX; i++)
	{
		if (m_aObj[i].pObj == nullptr) { continue; }

		D3DXVECTOR3 pos = m_aObj[i].pObj->GetPosition();
		D3DXVECTOR3 rot = VECTOR3_ZERO;
		D3DXVECTOR3 vtxmax = m_aObj[i].pObj->GetVtxMax();
		D3DXVECTOR3 vtxmin = m_aObj[i].pObj->GetVtxMin();

		// �����蔻��
		bool hit = collision::CollideRayToOBB(&touchpos, origin, vec, pos, rot, vtxmax, vtxmin);

		// �����������̂�
		if (!hit) { continue; }

		D3DXVECTOR3 diff = (touchpos - origin);
		float nowlength = D3DXVec3Length(&diff);

		// ����
		if (nowlength > length) { continue; }

		// �L�� �G�ꂽ���W�擾
		m_pHold = &m_aObj[i];
		m_Info.startpos = m_pos;
		m_Info.touchpos = touchpos;
		m_Info.touchworldpos = pMouse->GetWorldInfo().pos;
		length = nowlength;
	}
}

//==========================================================
// ����
//==========================================================
void CEdit_Arrow::Release()
{
	CInputMouse* pMouse = CInputMouse::GetInstance();
	bool press = pMouse->GetPress(CInputMouse::BUTTON_LBUTTON);
	if (press) { return; }

	m_pHold = nullptr;
	m_Info.startpos = m_pos;
}