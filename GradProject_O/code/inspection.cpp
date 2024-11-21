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
#include "gimmick_policestation.h"
#include "a_star.h"
#include "particle3D.h"

// �������O��Ԃ��`
namespace
{
	const float POLICE_SETLENGTH = 700.0f;	// �x�@�ݒu����
	const float TIMER = 10.0f;				// �^�C�}�[
	const float START_SPEEDDEST = 40.0f;	// �X�^�[�g
	const float STOP_TIMER = 3.0f;			// �X�g�b�v
	const float ROTATE_TIMER = 1.0f;		// ��]
	const float LAGER_TIMER = 3.0f;			// ������o��܂ł̑��x
}

//==========================================================
// �R���X�g���N�^
//==========================================================
CInstpection::CInstpection()
{
	// �l�̃N���A
	m_Info = SInfo();
	m_pNearStation = nullptr;
	m_pRoad = nullptr;
	m_LagerInfo = SLagerInfo();

	for (int i = 0; i < TYPE::TYPE_MAX; i++)
	{
		m_aPoliceInfo[i] = SPoliceInfo();
	}
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
	// ���W�擾
	D3DXVECTOR3 pos = m_Info.pos;

	// ����
	m_pNearStation = CGimmickPoliceStation::GetNear(GetPosition());

	// �����擾
	D3DXVECTOR3 rot = m_Info.rot;
	rot.y = m_Info.rot.y;
	Adjust(rot);

	// �o������p�g�J�[�̐���
	for (int i = 0; i < TYPE::TYPE_MAX; i++)
	{
		if (m_pNearStation != nullptr) { pos = m_pNearStation->GetPos(); }

		// ���W�擾
		D3DXVECTOR3 goalpos = m_Info.pos;
		goalpos.x += sinf(rot.y) * POLICE_SETLENGTH;
		goalpos.z += cosf(rot.y) * POLICE_SETLENGTH;

		m_aPoliceInfo[i].pPolice = CAddPolice::Create(pos, VECTOR3_ZERO, VECTOR3_ZERO);
		m_aPoliceInfo[i].goalpos = goalpos;

		// �o�H��ݒ�
		if (m_pRoad != nullptr)
		{
			m_aPoliceInfo[i].pPolice->SetNavi(AStar::AStar(m_pNearStation->GetRoad()->GetSearchSelf(), m_pRoad->GetSearchSelf()));
			m_aPoliceInfo[i].pPolice->SetState(CPolice::STATE::STATE_NORMAL);
		}

		rot.y += D3DX_PI;
		Adjust(rot.y);
	}

	return S_OK;
}

//==========================================================
// �I������
//==========================================================
void CInstpection::Uninit(void)
{
	// �x�@�p��
	for (int i = 0; i < TYPE::TYPE_MAX; i++)
	{
		if (m_aPoliceInfo[i].pPolice != nullptr)
		{
			m_aPoliceInfo[i].pPolice = nullptr;
			m_aPoliceInfo[i] = SPoliceInfo();
		}
	}

	Release();
}

//==========================================================
// �X�V����
//==========================================================
void CInstpection::Update(void)
{
	// �f�o�b�O�\��
	CDebugProc::GetInstance()->Print("����̍��W : [ %f, %f, %f]\n", m_Info.pos.x, m_Info.pos.y, m_Info.pos.z);

	// �ړ�
	Away();

	// ������z�u
	LagerSet();
}

