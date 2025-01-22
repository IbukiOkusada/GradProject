//==========================================================
//
// �G�f�B�^�[��] [edit_rotate.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "edit_rotate.h"
#include "objectX.h"
#include "input_mouse.h"
#include "debugproc.h"
#include "camera.h"
#include "camera_manager.h"

// �������O���
namespace
{
	const float DEF_LENGTH = 20000.0f;
	const char* FILENAME[CEdit_Rotate::VEC_MAX] = {	// ���f���t�@�C����
		"data\\MODEL\\edit\\cube.x",
		"data\\MODEL\\edit\\x_rotate.x",
		"data\\MODEL\\edit\\y_rotate.x",
		"data\\MODEL\\edit\\z_rotate.x",
	};
}

//==========================================================
// �R���X�g���N�^
//==========================================================
CEdit_Rotate::CEdit_Rotate()
{

}

//==========================================================
// �f�X�g���N�^
//==========================================================
CEdit_Rotate::~CEdit_Rotate()
{

}

//==========================================================
// ����
//==========================================================
CEdit_Rotate* CEdit_Rotate::Create(const D3DXVECTOR3& pos)
{
	CEdit_Rotate* pRotate = DEBUG_NEW CEdit_Rotate;

	if (pRotate != nullptr)
	{
		pRotate->Init();
		pRotate->SetPosition(pos);
		pRotate->m_Info.startpos = pos;
		pRotate->m_Info.touchpos = pos;
	}

	return pRotate;
}

//==========================================================
// ����������
//==========================================================
HRESULT CEdit_Rotate::Init(void)
{
	m_pos = VECTOR3_ZERO;

	// ���f������
	for (int i = 0; i < VEC_MAX; i++)
	{
		m_aObj[i].pObj = CObjectX::Create(VECTOR3_ZERO, VECTOR3_ZERO, FILENAME[i], 2);
		m_aObj[i].pObj->SetScale(D3DXVECTOR3(0.5f, 0.5f, 0.5f));
		m_aObj[i].pObj->SetType(CObject::TYPE_EDIT);
	}

	return S_OK;
}

#if 0
//==========================================================
// �I������
//==========================================================
void CEdit_Rotate::Uninit(void)
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
#endif

#if 0
//==========================================================
// �X�V����
//==========================================================
void CEdit_Rotate::Update(void)
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

		if (m_pHold != &m_aObj[i]) {
			m_aObj[i].pObj->SetColMulti(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
			m_aObj[i].pObj->SetColAdd(D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f));
		}
		else {
			m_aObj[i].pObj->SetColMulti(D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f));
			m_aObj[i].pObj->SetColAdd(D3DXCOLOR(0.3f, 0.3f, 1.0f, 1.0f));
		}
	}
}
#endif

//==========================================================
// ���W�ݒ�
//==========================================================
void CEdit_Rotate::Rotation()
{
	// ����ł���Ƃ��̂�
	if (GetHold() == nullptr) { return; }

	// ���͊m�F
	CInputMouse* pMouse = CInputMouse::GetInstance();
	bool press = pMouse->GetPress(CInputMouse::BUTTON_LBUTTON);
	if (!press) { return; }

	D3DXVECTOR3 worldpos = pMouse->GetWorldInfo().pos;
	D3DXVECTOR3 move = worldpos - m_Info.touchworldpos;
	move *= (CCameraManager::GetInstance()->GetTop()->GetLength() / DEF_LENGTH);

	// 4. �V�����ʒu���v�Z
	D3DXVECTOR3 newpos = m_Info.startpos;

	// ��ގw��
	switch (GetHold()->type)
	{
	case VEC_ALL:
	{
		newpos = m_Info.startpos + move;
		CDebugProc::GetInstance()->Print("�S���I");
	}
	break;

	case VEC_X:
	{
		newpos.x = m_Info.startpos.x + move.x;
		CDebugProc::GetInstance()->Print("X�I");
	}
	break;

	case VEC_Y:
	{
		newpos.y = m_Info.startpos.y + move.y;
		CDebugProc::GetInstance()->Print("Y�I");
	}
	break;

	case VEC_Z:
	{
		newpos.z = m_Info.startpos.z + move.z;
		CDebugProc::GetInstance()->Print("Z�I");
	}
	break;

	default:
		break;
	}

	m_rot = (newpos - m_Info.startpos);
	correction::Adjust(m_rot);

	CDebugProc::GetInstance()->Print("���� [ %f, %f, %f ]\n", m_rot.x, m_rot.y, m_rot.z);
}
