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
#include "inspection_manager.h"

// �������O��Ԃ��`
namespace
{
	const float POLICE_SETLENGTH = 700.0f;	// �x�@�ݒu����
	const float TIMER = 10.0f;				// �^�C�}�[
	const float START_SPEEDDEST = 40.0f;	// �X�^�[�g
	const float STOP_TIMER = 3.0f;			// �X�g�b�v
	const float ROTATE_TIMER = 1.0f;		// ��]
	const float LAGER_TIMER = 3.0f;			// ������o��܂ł̑��x
	const float LAGER_LENGTH = 700.0f;
}

//==========================================================
// �R���X�g���N�^
//==========================================================
CInspection::CInspection(int nId)
{
	// �l�̃N���A
	m_Info = SInfo();
	m_Info.nId = nId;
	m_pNearStation = nullptr;
	m_pRoad = nullptr;
	m_LagerInfo = SLagerInfo();

	for (int i = 0; i < TYPE::TYPE_MAX; i++)
	{
		m_aPoliceInfo[i] = SPoliceInfo();
	}

	// ���X�g�ɑ}��
	CInspectionManager::GetInstance()->ListIn(this);
}

//==========================================================
// �f�X�g���N�^
//==========================================================
CInspection::~CInspection()
{

}

//==========================================================
// ����������
//==========================================================
HRESULT CInspection::Init(void)
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
void CInspection::Uninit(void)
{
	// �x�@�p��
	for (int i = 0; i < TYPE::TYPE_MAX; i++)
	{
		if (m_aPoliceInfo[i].pPolice != nullptr)
		{
			m_aPoliceInfo[i].pPolice->SetState(CPolice::STATE::STATE_SEARCH);
			m_aPoliceInfo[i].pPolice = nullptr;
			m_aPoliceInfo[i] = SPoliceInfo();
		}
	}

	for (int i = 0; i < InstpectionData::NUM_EFFECT; i++)
	{
		SAFE_DELETE(m_LagerInfo.apEffect[i]);
	}

	// ���X�g����O��
	CInspectionManager::GetInstance()->ListOut(this);

	Release();
}

//==========================================================
// �X�V����
//==========================================================
void CInspection::Update(void)
{
	// �f�o�b�O�\��
	CDebugProc::GetInstance()->Print("����̍��W : [ %f, %f, %f] : ���� : [ %f, %f, %f] \n", 
		m_Info.pos.x, m_Info.pos.y, m_Info.pos.z, m_Info.rot.x, m_Info.rot.y, m_Info.rot.z);

	// �ړ�
	Away();

	// ������z�u
	LagerSet();

	// �Փ˔���
	Collision();
}

//==========================================================
// ����
//==========================================================
CInspection* CInspection::Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, CRoad* pRoad, int nId)
{
	CInspection* pInsp = nullptr;

	pInsp = DEBUG_NEW CInspection(nId);

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
void CInspection::Away()
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
void CInspection::LagerSet()
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
		// ����̂�
		if (m_LagerInfo.fTimer == 0.0f)
		{
			// �G�t�F�N�g�̐���
			for (int i = 0; i < InstpectionData::NUM_EFFECT; i++)
			{
				// �x�@���g���Ă�����
				if (m_aPoliceInfo[i].pPolice == nullptr) { continue; }

				m_LagerInfo.apEffect[i] = CEffekseer::GetInstance()->Create("data\\EFFEKSEER\\marker_laser.efkefc", 
					m_aPoliceInfo[i].goalpos, VECTOR3_ZERO, VECTOR3_ZERO, 70.0f, true, false);
			}
		}

		m_LagerInfo.fTimer += CDeltaTime::GetInstance()->GetDeltaTime();

		if (m_LagerInfo.fTimer >= LAGER_TIMER)
		{
			m_LagerInfo.fTimer = LAGER_TIMER;
		}
	}

	// ���������߂�
	m_LagerInfo.scale = m_LagerInfo.fTimer / LAGER_TIMER;

	for (int i = 0; i < TYPE::TYPE_MAX; i++)
	{
		SPoliceInfo* pInfo = &m_aPoliceInfo[i];

		D3DXVECTOR3 posdiff = m_Info.pos - pInfo->goalpos;
		D3DXVECTOR3 nor = posdiff;
		D3DXVec3Normalize(&nor, &nor);

		if (i >= InstpectionData::NUM_EFFECT) { continue; }
		if (m_LagerInfo.apEffect[i] == nullptr) { continue; }

		// ����
		float diff = atan2f(m_Info.pos.x - pInfo->goalpos.x, m_Info.pos.z - pInfo->goalpos.z);
		Adjust(diff);
		D3DXVECTOR3 rot = VECTOR3_ZERO;
		rot.y = diff;
		m_LagerInfo.apEffect[i]->m_rot = rot;

		// �X�P�[��
		m_LagerInfo.apEffect[i]->m_Scale.Z = m_LagerInfo.scale * LAGER_LENGTH;

		// �ʒu
		D3DXVECTOR3 pos = pInfo->goalpos + ((m_Info.pos - pInfo->goalpos) * m_LagerInfo.scale);
		m_LagerInfo.apEffect[i]->m_pos = pos;
	}
}