//==========================================================
// ����
//==========================================================
CInstpection* CInstpection::Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, CRoad* pRoad)
{
	CInstpection* pInsp = nullptr;

	pInsp = DEBUG_NEW CInstpection;

	if (pInsp != nullptr)
	{
		// �l�ݒ�
		pInsp->SetPosition(pos);
		pInsp->SetRotation(rot);
		pInsp->m_pRoad = pRoad;

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
	for (int i = 0; i < TYPE::TYPE_MAX; i++)
	{
		SPoliceInfo* pInfo = &m_aPoliceInfo[i];
		// ��~��Ԃ̎���������
		if (pInfo->pPolice == nullptr) { continue; }
		if (pInfo->pPolice->GetState() != CPolice::STATE::STATE_STOP) { continue; }
		if (pInfo->fTimer >= STOP_TIMER) { continue; }

		// ����̒n�_��ۑ�
		if (pInfo->fTimer == 0.0f) { 
			pInfo->startpos = pInfo->pPolice->GetPosition();
			pInfo->goalrot.y = atan2f(pInfo->goalpos.x - pInfo->startpos.x, pInfo->goalpos.z - pInfo->startpos.z);
			Adjust(pInfo->goalrot.y);
		}

		// �^�C�}�[��i�s
		pInfo->fTimer += CDeltaTime::GetInstance()->GetDeltaTime();
		float range = pInfo->fTimer / STOP_TIMER;
		if (range >= 1.0f) { range = 1.0f; }

		// �ړ�
		D3DXVECTOR3 diff = pInfo->goalpos - pInfo->startpos;
		D3DXVECTOR3 pos = pInfo->startpos + diff * range;
		pInfo->pPolice->SetPosition(pos);

		// ����
		D3DXVECTOR3 rot = pInfo->pPolice->GetRotation();
		float rotdiff = pInfo->goalrot.y - rot.y;
		Adjust(rotdiff);
		rot.y += rotdiff * 0.1f;
		Adjust(rot);
		pInfo->pPolice->SetRotation(rot);
	}
}

//==========================================================
// �������z�u
//==========================================================
void CInstpection::LagerSet()
{
	// ��~���Ă���Ƃ��̂�
	if (m_aPoliceInfo[TYPE::TYPE_SEARCH_L].fTimer < STOP_TIMER
		|| m_aPoliceInfo[TYPE::TYPE_SEARCH_R].fTimer < STOP_TIMER)
	{
		return;
	}

	// ��]����
	LagerSetRotation();

	// �^�C�}�[��i�߂�
	if (m_LagerInfo.fTimer < LAGER_TIMER)
	{
		m_LagerInfo.fTimer += CDeltaTime::GetInstance()->GetDeltaTime();

		if (m_LagerInfo.fTimer >= LAGER_TIMER)
		{
			m_LagerInfo.fTimer = LAGER_TIMER;
		}
	}

	// ���������߂�
	float range = m_LagerInfo.fTimer / LAGER_TIMER;
	int cnt = static_cast<int>(range * 10.0f);

	for (int i = 0; i < TYPE::TYPE_MAX; i++)
	{
		SPoliceInfo* pInfo = &m_aPoliceInfo[i];

		D3DXVECTOR3 posdiff = m_Info.pos - pInfo->goalpos;
		D3DXVECTOR3 nor = posdiff;
		D3DXVec3Normalize(&nor, &nor);

		// �w�萔���G�t�F�N�g����
		for (int j = 0; j < cnt; j++)
		{
			D3DXVECTOR3 pos = pInfo->goalpos + posdiff * (static_cast<float>(j) * 0.1f);
			pos.y += 10.0f;
			CParticle3D::Create(pos, nor, CEffect3D::TYPE::TYPE_LASER);
		}
	}
}

//==========================================================
// ���⎞�̌����ݒ�
//==========================================================
void CInstpection::LagerSetRotation()
{
	// �^�C�}�[��i�s
	if (m_LagerInfo.fRotateTimer <= ROTATE_TIMER) {
		m_LagerInfo.fRotateTimer += CDeltaTime::GetInstance()->GetDeltaTime();
	}
	float range = m_LagerInfo.fRotateTimer / ROTATE_TIMER;
	if (range > 1.0f) { range = 1.0f; }

	for (int i = 0; i < TYPE::TYPE_MAX; i++)
	{
		SPoliceInfo* pInfo = &m_aPoliceInfo[i];

		// ��~��Ԃ̎���������
		if (pInfo->pPolice == nullptr) { continue; }
		if (pInfo->pPolice->GetState() != CPolice::STATE::STATE_STOP) { continue; }

		// ���������߂�
		float diff = atan2f(m_Info.pos.x - pInfo->goalpos.x, m_Info.pos.z - pInfo->goalpos.z) - pInfo->goalrot.y;
		Adjust(diff);

		// ������ݒ�
		D3DXVECTOR3 rot = pInfo->goalrot;
		rot.y += diff * range;
		pInfo->pPolice->SetRotation(rot);
	}
}