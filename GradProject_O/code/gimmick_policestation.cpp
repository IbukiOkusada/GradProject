//==========================================================
//
// ���ΐ��M�~�b�N���� [gimmick_firehydrant.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "manager.h"
#include "gimmick_policestation.h"
#include "deltatime.h"
#include "objectX.h"
#include "camera.h"
#include "camera_manager.h"
#include "renderer.h"
#include "particle3D.h"
#include "player.h"
#include "player_manager.h"
#include "add_police.h"
#include "debugproc.h"
#include "road_manager.h"
#include "car_manager.h"

// �萔��`
namespace
{
	// �t�@�C����
	const char* FILENAME = "data\\MODEL\\map\\policestation.x";
	const float OUT_ENGINE = 0.6f;
	const float INTERVAL = 5.0f;
	const float SEARCH_DISTANCE = 3000.0f;
	const float SEARCH_RANGE = D3DX_PI * 0.1f;
}

Clist<CGimmickPoliceStation*> CGimmickPoliceStation::m_List = {};

//==========================================================
// �R���X�g���N�^
//==========================================================
CGimmickPoliceStation::CGimmickPoliceStation()
{
	// �l�̃N���A
	m_pObj = nullptr;
	m_Info = SInfo();
	m_pRoad = nullptr;
	m_List.Regist(this);
}

//==========================================================
// �f�X�g���N�^
//==========================================================
CGimmickPoliceStation::~CGimmickPoliceStation()
{

}

//==========================================================
// ����������
//==========================================================
HRESULT CGimmickPoliceStation::Init(void)
{
	m_pObj = CObjectX::Create(GetPos(), GetRot(), FILENAME);
	m_pObj->SetScale(VECTOR3_ONE);
	SetVtxMax(m_pObj->GetVtxMax());
	SetVtxMin(m_pObj->GetVtxMin());
	m_pObj->SetScale(GetScale());
	m_Info.fInterVal = INTERVAL;
	SetType(TYPE::TYPE_POLICESTATION);

	// �ł��߂������擾
	m_pRoad = CRoadManager::GetInstance()->GetNearRoad(GetPos());

	return S_OK;
}

//==========================================================
// �I������
//==========================================================
void CGimmickPoliceStation::Uninit(void)
{
	// �I�u�W�F�N�g�p��
	if (m_pObj != nullptr)
	{
		m_pObj->Uninit();
		m_pObj = nullptr;
	}

	m_pRoad = nullptr;
	m_List.Delete(this);

	CGimmick::Uninit();
}

