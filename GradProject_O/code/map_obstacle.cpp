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
#include "edit_manager.h"

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
	correction::Adjust(&m_Info.rot);
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

	// �I�u�W�F�N�g�̍X�V
	if (m_pObj != nullptr)
	{
		m_pObj->DrawCheck();
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
	correction::Adjust(&m_Info.rot);

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
	CMapManager* pMgr = CMapManager::GetInstance();
	if (idx < 0 || idx >= static_cast<int>(pMgr->GetFileNameList().size())) { m_Info.fileidx = 0;}

	m_Info.fileidx = idx;

	if (m_pObj == nullptr) { return; }

	std::vector<std::string> str = pMgr->GetFileNameList();

	m_pObj->BindFile(str[idx].c_str());
}
