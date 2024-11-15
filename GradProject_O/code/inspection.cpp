//==========================================================
//
// ���⏈�� [inspection.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "inspection.h"
#include "add_police.h"
#include "objectX.h"
#include "debugproc.h"
#include "player.h"
#include "player_manager.h"
#include "deltatime.h"

// �������O��Ԃ��`
namespace
{
	const float POLICE_SETROT = D3DX_PI * 0.5f;
	const float POLICE_SETLENGTH = 700.0f;
	const float TIMER = 10.0f;
}

//==========================================================
// �R���X�g���N�^
//==========================================================
CInstpection::CInstpection()
{
	// �l�̃N���A
	m_Guard = SGuardInfo();
	m_Info = SInfo();
	m_bHit = false;
}

//==========================================================
// �f�X�g���N�^
//==========================================================
CInstpection::~CInstpection()
{

}

//==========================================================
// ����������
//==========================================================
HRESULT CInstpection::Init(void)
{
	// ��̐���
	m_Guard.pObj = CObjectX::Create(m_Info.pos, m_Info.rot, "data\\MODEL\\map\\guardrail001.x");
	m_Guard.pObj->SetScale(D3DXVECTOR3(5.0f, 2.0f, 2.0f));

	// �p�g�J�[�̐ݒ�
	{
		// �����擾
		D3DXVECTOR3 rot = m_Info.rot;
		rot.y = POLICE_SETROT + m_Info.rot.y;
		Adjust(rot);

		// ���W�擾
		D3DXVECTOR3 pos = m_Info.pos;
		pos.x += sinf(rot.y) * POLICE_SETLENGTH;
		pos.z += cosf(rot.y) * POLICE_SETLENGTH;

		// ����
		m_pPolice = CAddPolice::Create(pos, rot, VECTOR3_ZERO);
		m_pPolice->SetState(CPolice::STATE::STATE_STOP);
	}

	return S_OK;
}

//==========================================================
// �I������
//==========================================================
void CInstpection::Uninit(void)
{
	// �x�@�p��
	if (m_pPolice != nullptr)
	{
		m_pPolice = nullptr;
	}

	if (m_Guard.pObj != nullptr)
	{
		m_Guard.pObj = nullptr;
	}

	Release();
}

//==========================================================
// �X�V����
//==========================================================
void CInstpection::Update(void)
{
	if (m_Guard.pObj != nullptr)
	{
		if (m_Guard.pObj->GetHit() || m_Guard.pObj->GetHitOld())
		{
			m_bHit = true;

			// �����蔻��𖳂��ɂ���
			m_Guard.pObj->SetEnableCollision(false);

			CPlayer* pPlayer = CPlayerManager::GetInstance()->GetTop();
			float rot = atan2f(m_Guard.pObj->GetPosition().x - pPlayer->GetPosition().x, m_Guard.pObj->GetPosition().z - pPlayer->GetPosition().z);
			float speed = CPlayerManager::GetInstance()->GetTop()->GetEngine();

			// ���W�ݒ�
			m_Guard.targetpos = {
				m_Guard.pObj->GetPosition().x + sinf(rot) * speed * 500.0f,
				m_Guard.pObj->GetPosition().y,
				m_Guard.pObj->GetPosition().z + cosf(rot) * speed * 500.0f,
			};

			m_Guard.targetrot = {
				D3DX_PI * 0.5f,
				rot,
				0.0f
			};

			m_Guard.startpos = m_Guard.pObj->GetPosition();
			m_Guard.startrot = m_Guard.pObj->GetRotation();

			// �x�@��؂藣��
			if (m_pPolice != nullptr)
			{
				m_pPolice->SetState(CPolice::STATE::STATE_SEARCH);
				m_pPolice = nullptr;
			}
		}
	}

	// �f�o�b�O�\��
	CDebugProc::GetInstance()->Print("����̍��W : [ %f, %f, %f]\n", m_Info.pos.x, m_Info.pos.y, m_Info.pos.z);

	// �Փ˂��Ă��Ȃ�
	if (!m_bHit) { return; }

	// �ړ�
	Away();
}

//==========================================================
// ����
//==========================================================
CInstpection* CInstpection::Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot)
{
	CInstpection* pInsp = nullptr;

	pInsp = DEBUG_NEW CInstpection;

	if (pInsp != nullptr)
	{
		// �l�ݒ�
		pInsp->SetPosition(pos);
		pInsp->SetRotation(rot);

		// ����������
		pInsp->Init();
	}

	return pInsp;
}

//==========================================================
// �ړ�
//==========================================================
void CInstpection::Away()
{
	// �ړ�����
	if (m_Guard.pObj == nullptr) { return; }
	if (!m_bHit) { return; }

	// �ʒu�X�V
	D3DXVECTOR3 posdiff = m_Guard.targetpos - m_Guard.pObj->GetPosition();
	D3DXVECTOR3 setpos = m_Guard.pObj->GetPosition() + posdiff * 0.1f;
	m_Guard.pObj->SetPosition(setpos);

	// �����X�V
	D3DXVECTOR3 rotdiff = m_Guard.targetrot - m_Guard.pObj->GetRotation();
	Adjust(rotdiff);
	D3DXVECTOR3 setrot = m_Guard.pObj->GetRotation() + rotdiff * 0.1f;
	Adjust(setrot);
	m_Guard.pObj->SetRotation(setrot);

	m_Guard.fTimer += CDeltaTime::GetInstance()->GetDeltaTime();
}