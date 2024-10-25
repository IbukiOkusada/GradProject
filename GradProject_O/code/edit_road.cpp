//==========================================================
//
// �G�f�B�^�[�� [edit_road.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "edit_road.h"
#include "input_mouse.h"
#include "input_keyboard.h"
#include "debugproc.h"
#include "road_manager.h"
#include "road.h"
#include "object3D.h"
#include "edit_arrow.h"

namespace
{
	const float MIN_LENGTH = 100.0f;	// �ŏ��ړ���
	const char* FILENAME = "data\\FILE\\map\\road.bin";
	const D3DXVECTOR2 SET_SIZE = D3DXVECTOR2(600.0f, 600.0f);
}

//==========================================================
// �R���X�g���N�^
//==========================================================
CEdit_Road::CEdit_Road()
{
	// �l�̃N���A
	m_pSelectRoad = nullptr;
	m_pArrow = nullptr;
}

//==========================================================
// �f�X�g���N�^
//==========================================================
CEdit_Road::~CEdit_Road()
{

}

//==========================================================
// ����������
//==========================================================
HRESULT CEdit_Road::Init(void)
{
	m_fMoveLength = MIN_LENGTH;
	return S_OK;
}

//==========================================================
// �I������
//==========================================================
void CEdit_Road::Uninit(void)
{
	// ���I��
	if (m_pArrow != nullptr)
	{
		m_pArrow->Uninit();
		m_pArrow = nullptr;
	}

	m_pSelectRoad = nullptr;

	CEdit::Uninit();
}

//==========================================================
// �X�V����
//==========================================================
void CEdit_Road::Update(void)
{
	CDebugProc::GetInstance()->Print(" [ ���z�u���[�h ]\n");
	CInputKeyboard* pKey = CInputKeyboard::GetInstance();
	CRoad* pOld = m_pSelectRoad;

	// �I��
	ClickCheck();

	CDebugProc::GetInstance()->Print("[");

	// �ĘA��
	ReConnect();

	// �ۑ�
	Save();

	if (pKey->GetPress(DIK_LALT) || pKey->GetPress(DIK_RALT)) { CDebugProc::GetInstance()->Print("]\n"); return; }

	// �I������Ă��Ȃ��A�������͑I����������
	if (m_pSelectRoad == nullptr || pOld == nullptr) { 

		// ����
		Create();

		CDebugProc::GetInstance()->Print("]\n"); 
		return; 
	}

	// �I�����ꂽ���̂�F�ς���
	m_pSelectRoad->GetObj()->SetCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f));

	// ���̍X�V
	if (m_pArrow != nullptr)
	{
		m_pArrow->Update();
	}

	// �ړ�
	Move();

	// �폜
	Delete();

	CDebugProc::GetInstance()->Print("]\n\n");

	// �����
	if (m_pSelectRoad == nullptr) { return; }
	CDebugProc::GetInstance()->Print("[ ����� : ");
	D3DXVECTOR3 pos = m_pSelectRoad->GetPosition();
	CDebugProc::GetInstance()->Print("���W : [ %f, %f, %f] : ", pos.x, pos.y, pos.z);
	CDebugProc::GetInstance()->Print("]\n");
}

//==========================================================
// �I��
//==========================================================
void CEdit_Road::ClickCheck()
{
	CInputMouse* pMouse = CInputMouse::GetInstance();
	CRoad* pRoad = CRoadManager::GetInstance()->GetTop();

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

	if (m_pSelectRoad != nullptr)
	{
		m_pSelectRoad->GetObj()->SetCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	}

	m_pSelectRoad = nullptr;

	// ����S�Ċm�F
	while (pRoad != nullptr)
	{
		CRoad* pNext = pRoad->GetNext();

		// �Փ˂���
		if (CursorCollision(pRoad))
		{
			m_pSelectRoad = pRoad;

			if (m_pArrow == nullptr)
			{
				m_pArrow = CEdit_Arrow::Create(m_pSelectRoad->GetPosition());
			}

			m_pArrow->SetPosition(m_pSelectRoad->GetPosition());

			return;
		}

		pRoad = pNext;
	}

	// �����I�΂�Ă��Ȃ�
	if (m_pSelectRoad != nullptr) { return; }

	// ���g�p��
	if (m_pArrow == nullptr) { return; }

	m_pArrow->Uninit();
	m_pArrow = nullptr;
}

