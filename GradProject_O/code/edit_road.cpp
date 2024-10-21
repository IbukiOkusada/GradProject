//==========================================================
//
// �G�f�B�^�[�� [edit_road.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "edit_road.h"
#include "input_mouse.h"
#include "debugproc.h"
#include "road_manager.h"
#include "road.h"
#include "object3D.h"

//==========================================================
// �R���X�g���N�^
//==========================================================
CEdit_Road::CEdit_Road()
{
	// �l�̃N���A
	m_pSelectRoad = nullptr;
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
	return S_OK;
}

//==========================================================
// �I������
//==========================================================
void CEdit_Road::Uninit(void)
{
	CEdit::Uninit();
}


//==========================================================
// �X�V����
//==========================================================
void CEdit_Road::Update(void)
{
	CDebugProc::GetInstance()->Print(" [ ���z�u���[�h ]\n");

	// �I��
	ClickCheck();

	if (m_pSelectRoad != nullptr)
	{
		m_pSelectRoad->GetObj()->SetCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f));
	}
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
		}

		pRoad = pNext;
	}
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
