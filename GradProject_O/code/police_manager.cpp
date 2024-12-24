//==========================================================
//
// �x�@�}�l�[�W���[ [police_manager.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "police_manager.h"
#include "police.h"
#include "player_manager.h"
#include "road.h"
#include "inspection.h"
#include "add_police.h"
#include "deltatime.h"
#include "debugproc.h"
#include "pred_route.h"
#include "network.h"
#include "inspection_manager.h"
#include "input_keyboard.h"
#include "car_manager.h"

// ���O���
namespace
{
	const int MAX_NEAR = 2;			// �߂��x�@�̊m�F��
	const float INTERVAL = 5.0f;	// �C���^�[�o��
	const int MAX_POLICE = (15);	// �x�@�̍ő�l
}

// �ÓI�����o�ϐ��錾
CPoliceManager* CPoliceManager::m_pInstance = nullptr;	// �C���X�^���X

//==========================================================
// �R���X�g���N�^
//==========================================================
CPoliceManager::CPoliceManager()
{
	// �l�̃N���A
	m_pList = nullptr;
	m_InspInfo = SInspInfo();
	m_nNum = 0;
	m_maplist.Clear();
	m_NearPoliceList.clear();
	m_Siren.clear();
}

//==========================================================
// �f�X�g���N�^
//==========================================================
CPoliceManager::~CPoliceManager()
{

}

//==========================================================
// ����������
//==========================================================
HRESULT CPoliceManager::Init(void)
{
	m_InspInfo.fTime = INTERVAL;

	m_Siren.clear();
	for (int i = 0; i < MAX_NEAR; i++)
	{
		m_Siren.push_back(CMasterSound::CObjectSound::Create("data\\SE\\siren.wav", -1));
		m_Siren[i]->SetVolume(0.0f);
		m_Siren[i]->Stop();
	}

	return S_OK;
}