//==========================================================
// �X�V����
//==========================================================
void CGimmickPoliceStation::Update(void)
{
	if (CManager::GetInstance()->GetMode() == CScene::MODE::MODE_TITLE) { return; }
	if (m_pObj == nullptr) { return; }

	// �C���^�[�o�����I����Ă��Ȃ�
	if (m_Info.fSpawnTime < m_Info.fInterVal) {
		m_Info.fSpawnTime += CDeltaTime::GetInstance()->GetDeltaTime();

		if (m_Info.fSpawnTime > m_Info.fInterVal)
		{
			m_Info.fSpawnTime = m_Info.fInterVal;
		}

		return;
	}

	if (CNetWork::GetInstance()->GetState() == CNetWork::STATE::STATE_SINGLE)
	{
		// �v���C���[���m�F
		CPlayer* pPlayer = CPlayerManager::GetInstance()->GetPlayer();
		if (pPlayer == nullptr) { return; }
		if (pPlayer->GetEngine() <= OUT_ENGINE) { return; }	// �Z�[�t

		// ��������
		D3DXVECTOR3 pos = GetPos() - pPlayer->GetPosition();
		float distance = D3DXVec3Length(&pos);
		CDebugProc::GetInstance()->Print("���� [ %f ]", distance);

		if (distance > SEARCH_DISTANCE) { return; }

		// ��������
		float rot = atan2f(pos.x, pos.z);
		Adjust(rot);
		float dest = rot - GetRot().y;
		Adjust(dest);
		dest = fabsf(dest);
		CDebugProc::GetInstance()->Print("���� [ %f ]", dest);

		// �͈͓��̂݌x�@����
		if (dest < -SEARCH_RANGE || dest > SEARCH_RANGE) { return; }
		m_Info.fSpawnTime = 0.0f;

		CAddPolice* pP = CAddPolice::Create(GetPos(), GetRot(), VECTOR3_ZERO, CCarManager::GetInstance()->GetMapList()->GetInCnt());

		if (pP != nullptr)
		{
			// �����̌x�@�̃^�C�v��ݒ�
			pP->SetTypeAI(CPoliceAI::TYPE_NONE);
			pP->SetType(CCar::TYPE::TYPE_ACTIVE);

			// �ړI�n�ݒ�
			pP->SetRoadTarget(CRoadManager::GetInstance()->GetNearRoad(GetPos()));

			// �ǐՏ�ԂɕύX
			pP->SetChase(true);
			pP->GetAi()->BeginChase(pPlayer);

			// �����̌x�@�͉������Ă΂Ȃ��悤�ɂ���
			pP->GetAi()->SetCall(true);
		}
	}
	else
	{
		for (int i = 0; i < NetWork::MAX_CONNECT; i++)
		{
			if (!CNetWork::GetInstance()->GetConnect(i)) { continue; }

			// �v���C���[���m�F
			CPlayer* pPlayer = CPlayerManager::GetInstance()->GetPlayer(i);
			if (pPlayer == nullptr) { return; }
			if (pPlayer->GetEngine() <= OUT_ENGINE) { return; }	// �Z�[�t

			// ��������
			D3DXVECTOR3 pos = GetPos() - pPlayer->GetPosition();
			float distance = D3DXVec3Length(&pos);
			CDebugProc::GetInstance()->Print("���� [ %f ]", distance);

			if (distance > SEARCH_DISTANCE) { return; }

			// ��������
			float rot = atan2f(pos.x, pos.z);
			Adjust(rot);
			float dest = rot - GetRot().y;
			Adjust(dest);
			dest = fabsf(dest);
			CDebugProc::GetInstance()->Print("���� [ %f ]", dest);

			// �͈͓��̂݌x�@����
			if (dest < -SEARCH_RANGE || dest > SEARCH_RANGE) { return; }
			m_Info.fSpawnTime = 0.0f;

			if (i == CNetWork::GetInstance()->GetIdx())
			{

				//CAddPolice* pP = CAddPolice::Create(GetPos(), GetRot(), VECTOR3_ZERO, CCarManager::GetInstance()->GetMapList()->GetInCnt());

				//if (pP != nullptr)
				//{
				//	// �����̌x�@�̃^�C�v��ݒ�
				//	pP->SetTypeAI(CPoliceAI::TYPE_NONE);
				//	pP->SetType(CCar::TYPE::TYPE_ACTIVE);

				//	// �ړI�n�ݒ�
				//	pP->SetRoadTarget(CRoadManager::GetInstance()->GetNearRoad(GetPos()));

				//	// �ǐՏ�ԂɕύX
				//	pP->SetChase(true);
				//	pP->GetAi()->BeginChase(pPlayer);

				//	// �����̌x�@�͉������Ă΂Ȃ��悤�ɂ���
				//	pP->GetAi()->SetCall(true);
				//}
			}
		}
	}
}

//==========================================================
// ����
//==========================================================
CGimmickPoliceStation* CGimmickPoliceStation::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale)
{
	CGimmickPoliceStation* pPoliceStation = nullptr;

	pPoliceStation = DEBUG_NEW CGimmickPoliceStation;

	if (pPoliceStation != nullptr)
	{
		pPoliceStation->SetPos(pos);
		pPoliceStation->SetRot(rot);
		pPoliceStation->SetScale(scale);

		// ����������
		pPoliceStation->Init();
	}

	return pPoliceStation;
}

//==========================================================
// �F�{���ύX
//==========================================================
void CGimmickPoliceStation::SetColMulti(const D3DXCOLOR& col)
{
	if (m_pObj == nullptr) { return; }

	m_pObj->SetColMulti(col);
}

//==========================================================
// ���W���f
//==========================================================
void CGimmickPoliceStation::SetObjPos(const D3DXVECTOR3& pos)
{
	if (m_pObj == nullptr) { return; }

	m_pObj->SetPosition(pos);
}

//==========================================================
// �������f
//==========================================================
void CGimmickPoliceStation::SetObjRot(const D3DXVECTOR3& rot)
{
	if (m_pObj == nullptr) { return; }

	m_pObj->SetRotation(rot);
}

//==========================================================
// �X�P�[�����f
//==========================================================
void CGimmickPoliceStation::SetObjScale(const D3DXVECTOR3& scale)
{
	if (m_pObj == nullptr) { return; }

	m_pObj->SetScale(scale);
}

//==========================================================
// �ł��߂��ꏊ�󂯎��
//==========================================================
CGimmickPoliceStation* CGimmickPoliceStation::GetNear(const D3DXVECTOR3& pos)
{
	float length = 1000000.0f;
	CGimmickPoliceStation* pStation = nullptr;
	// �������J��Ԃ�
	for (int i = 0; i < m_List.GetNum(); i++)
	{
		// �m�F
		CGimmickPoliceStation* pCheck = m_List.Get(i);
		D3DXVECTOR3 vec = pCheck->GetPos() - pos;
		float temp = D3DXVec3Length(&vec);

		// �������߂�
		if (temp <= length)
		{
			length = temp;
			pStation = pCheck;
		}
	}

	return pStation;
}