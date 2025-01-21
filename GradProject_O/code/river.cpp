//===============================================
//
// ��̏��� [river.cpp]
// Author : Ibuki Okusada
//
//===============================================
#include "river.h"
#include "meshfield.h"
#include "player.h"
#include "player_manager.h"

namespace
{
	const char* FILENAME[river::MAX] =
	{
		"data\\TEXTURE\\river000.jpg",
		"data\\TEXTURE\\river000.png",
	};
}

//===============================================
// �R���X�g���N�^
//===============================================
CRiver::CRiver(SInfo info) :
	m_Info(info)
{
	// �l�̃N���A
	m_obj.clear();
}

//===============================================
// �f�X�g���N�^
//===============================================
CRiver::~CRiver()
{
	
}

//===============================================
// ��������
//===============================================
CRiver* CRiver::Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, const D3DXVECTOR2& size,
	const D3DXVECTOR2& speed, const int& nWidth, const int& nHeight)
{
	CRiver* pRiver = DEBUG_NEW CRiver(SInfo(pos, rot, size, speed, nWidth, nHeight));

	// �������s
	if (pRiver == nullptr) { return nullptr; }

	// ����������
	pRiver->Init();

	return pRiver;
}

//===============================================
// ����������
//===============================================
HRESULT CRiver::Init(void)
{
	// �I�u�W�F�N�g�̏�����
	for (int i = 0; i < river::MAX; i++)
	{
		// ���W�ݒ�
		D3DXVECTOR3 pos = VECTOR3_ZERO;
		pos.y += i * 1.0f;
		
		// ����
		CMeshField* p = CMeshField::Create(m_Info.pos + pos, m_Info.rot, m_Info.size.x, m_Info.size.y, FILENAME[i],
			m_Info.nWidth, m_Info.nHeight);

		// �X�N���[�����x�ݒ�
		p->SetTexMove(m_Info.speed + (m_Info.speed * static_cast<float>(i)));

		// �Փːݒ�
		p->SetCollision(false);

		// ���X�g�ɑ}��
		m_obj.push_back(p);
	}

	return S_OK;
}

//===============================================
// �I������
//===============================================
void CRiver::Uninit(void)
{
	// �I�u�W�F�N�g�̏I��
	for (const auto& it : m_obj)
	{
		it->Uninit();
	}

	m_obj.clear();

	// ���
	Release();
}

//===============================================
// �X�V����
//===============================================
void CRiver::Update(void)
{
	// �I�u�W�F�N�g�̍X�V
	for (const auto& it : m_obj)
	{
		it->Update();
	}

	// �v���C���[�̗����m�F
	CPlayer* pPlayer = CPlayerManager::GetInstance()->GetPlayer();
	if (pPlayer == nullptr) { return; }

	if (pPlayer->GetPosition().y <= m_Info.pos.y)
	{
		pPlayer->SetPosition(VECTOR3_ZERO);
		pPlayer->Damage(pPlayer->GetLifeOrigin() * 0.1f);
	}
}