//==========================================================
// �I������
//==========================================================
void CPoliceManager::Uninit(void)
{
	// �e�N���X�̏I������
	CListManager::Uninit();

	// ���X�g�p��
	ListRelease();

	m_maplist.Clear();
	m_NearPoliceList.clear();

	
	{
		for (int i = 0; i < m_Siren.size(); i++)
		{
			m_Siren[i]->SetVolume(0.0f);
			m_Siren[i]->Stop();
			SAFE_UNINIT_DELETE(m_Siren[i])
		}

		m_Siren.clear();
	}
	

	// �C���X�^���X�̔p��
	if (m_pInstance != nullptr) {	// �C���X�^���X���m�ۂ���Ă���
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

//==========================================================
// �X�V����
//==========================================================
void CPoliceManager::Update(void)
{
	m_InspInfo.fInterval += CDeltaTime::GetInstance()->GetDeltaTime();

	CDebugProc::GetInstance()->Print("�x�@�̑��� : [ %d ]\n", m_pList->GetNum());

	// �߂��x�@���擾����
	CPlayer* pPlayer = CPlayerManager::GetInstance()->GetPlayer();
	if (pPlayer == nullptr) { return; }
	std::vector<int> nearid = {};

	// �O����o����
	std::vector<CPolice*> old = m_NearPoliceList;
	m_NearPoliceList.clear();

	// �߂��x�@�擾
	while (nearid.size() < MAX_NEAR)
	{
		float minlength = 1000000.0f;
		int id = -1;
		for (int i = 0; i < GetList()->GetNum(); i++)
		{
			// ������O���ɂ���
			auto it = find(nearid.begin(), nearid.end(), i);
			if (it != nearid.end()) { continue; }

			CPolice* pPolice = GetList()->Get(i);
			D3DXVECTOR3 vec = pPlayer->GetPosition() - pPolice->GetPosition();
			float length = D3DXVec3Length(&vec);

			// ���߂�
			if (length < minlength)
			{
				id = i;
				minlength = length;
			}

		}

		// �܂����Ȃ�
		auto it = find(nearid.begin(), nearid.end(), id);
		if (it == nearid.end()&& id != -1)
		{
			nearid.push_back(id);
			it = find(nearid.begin(), nearid.end(), id);
			m_NearPoliceList.push_back(GetList()->Get(id));
		}
		else
		{
			break;
		}
	}

	// ����g���Ȃ��ꍇ����
	std::vector<int> activesoundid = {};
	for (int i = 0; i < old.size(); i++)
	{
		auto it = find(m_NearPoliceList.begin(), m_NearPoliceList.end(), old[i]);

		// �g�p����Ă���ꍇ�͎g�p���Ă���ID���擾����
		if (it != m_NearPoliceList.end())
		{
			if (old[i]->GetSound() != nullptr)
			{
				activesoundid.push_back(std::distance(m_Siren.begin(), find(m_Siren.begin(), m_Siren.end(), old[i]->GetSound())));
			}
		}
		else
		{
			old[i]->SetSound(nullptr);
		}
	}

	// ����ݒ�
	{
		int soundcnt = 0;
		for (int i = 0; i < m_NearPoliceList.size(); i++)
		{
			CPolice* pPolice = m_NearPoliceList[i];
			// �Ȃ����łɎg�p���Ă���
			if (pPolice->GetSound() != nullptr) { continue; }

			for (int j = 0; j < m_Siren.size(); j++)
			{
				// �g���Ă���
				if (activesoundid.end() != find(activesoundid.begin(), activesoundid.end(), j)) { continue; }

				// ���Ă͂߂�
				if (pPolice->GetChase())
				{
					pPolice->SetSound(GetSound(j));
				}
				else
				{
					pPolice->SetSound(m_Siren[j]);
				}
				
				activesoundid.push_back(j);
			}

		}
	}

	// �������
	nearid.clear();
}

//==========================================================
// �C���X�^���X���m��
//==========================================================
CPoliceManager* CPoliceManager::GetInstance(void)
{
	if (m_pInstance == nullptr) {	// �g���Ă��Ȃ�
		m_pInstance = DEBUG_NEW CPoliceManager;
		m_pInstance->Init();
	}

	return m_pInstance;
}

//==========================================================
// �C���X�^���X�������[�X
//==========================================================
void CPoliceManager::Release(void)
{
	if (m_pInstance != nullptr) {	// �C���X�^���X���m�ۂ���Ă���
		m_pInstance->Uninit();
	}
}

//==========================================================
// ���X�g�ɑ}��
//==========================================================
void CPoliceManager::ListIn(CPolice* pPolice)
{
	GetList()->Regist(pPolice);
	IdListIn(pPolice);
}

//==========================================================
// ���X�g����O��
//==========================================================
void CPoliceManager::ListOut(CPolice* pPolice)
{
	// ���X�g���玩�����g���폜����
	GetList()->Delete(pPolice);
	IdListOut(pPolice);
}

//==========================================================
// Id���X�g�ɑ}��
//==========================================================
void CPoliceManager::IdListIn(CPolice* pPolice)
{
	m_maplist.Regist(pPolice->GetId(), pPolice);
}

//==========================================================
// Id���X�g����O��
//==========================================================
void CPoliceManager::IdListOut(CPolice* pPolice)
{
	// ���X�g���玩�����g���폜����
	m_maplist.Delete(pPolice->GetId(), pPolice);
}

//==========================================================
// �U���q�b�g�m�F
//==========================================================
bool CPoliceManager::Hit(D3DXVECTOR3& pos, const float fRange, const float fHeight, const int nDamage)
{
	bool bUse = false;

	//�ʔ���
	for (int i = 0; i < GetList()->GetNum(); i++)
	{
		//CPolice* pPolice = GetList()->Get(i);
		//if (pPolice->HitCheck(pos, fRange, fHeight, nDamage)) {
			//bUse = true;
		//}
	}

	return bUse;
}

//==========================================================
// �S�����x����Ԃɂ���
//==========================================================
void CPoliceManager::Warning(CPolice* pPolice)
{
	if (pPolice->GetState() == CPolice::STATE::STATE_FADEOUT) { return; }

	for (int i = 0; i < GetList()->GetNum(); i++)
	{
		// �x�@�Ǝ�������v���Ă���ʖ�
		CPolice* pP = GetList()->Get(i);
		if (pP == pPolice) { continue; }

		// ���ݒǐՒ����ʖ�
		CPolice::STATE outstate = CPolice::STATE::STATE_CHASE;
		if (pP->GetState() == outstate) { continue; }

		// ��~�����ʖ�
		outstate = CPolice::STATE::STATE_STOP;
		if (pP->GetState() == outstate) { continue; }

		// �x����Ԃɂ���
		pP->SetState(CPolice::STATE::STATE_SEARCH);
	}

	if (m_InspInfo.fInterval >= m_InspInfo.fTime && pPolice->IsActive())
	{
		m_InspInfo.fInterval = 0.0f;
		// �����z�u����
		SetInspection();
	}
}

//==========================================================
// �����z�u����
//==========================================================
void CPoliceManager::SetInspection()
{
	CPlayer* pPlayer = CPlayerManager::GetInstance()->GetPlayer();
	if (pPlayer == nullptr) { return; }

	// �v���C���[�̗\�����擾����
	CPredRoute* pRoute = pPlayer->GetPredRoute();
	if (pRoute == nullptr) { return; }

	// �ڕW�n�_�ƂЂƂO
	CRoad* pRoad = pRoute->GetPredRoad();
	CRoad* pPrev = pRoute->GetPredPrevRoad();

	// ����������Ȃ�����
	if (pRoad == nullptr) { return; }

	// �������擾����
	D3DXVECTOR3 rot = VECTOR3_ZERO;
	float targetrot = atan2f(pPrev->GetPosition().x - pRoad->GetPosition().x,
		pPrev->GetPosition().z - pRoad->GetPosition().z);
	rot.y = targetrot + D3DX_PI * 0.5f;
	Adjust(&rot.y);

	// �ݒu
	D3DXVECTOR3 pos = pRoad->GetPosition();
	pos.x += sinf(targetrot) * pRoad->GetInfo()->size.x;
	pos.z += cosf(targetrot) * pRoad->GetInfo()->size.y;

	int startid = CCarManager::GetInstance()->GetMapList()->GetInCnt();

	// ���␶��
	CInspectionManager::GetInstance()->SetNextInspection(pos, rot, pRoad, CInspectionManager::GetInstance()->GetCreateCnt(), startid);

	// ���⑗�M
	auto net = CNetWork::GetInstance();
	net->SendSetInspection(CInspectionManager::GetInstance()->GetCreateCnt(), pos, rot, pRoad->GetIdx(), startid);
}