//==========================================================
// ���⎞�̌����ݒ�
//==========================================================
void CInspection::LagerSetRotation()
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
		float diff = atan2f(m_Info.pos.x - pInfo->goalpos.x, m_Info.pos.z - pInfo->goalpos.z);
		Adjust(diff);
		float dest = diff - pInfo->goalrot.y;
		Adjust(dest);

		// ������ݒ�
		D3DXVECTOR3 rot = pInfo->goalrot;
		rot.y += dest * range;
		pInfo->pPolice->SetRotation(rot);

		// ���W����������
		diff += D3DX_PI;
		Adjust(diff);
		D3DXVECTOR3 pos = pInfo->goalpos;
		pos.x += sinf(diff) * (100.0f * range);
		pos.z += cosf(diff) * (100.0f * range);
		pInfo->pPolice->SetPosition(pos);
	}
}

//==========================================================
// �o��
//==========================================================
void CInspection::Start()
{
	for (int i = 0; i < TYPE::TYPE_MAX; i++)
	{
		SPoliceInfo* pInfo = &m_aPoliceInfo[i];

		// �x�@��؂藣��
		if (pInfo->pPolice != nullptr)
		{
			std::vector<CRoad::SSearch*> navi = {};
			navi.clear();
			
			pInfo->pPolice->SetNavi(navi);
			pInfo->pPolice->SetState(CPolice::STATE::STATE_SEARCH);
			pInfo->pPolice = nullptr;
		}

		if (i < InstpectionData::NUM_EFFECT)
		{
			SAFE_DELETE(m_LagerInfo.apEffect[i]);
		}
	}

	// �I��
	Uninit();
}

//==========================================================
// ����
//==========================================================
void CInspection::Collision()
{
	CPlayer* pPlayer = CPlayerManager::GetInstance()->GetPlayer();
	if (pPlayer == nullptr) { return; }

	for (int i = 0; i < InstpectionData::NUM_EFFECT; i++)
	{
		if (m_LagerInfo.apEffect[i] == nullptr) { continue; }

		// ����
		D3DXVECTOR3 vtx1 = m_aPoliceInfo[i].goalpos;
		D3DXVECTOR3 vtx2 = m_aPoliceInfo[i].goalpos + (m_Info.pos - m_aPoliceInfo[i].goalpos) * m_LagerInfo.scale;

		// �v���C���[�̈ʒu
		D3DXVECTOR3 pos = pPlayer->GetPosition();
		D3DXVECTOR3 posOld = pPlayer->GetOldPosition();

		// �ʂ�߂���
		if (collision::LineCrossProduct(vtx1, vtx2, &pos, posOld))
		{
			// �ǐ�
			Start();
			return;
		}
		// �x�@�����ɏo���ς�
		else if (m_aPoliceInfo[i].pPolice->GetState() != CPolice::STATE::STATE_STOP &&
			m_aPoliceInfo[i].pPolice->GetState() != CPolice::STATE::STATE_NORMAL)
		{
			// �ǐ�
			Start();
			return;
		}
	}

	// ���Ԃ��v��
	for (int i = 0; i < TYPE::TYPE_MAX; i++)
	{ 
		if (m_aPoliceInfo[i].pPolice == nullptr) { return; }
		//if(m_aPoliceInfo[i].pPolice->GetState() != )
	}
}