//==========================================================
// �I���������̂Ƃ̏Փ˔���
//==========================================================
bool CEdit_Road::CursorCollision(CRoad* pRoad)
{
	// �}�E�X���
	CInputMouse* pMouse = CInputMouse::GetInstance();
	CInputMouse::SRayInfo info = pMouse->GetRayInfo();

	// �����
	D3DXVECTOR3 pos = pRoad->GetPosition();
	VERTEX_3D* pVtx = pRoad->GetObj()->GetVertex();

	// �g�p����Ă��Ȃ�������Ԃ�
	if (pVtx == nullptr) { return false; }

	D3DXVECTOR3 pos0 = pos + pVtx[0].pos, pos1 = pos + pVtx[1].pos, pos2 = pos + pVtx[2].pos;

	// �O�p�`�Ɛ����̏Փˊm�F
	if (TriangleCollision(info.origin, info.vec, pos0, pos1, pos2))
	{
		return true;
	}

	// ���΂̎O�p��
	pos0 = pos + pVtx[3].pos;

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
bool CEdit_Road::TriangleCollision(const D3DXVECTOR3& rayOrigin, const D3DXVECTOR3& rayDir,
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
// �q���Ȃ���
//==========================================================
void CEdit_Road::ReConnect()
{
	CDebugProc::GetInstance()->Print(" ���ĘA�� : F5, ");
	CInputKeyboard* pKey = CInputKeyboard::GetInstance();

	// ���͊m�F
	if (!pKey->GetTrigger(DIK_F5)) { return; }

	// �Đڑ�
	CRoadManager::GetInstance()->AllConnect();
}

//==========================================================
// �폜
//==========================================================
void CEdit_Road::Delete()
{
	CDebugProc::GetInstance()->Print(" ���폜 : Delete or BackSpace, ");
	CInputKeyboard* pKey = CInputKeyboard::GetInstance();

	// ���͊m�F
	if (!pKey->GetTrigger(DIK_DELETE) && !pKey->GetTrigger(DIK_BACKSPACE)) { return; }

	// �Đڑ�
	m_pSelectRoad->Uninit();
	m_pSelectRoad = nullptr;

	// �ĘA��
	ReConnect();

	// ���g�p��
	if (m_pArrow == nullptr) { return; }

	m_pArrow->Uninit();
	m_pArrow = nullptr;
}

//==========================================================
// �ړ�
//==========================================================
void CEdit_Road::Move()
{
	if (m_pSelectRoad == nullptr) { return; }
	if (m_pArrow == nullptr) { return; }

	D3DXVECTOR3 pos = m_pSelectRoad->GetPosition();	// ���W
	D3DXVECTOR3 Arrowpos = m_pArrow->GetPosition();	// �����W

	// X���W
	float length = fabsf(Arrowpos.x - pos.x);
	int setpos = static_cast<int>(Arrowpos.x);
	int movelength = static_cast<int>(m_fMoveLength);

	// �傫��
	if (length >= m_fMoveLength)
	{
		pos.x = static_cast<float>(setpos - (setpos % movelength));
	}

	// Y���W
	length = fabsf(Arrowpos.y - pos.y);
	setpos = static_cast<int>(Arrowpos.y);
	movelength = static_cast<int>(m_fMoveLength);

	// �傫��
	if (length >= m_fMoveLength)
	{
		pos.y = static_cast<float>(setpos - (setpos % movelength));
	}

	// Z���W
	length = fabsf(Arrowpos.z - pos.z);
	setpos = static_cast<int>(Arrowpos.z);
	movelength = static_cast<int>(m_fMoveLength);

	// �傫��
	if (length >= m_fMoveLength)
	{
		pos.z = static_cast<float>(setpos - (setpos % movelength));
	}

	// �I���������̍��W�ݒ�
	m_pSelectRoad->SetPosition(pos);
}

//==========================================================
// �ۑ�
//==========================================================
void CEdit_Road::Save()
{
	CDebugProc::GetInstance()->Print(" �ۑ� : F7, ");
	CInputKeyboard* pKey = CInputKeyboard::GetInstance();

	// ���͊m�F
	if (!pKey->GetTrigger(DIK_F7)) { return; }

	CRoadManager* pMgr = CRoadManager::GetInstance();
	// �t�@�C�����J��
	std::ofstream File(FILENAME, std::ios::binary);
	if (!File.is_open()) {
		return;
	}

	std::vector<CRoad::SInfo> savedata;

	for (int i = 0; i < pMgr->GetList()->GetNum(); i++)
	{
		savedata.push_back(*pMgr->GetList()->Get(i));
	}

	int size = savedata.size();

	// �x�N�g���̃T�C�Y���Z�[�u
	File.write(reinterpret_cast<const char*>(&size), sizeof(size));

	// �f�[�^���o�C�i���t�@�C���ɏ����o��
	File.write(reinterpret_cast<char*>(savedata.data()), size * sizeof(CRoad::SInfo));

	// �t�@�C�������
	File.close();
}

//==========================================================
// �ۑ�
//==========================================================
void CEdit_Road::Create()
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

	CRoad::Create(pos, VECTOR3_ZERO, SET_SIZE);
}