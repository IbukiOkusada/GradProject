//==========================================================
//
// �}�b�v����Q���̏��� [map_obstacle.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "map_obstacle.h"
#include "map_manager.h"
#include "objectX.h"
#include "manager.h"
#include "renderer.h"
#include "camera.h"
#include "camera_manager.h"
#include "game.h"

namespace
{
	const float RANGE_WIDTH = SCREEN_WIDTH * 0.1f;
	const float RANGE_HEIGHT = SCREEN_HEIGHT * 0.1f;
}

// �ÓI�����o�ϐ��錾
Clist<CMapObstacle*>* CMapObstacle::m_pList = nullptr;

//==========================================================
// �R���X�g���N�^
//==========================================================
CMapObstacle::CMapObstacle(const SInfo& info)
{
	//���g�����X�g�ɓo�^
	GetInstance()->Regist(this);
	m_Info = info;
	m_pObj = nullptr;
}

//==========================================================
// �f�X�g���N�^
//==========================================================
CMapObstacle::~CMapObstacle()
{
	//���g�����X�g����폜
	GetInstance()->Delete(this);
	if (GetInstance()->Empty())
	{//���X�g�̒��g����Ȃ烊�X�g��j��
		ListRelease();
	}
}

//==========================================================
// ����������
//==========================================================
HRESULT CMapObstacle::Init(void)
{
	std::vector<std::string> str = CMapManager::GetInstance()->GetFileNameList();
	m_pObj = CObjectX::Create(m_Info.pos, m_Info.rot, str[m_Info.fileidx].c_str());
	m_pObj->SetScale(m_Info.scale);

	return S_OK;
}

//==========================================================
// ����������(�I�[�o�[���[�h)
//==========================================================
HRESULT CMapObstacle::Init(const std::string& filename)
{
	m_pObj = CObjectX::Create(m_Info.pos, m_Info.rot, filename.c_str());

	return S_OK;
}

//==========================================================
// �I������
//==========================================================
void CMapObstacle::Uninit(void)
{
	if (m_pObj != nullptr)
	{
		m_pObj->Uninit();
		m_pObj = nullptr;
	}

	Release();
}

//==========================================================
// �X�V����
//==========================================================
void CMapObstacle::Update(void)
{
	DrawCheck();

	// �I�u�W�F�N�g�̍X�V
	if (m_pObj != nullptr)
	{
		m_pObj->SetPosition(m_Info.pos);
		m_pObj->SetRotation(m_Info.rot);
		m_pObj->SetScale(m_Info.scale);
	}
}

//==========================================================
// ����
//==========================================================
CMapObstacle* CMapObstacle::Create(const SInfo& info)
{
	CMapObstacle* pGoal = nullptr;

	pGoal = DEBUG_NEW CMapObstacle(info);

	if (pGoal != nullptr)
	{
		pGoal->m_Info = info;
		// ����������
		pGoal->Init();
	}

	return pGoal;
}

//==========================================================
// ���W�ݒ�
//==========================================================
void CMapObstacle::SetPosition(const D3DXVECTOR3& pos)
{
	m_Info.pos = pos;

	// �I�u�W�F�N�g�̍X�V
	if (m_pObj != nullptr)
	{
		m_pObj->SetPosition(m_Info.pos);
	}
}

//==========================================================
// �����ݒ�
//==========================================================
void CMapObstacle::SetRotation(const D3DXVECTOR3& rot)
{
	m_Info.rot = rot;

	// �I�u�W�F�N�g�̍X�V
	if (m_pObj != nullptr)
	{
		m_pObj->SetRotation(m_Info.rot);
	}
}

//==========================================================
// �X�P�[���ݒ�
//==========================================================
void CMapObstacle::SetScale(const D3DXVECTOR3& scale)
{
	m_Info.scale = scale;

	// �I�u�W�F�N�g�̍X�V
	if (m_pObj != nullptr)
	{
		m_pObj->SetScale(m_Info.scale);
	}
}

//==========================================================
// ���f���ݒ�
//==========================================================
void CMapObstacle::BindModel(const int& idx)
{
	if (idx < 0 || idx >= CMapManager::GetInstance()->GetFileNameList().size()) { m_Info.fileidx = 0;}

	m_Info.fileidx = idx;

	if (m_pObj == nullptr) { return; }

	std::vector<std::string> str = CMapManager::GetInstance()->GetFileNameList();

	m_pObj->BindFile(str[idx].c_str());
}

//==========================================================
// �`��m�F
//==========================================================
void CMapObstacle::DrawCheck()
{
	// ���ΐ����ړ�����
	if (m_pObj == nullptr) { return; }

	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();
	D3DXMATRIX mtxProjection, mtxView, mtxWorld;
	D3DVIEWPORT9 Viewport;
	D3DXVECTOR3 pos = VECTOR3_ZERO;
	D3DXVECTOR3 mypos = m_pObj->GetPosition();

	// �K�v�ȏ��擾
	pDevice->GetTransform(D3DTS_PROJECTION, &mtxProjection);
	pDevice->GetTransform(D3DTS_VIEW, &mtxView);
	pDevice->GetViewport(&Viewport);

	//���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&mtxWorld);

	// �X�N���[�����W�擾
	D3DXVec3Project(&pos, &mypos, &Viewport, &mtxProjection, &mtxView, &mtxWorld);

	float f = 0.5f * (m_Info.scale.x + m_Info.scale.z);

	// ��ʊO�Ȃ�o���Ȃ�
	if (pos.x < 0.0f - (SCREEN_WIDTH * 0.3f * f) || pos.x > SCREEN_WIDTH + (SCREEN_WIDTH * 0.3f * f) ||
		pos.y < 0.0f - (SCREEN_HEIGHT * 0.3f * m_Info.scale.y) || pos.y > SCREEN_HEIGHT + (SCREEN_HEIGHT * 0.3f * m_Info.scale.y) ||
		pos.z >= 1.0f) {

		// �F�𓧖��ɋ߂Â���
		D3DXCOLOR col = m_pObj->GetColMulti();
		if (col.a > 0.0f)
		{
			col.a -= 0.1f;
			
			if (col.a <= 0.0f)
			{
				col.a = 0.0f;
				m_pObj->SetDraw(false);
			}
		}

		m_pObj->SetColMulti(col);

		return;
	}

	// ���������
	{
		CCamera* pCamera = CCameraManager::GetInstance()->GetTop();
		D3DXVECTOR3 lenpos = pCamera->GetPositionR() - m_Info.pos;
		if (D3DXVec3Length(&lenpos) >= Game::DOME_LENGTH)
		{
			m_pObj->SetDraw(false);
			return;
		}
	}

	// �F�����ɋ߂Â���
	D3DXCOLOR col = m_pObj->GetColMulti();
	if (col.a < 1.0f)
	{
		col.a += 0.1f;

		if (col.a >= 1.0f)
		{
			col.a = 1.0f;
		}
	}

	m_pObj->SetColMulti(col);
	m_pObj->SetDraw();
}
