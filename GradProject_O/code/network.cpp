//===============================================
//
// �ʐM�����̊Ǘ����� [network.cpp]
// Author : Ibuki Okusada
//
//===============================================
#include "network.h"
#include "tcp_client.h"

// �f�[�^�󂯓n���p
#include "player_manager.h"
#include "player.h"
#include "goal.h"
#include "gimmick.h"
#include "debugproc.h"
#include "goal_manager.h"
#include "manager.h"
#include "fade.h"
#include "inspection_manager.h"
#include "inspection.h"
#include "road_manager.h"
#include "road.h"
#include "car_manager.h"
#include "car.h"
#include "police_manager.h"
#include "police.h"
#include "add_police.h"
#include "police_AI.h"
#include "manager.h"

//===============================================
// ���O���
//===============================================
namespace
{
	const int DEF_PORT = (22333);			// �f�t�H���g�̃|�[�g�ԍ�
	const std::string ADDRESS_FILENAME =	// �A�h���X�̃t�@�C����
		"data\\TXT\\address.txt";
}

//===============================================
// �֐��|�C���^
//===============================================
// ��ԊǗ�
CNetWork::RECV_FUNC CNetWork::m_RecvFunc[] =
{
	&CNetWork::RecvNone,	// �����Ȃ�
	&CNetWork::RecvJoin,	// �ڑ�����
	&CNetWork::RecvGetId,	// ID�擾
	&CNetWork::RecvDelete,	// �폜
	&CNetWork::RecvPlPos,	// �v���C���[���W
	&CNetWork::RecvPlDamage,	// �v���C���[�_���[�W
	&CNetWork::RecvPlGoal,	// �v���C���[�S�[��
	&CNetWork::RecvGmHit,	// �M�~�b�N�q�b�g
	&CNetWork::RecvNextGoal,	// ���̃S�[���ݒu
	&CNetWork::RecvGameStartOk,	// �Q�[���J�n��������
	&CNetWork::RecvGameStart,	// �Q�[���J�n
	&CNetWork::RecvTutorialOk,	// �`���[�g���A���I���\
	&CNetWork::RecvTutorialNo,	// �`���[�g���A���I���\
	&CNetWork::RecvTutorialEnd,	// �`���[�g���A���I��
	&CNetWork::RecvSetInspection,	// �`���[�g���A���I���\
	&CNetWork::RecvEndInspection,	// �`���[�g���A���I��
	&CNetWork::RecvCarPos,		// �ԍ��W
	&CNetWork::RecvPdPos,		// �x�@���W
	&CNetWork::RecvAddPdPos,	// �ǉ����W
	&CNetWork::RecvPdChase,		// �x�@���W
	&CNetWork::RecvAddPdChase,	// �ǉ����W
	&CNetWork::RecvPdChaseEnd,	// �x�@���W
	&CNetWork::RecvAddPdChaseEnd,	// �ǉ����W
};

// �ÓI�����o�ϐ�
CNetWork* CNetWork::m_pInstance = nullptr;

//===============================================
// �R���X�g���N�^
//===============================================
CNetWork::CNetWork()
{
	// �l�̃N���A
	m_pClient = nullptr;
	m_state = STATE::STATE_SINGLE;
	m_nMyIdx = -1;
	m_bEnd = true;
	m_nSledCnt = 0;

	for (int i = 0; i < NetWork::MAX_CONNECT; i++)
	{
		m_aConnect[i] = false;
	}
}

//===============================================
// �f�X�g���N�^
//===============================================
CNetWork::~CNetWork()
{

}

//===============================================
// ����
//===============================================
CNetWork* CNetWork::Create()
{
	if (m_pInstance == nullptr)
	{
		m_pInstance = DEBUG_NEW CNetWork;
		m_pInstance->Init();
	}

	return m_pInstance;
}


//===============================================
// �C���X�^���X���擾
//===============================================
CNetWork* CNetWork::GetInstance()
{
	return m_pInstance;
}


//===============================================
// ���
//===============================================
void CNetWork::Release()
{
	if (m_pInstance != nullptr)
	{
		m_pInstance->Uninit();
		m_pInstance = nullptr;
	}
}

//===============================================
// ������
//===============================================
HRESULT CNetWork::Init()
{
	// �N���C�A���g���쐬
	m_pClient = DEBUG_NEW CClient;
	
	m_nMyIdx = -1;
	m_state = STATE_SINGLE;

	return S_OK;
}


//===============================================
// �I��
//===============================================
void CNetWork::Uninit()
{
	m_bEnd = true;

	// �ؒf
	DisConnect();

	// �N���C�A���g�̏I��
	if (m_pClient != nullptr)
	{
		m_pClient->Uninit();
		delete m_pClient;
		m_pClient = nullptr;
	}

	for (int i = 0; i < NetWork::MAX_CONNECT; i++)
	{
		m_aConnect[i] = false;
	}

	m_nMyIdx = -1;

	delete this;
}

//===============================================
// �I��
//===============================================
void CNetWork::Update()
{
	if (m_SendTime.IsOK())
	{
		m_SendTime.Start();

		if (m_pClient == nullptr) { return; }
		m_pClient->Send();
	}

	if (m_pClient == nullptr) { return; }
	CDebugProc::GetInstance()->Print("���݂̃o�C�g�� [ %d ]\n", m_pClient->GetByte());
}

//===============================================
// �A�h���X�t�@�C���ǂݍ���
//===============================================
void CNetWork::LoadAddressFile()
{
	// �t�@�C�����J��
	std::ifstream File(ADDRESS_FILENAME);
	if (!File.is_open()) {
		return;
	}

	// �R�����g�p
	std::string hoge;

	// �f�[�^�ǂݍ���
	std::string line;

	while (std::getline(File, line))
	{
		// �R�����g�̓X�L�b�v
		if (line.empty() ||
			line[0] == '#')
		{
			continue;
		}

		// �X�g���[���쐬
		std::istringstream lineStream(line);

		if (line.find("ADDLESS") != std::string::npos)
		{// TEXT_FILENAME�Ń��f�����ǂݍ���

			// �X�g���[���쐬
			std::istringstream lineStr(line);

			// ���n��
			lineStr >>
				hoge >>
				hoge >>		// ��
				m_Address;	// �A�h���X

			continue;
		}

		if (line.find("END_SCRIPT") != std::string::npos)
		{
			break;
		}
	}

	// �t�@�C�������
	File.close();
}

//===============================================
// �Đڑ�
//===============================================
bool CNetWork::ReConnect()
{
	// ���ɐڑ��ς�
	if (m_state == STATE::STATE_ONLINE) { return true; }

	m_nMyIdx = -1;
	m_state = STATE_MAX;

	// �A�h���X���Ď擾
	LoadAddressFile();

	// ����
	if (m_pClient == nullptr)
	{
		m_pClient = DEBUG_NEW CClient;
	}

	// �ڑ����s
	if (!m_pClient->Init(m_Address.c_str(), DEF_PORT)) 
	{
		m_pClient->Uninit();
		delete m_pClient;
		m_pClient = nullptr;
		m_state = STATE_SINGLE;
		m_nMyIdx = -1;
		m_bEnd = true;
		return false; 
	}

	m_bEnd = false;

	// �}���`�X���b�h
	std::thread th(&CNetWork::Online, this);
	th.detach();

	SendGetId();

	m_state = STATE_ONLINE;
	return true;
}

//===============================================
// �ؒf
//===============================================
bool CNetWork::DisConnect()
{
	m_bEnd = true;

	// �X���b�h���Ȃ��Ȃ�����
	while (1)
	{
		OnlineEnd();
		if (m_nSledCnt <= 0)
		{
			break;
		}
	}

	// �I������
	if (m_pClient != nullptr)
	{
		m_pClient->Uninit();
		delete m_pClient;
		m_pClient = nullptr;
	}

	for (int i = 0; i < NetWork::MAX_CONNECT; i++)
	{
		m_aConnect[i] = false;
	}

	m_nMyIdx = -1;
	m_state = STATE::STATE_SINGLE;

	return true;
}

//===================================================
// �I�����C���ʐM
//===================================================
void CNetWork::Online(void)
{
	m_nSledCnt = 1;

	while (1)
	{
		if (!GetActive())
		{
			break;
		}

		char* pData = DEBUG_NEW char[NetWork::MAX_SEND_DATA];	// ��M�p

		// ��M
		int* pRecvByte = DEBUG_NEW int;
		*pRecvByte = m_pClient->Recv(pData, NetWork::MAX_SEND_DATA);

		if (*pRecvByte > 0)
		{
			// �}���`�X���b�h
#if 0
			std::thread th(&CNetWork::ByteCheck, this, pData, pRecvByte);
			th.detach();
#else
			ByteCheck(pData, pRecvByte);
#endif
		}
		else
		{
			DeleteData(pData, pRecvByte);
			break;
		}
	}

	m_nSledCnt--;
}

//===================================================
// ��M�m�F
//===================================================
void CNetWork::ByteCheck(char* pRecvData, int* pRecvByte)
{
	m_mutex.lock();
	m_nSledCnt++;
	D3DXVECTOR3 pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXCOLOR col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	CFade* pFade = CManager::GetInstance()->GetFade();

	if (*pRecvByte <= 0)
	{
		// �f�[�^�폜
		DeleteData(pRecvData, pRecvByte);

		m_nSledCnt--;
		m_mutex.unlock();
		return;
	}

	int nByte = 0;	// �o�C�g��

	// �I�[�����܂Ŋm�F����
	while (1)
	{
		int id = -1;		// ID
		int command = NetWork::COMMAND_NONE;	// �R�}���h�ԍ�

		// ID�擾
		memcpy(&id, &pRecvData[nByte], sizeof(int));
		nByte += sizeof(int);

		// �R�}���h�擾
		memcpy(&command, &pRecvData[nByte], sizeof(int));
		nByte += sizeof(int);

		// �R�}���h���I�[�o�[����
		if (command >= NetWork::COMMAND_MAX || command < NetWork::COMMAND_NONE)
		{
			// �f�[�^�폜
			DeleteData(pRecvData, pRecvByte);

			m_nSledCnt--;
			m_mutex.unlock();
			return;
		}

		// �R�}���h�����s
		(this->*(m_RecvFunc[command]))(&nByte, id, &pRecvData[nByte]);

		// �S�Č���
		if (nByte >= *pRecvByte)
		{
			break;
		}
	}

	// �f�[�^�폜
	DeleteData(pRecvData, pRecvByte);

	m_nSledCnt--;
	m_mutex.unlock();
}

//===================================================
// �f�[�^�폜
//===================================================
void CNetWork::DeleteData(char* pRecvData, int* pRecvByte)
{
	if (pRecvData != nullptr)
	{
		delete[] pRecvData;
	}

	if (pRecvByte != nullptr)
	{
		delete pRecvByte;
	}
}

//===================================================
// �I�����M
//===================================================
void CNetWork::OnlineEnd(void)
{
	if (m_pClient != nullptr)
	{
		std::string senddata;
		senddata.resize(sizeof(int) + 1);
		int nProt = NetWork::COMMAND_DELETE;

		// protocol��}��
		memcpy(&senddata[0], &nProt, sizeof(int));

		// ���M
		m_pClient->SetData(senddata.c_str(), sizeof(int));
		m_pClient->Send();
	}
}

//===================================================
// �g�p�\�m�F
//===================================================
bool CNetWork::GetActive()
{
	if (m_bEnd || m_pClient == nullptr)
	{
		return false;
	}

	return true;
}

//===================================================
// �����v���C���[
//===================================================
CPlayer* CNetWork::CreatePlayer(int nId)
{
	m_aPlayerMutex[nId].lock();

	// �v���C���[����
	CPlayer* pPlayer = CPlayer::Create(VECTOR3_ZERO, VECTOR3_ZERO, VECTOR3_ZERO, nId);

	// ��M�p�v���C���[��
	pPlayer->SetType(CPlayer::TYPE::TYPE_RECV);

	m_aPlayerMutex[nId].unlock();

	return pPlayer;
}

//===================================================
// �����Ȃ�
//===================================================
void CNetWork::RecvNone(int* pByte, const int nId, const char* pRecvData)
{
	
}

//===================================================
// �ڑ�����
//===================================================
void CNetWork::RecvJoin(int* pByte, const int nId, const char* pRecvData)
{
	if (nId < 0 || nId >= NetWork::MAX_CONNECT) { return; }
	if (m_nMyIdx < 0) { return; }

	// �ڑ����ꂽ���Ƃɂ���
	m_aConnect[nId] = true;
}

//===================================================
// ID���擾����
//===================================================
void CNetWork::RecvGetId(int* pByte, const int nId, const char* pRecvData)
{
	if (nId < 0 || nId >= NetWork::MAX_CONNECT) { return; }

	// �ڑ����ꂽ���Ƃɂ���
	m_aConnect[nId] = true;
	m_nMyIdx = nId;
}

//===================================================
// �폜
//===================================================
void CNetWork::RecvDelete(int* pByte, const int nId, const char* pRecvData)
{
	if (nId < 0 || nId >= NetWork::MAX_CONNECT) { return; }

	m_aConnect[nId] = false;
}

//===================================================
// �v���C���[�̍��W���擾
//===================================================
void CNetWork::RecvPlPos(int* pByte, const int nId, const char* pRecvData)
{
	if (m_pClient == nullptr) { return; }
	// ���W�ɕϊ�
	D3DXVECTOR3 pos = VECTOR3_ZERO;
	memcpy(&pos, pRecvData, sizeof(D3DXVECTOR3));

	// �m�F�o�C�g�������Z
	*pByte += sizeof(D3DXVECTOR3);

	// ���W�ɕϊ�
	D3DXVECTOR3 rot = VECTOR3_ZERO;
	memcpy(&rot, &pRecvData[sizeof(D3DXVECTOR3)], sizeof(D3DXVECTOR3));

	// �m�F�o�C�g�������Z
	*pByte += sizeof(D3DXVECTOR3);

	// �v���C���[�̑��݊m�F
	if (nId < 0 || nId >= NetWork::MAX_CONNECT || nId == m_nMyIdx) { return; }
	CPlayer* pPlayer = CPlayerManager::GetInstance()->GetPlayer(nId);

	// �������Ă��Ȃ��ꍇ
	if (pPlayer == nullptr) { 
		RecvJoin(pByte, nId, pRecvData);
		return; 
	}

	// ���W�ݒ�
	pPlayer->SetRecvPosition(pos);
	pPlayer->SetRecvRotation(rot);
}

//===================================================
// �v���C���[�̃_���[�W���擾
//===================================================
void CNetWork::RecvPlDamage(int* pByte, const int nId, const char* pRecvData)
{
	// �̗͂ɕϊ�
	float life = 0.0f;
	memcpy(&life, pRecvData, sizeof(float));

	// �m�F�o�C�g�������Z
	*pByte += sizeof(float);

	// �v���C���[�̑��݊m�F
	if (nId < 0 || nId >= NetWork::MAX_CONNECT || nId == m_nMyIdx) { return; }
	CPlayer* pPlayer = CPlayerManager::GetInstance()->GetPlayer(nId);

	// �������Ă��Ȃ��ꍇ
	if (pPlayer == nullptr) {
		RecvJoin(pByte, nId, pRecvData);
		return;
	}

	// ���W�ݒ�
	pPlayer->Damage(life - pPlayer->GetLife());
}

//===================================================
// �v���C���[�̃S�[�����擾
//===================================================
void CNetWork::RecvPlGoal(int* pByte, const int nId, const char* pRecvData)
{
	// �S�[����Id�𓾂�
	int goalid = -1;
	memcpy(&goalid, pRecvData, sizeof(int));
	*pByte += 4;

	CGoal* pGoal = CGoalManager::GetInstance()->GetGoal(goalid);

	if (pGoal == nullptr) {
		return;
	}

	pGoal->RecvEnd(nId);
}

//===================================================
// �M�~�b�N�̃q�b�g���擾
//===================================================
void CNetWork::RecvGmHit(int* pByte, const int nId, const char* pRecvData)
{
	int byte = 0;

	// �M�~�b�N��Id�𓾂�
	int gmid = -1;
	memcpy(&gmid, &pRecvData[byte], sizeof(int));
	*pByte += sizeof(int);
	byte += sizeof(int);

	// �Փˈʒu���擾
	D3DXVECTOR3 pos = VECTOR3_ZERO;
	memcpy(&pos, &pRecvData[byte], sizeof(D3DXVECTOR3));
	*pByte += sizeof(D3DXVECTOR3);
	byte += sizeof(D3DXVECTOR3);

	// ���x���擾
	float speed = 0.0f;
	memcpy(&speed, &pRecvData[byte], sizeof(float));
	*pByte += sizeof(float);
	byte += sizeof(float);

	CGimmick* pGimmick = CGimmick::GetList()->Get(gmid);

	if (pGimmick == nullptr) {
		return;
	}

	// �Փ˂�����Ԃɂ���
	pGimmick->Hit(pos, speed);
}

//===================================================
// ���̃S�[���𐶐�
//===================================================
void CNetWork::RecvNextGoal(int* pByte, const int nId, const char* pRecvData)
{
	int byte = 0;

	// ���ɐ�������S�[����ID�𓾂�
	int goalid = -1;
	memcpy(&goalid, &pRecvData[byte], sizeof(int));
	*pByte += sizeof(int);

	CGoalManager::GetInstance()->SetNetId(goalid);
}

//===================================================
// �J�n�\�Ȃ̂���M
//===================================================
void CNetWork::RecvGameStartOk(int* pByte, const int nId, const char* pRecvData)
{

}

//===================================================
// �Q�[���J�n�\
//===================================================
void CNetWork::RecvGameStart(int* pByte, const int nId, const char* pRecvData)
{
	// �v���C���[�̑��݊m�F
	CPlayer* pPlayer = CPlayerManager::GetInstance()->GetPlayer();

	// �������Ă��Ȃ��ꍇ
	if (pPlayer == nullptr) {
		return;
	}

	// ���W�ݒ�
	pPlayer->SetType(CPlayer::TYPE::TYPE_GAMESTARTOK);
}

//===================================================
// �`���[�g���A��Ok
//===================================================
void CNetWork::RecvTutorialOk(int* pByte, const int nId, const char* pRecvData)
{
	if (nId < 0 || nId >= NetWork::MAX_CONNECT) { return; }

	CScene* pScene = CManager::GetInstance()->GetScene();

	if (pScene == nullptr) { return; }

	// ID�擾
	pScene->SetID(nId);

	// ���������t���O�𗧂Ă�
	pScene->ChangeFlag(true);

	pScene->ReadyUp(nId);
}

//===================================================
// �`���[�g���A��Ok�L�����Z��
//===================================================
void CNetWork::RecvTutorialNo(int* pByte, const int nId, const char* pRecvData)
{
	if (nId < 0 || nId >= NetWork::MAX_CONNECT) { return; }

	CScene* pScene = CManager::GetInstance()->GetScene();

	if (pScene == nullptr) { return; }

	// ID�擾
	pScene->SetID(nId);

	// ���������t���O��܂�
	pScene->ChangeFlag(false);

	pScene->ReadyUp(nId);
}

//===================================================
// �`���[�g���A���I��
//===================================================
void CNetWork::RecvTutorialEnd(int* pByte, const int nId, const char* pRecvData)
{
	//// ���̉�ʂɑJ��
	//if (CManager::GetInstance()->GetMode() == CScene::MODE::MODE_ENTRY)
	//{
	//	CManager::GetInstance()->GetFade()->Set(CScene::MODE::MODE_GAME);
	//}

	CScene* pScene = CManager::GetInstance()->GetScene();

	if (pScene == nullptr) { return; }

	pScene->EndTutorial();
}

//===================================================
// ����J�n
//===================================================
void CNetWork::RecvSetInspection(int* pByte, const int nId, const char* pRecvData)
{
	int byte = 0;

	// �����ID�𓾂�
	int inspid = -1;
	memcpy(&inspid, &pRecvData[byte], sizeof(int));
	*pByte += sizeof(int);
	byte += sizeof(int);

	// �ʒu���擾
	D3DXVECTOR3 pos = VECTOR3_ZERO;
	memcpy(&pos, &pRecvData[byte], sizeof(D3DXVECTOR3));
	*pByte += sizeof(D3DXVECTOR3);
	byte += sizeof(D3DXVECTOR3);

	// �������擾
	D3DXVECTOR3 rot = VECTOR3_ZERO;
	memcpy(&rot, &pRecvData[byte], sizeof(D3DXVECTOR3));
	*pByte += sizeof(D3DXVECTOR3);
	byte += sizeof(D3DXVECTOR3);

	// �אڂ��铹��ID�擾
	int roadid = -1;
	memcpy(&roadid, &pRecvData[byte], sizeof(int));
	*pByte += sizeof(int);
	byte += sizeof(int);

	// �x�@�̊J�nID�擾
	int startpdid = -1;
	memcpy(&startpdid, &pRecvData[byte], sizeof(int));
	*pByte += sizeof(int);
	byte += sizeof(int);

	CInspection* pInsp = CInspectionManager::GetInstance()->Get(inspid);

	if (pInsp != nullptr) { return; }

	CRoad* pRoad = CRoadManager::GetInstance()->GetList()->Get(roadid);

	if (pRoad == nullptr) {
		return;
	}

	// ���␶��
	CInspectionManager::GetInstance()->SetNextInspection(pos, rot, pRoad, inspid, startpdid);
}

//===================================================
// ����I��
//===================================================
void CNetWork::RecvEndInspection(int* pByte, const int nId, const char* pRecvData)
{
	int byte = 0;

	// �����ID�𓾂�
	int inspid = -1;
	memcpy(&inspid, &pRecvData[byte], sizeof(int));
	*pByte += sizeof(int);
	byte += sizeof(int);

	CInspection* pInsp = CInspectionManager::GetInstance()->Get(inspid);

	if (pInsp == nullptr) { return; }

	// ������
	pInsp->Start();
}

//===================================================
// �Ԃ̍��W
//===================================================
void CNetWork::RecvCarPos(int* pByte, const int nId, const char* pRecvData)
{
	int byte = 0;

	// �����ID�𓾂�
	int carid = -1;
	memcpy(&carid, &pRecvData[byte], sizeof(int));
	*pByte += sizeof(int);
	byte += sizeof(int);

	// �ʒu���擾
	D3DXVECTOR3 pos = VECTOR3_ZERO;
	memcpy(&pos, &pRecvData[byte], sizeof(D3DXVECTOR3));
	*pByte += sizeof(D3DXVECTOR3);
	byte += sizeof(D3DXVECTOR3);

	// �������擾
	D3DXVECTOR3 rot = VECTOR3_ZERO;
	memcpy(&rot, &pRecvData[byte], sizeof(D3DXVECTOR3));
	*pByte += sizeof(D3DXVECTOR3);
	byte += sizeof(D3DXVECTOR3);

	CCar* pCar = CCarManager::GetInstance()->GetMapList()->Get(carid);

	if (pCar == nullptr)
	{
		pCar = CCar::Create(pos, rot, VECTOR3_ZERO, carid);
		pCar->SetType(CCar::TYPE::TYPE_RECV);
	}

	if (pCar->GetType() == CCar::TYPE::TYPE_ACTIVE) { return; }

	pCar->SetRecvPosition(pos);
	pCar->SetRecvRotation(rot);
}

//===================================================
// �x�@�̍��W
//===================================================
void CNetWork::RecvPdPos(int* pByte, const int nId, const char* pRecvData)
{
	int byte = 0;

	// �����ID�𓾂�
	int carid = -1;
	memcpy(&carid, &pRecvData[byte], sizeof(int));
	*pByte += sizeof(int);
	byte += sizeof(int);

	// �ʒu���擾
	D3DXVECTOR3 pos = VECTOR3_ZERO;
	memcpy(&pos, &pRecvData[byte], sizeof(D3DXVECTOR3));
	*pByte += sizeof(D3DXVECTOR3);
	byte += sizeof(D3DXVECTOR3);

	// �������擾
	D3DXVECTOR3 rot = VECTOR3_ZERO;
	memcpy(&rot, &pRecvData[byte], sizeof(D3DXVECTOR3));
	*pByte += sizeof(D3DXVECTOR3);
	byte += sizeof(D3DXVECTOR3);

	CCar* pCar = CCarManager::GetInstance()->GetMapList()->Get(carid);

	if (pCar == nullptr)
	{
		pCar = CPolice::Create(pos, rot, VECTOR3_ZERO, carid);
		pCar->SetType(CCar::TYPE::TYPE_RECV);
	}

	if (pCar->GetType() == CCar::TYPE::TYPE_ACTIVE) { return; }

	pCar->SetRecvPosition(pos);
	pCar->SetRecvRotation(rot);
}

//===================================================
// �ǉ��x�@�̍��W
//===================================================
void CNetWork::RecvAddPdPos(int* pByte, const int nId, const char* pRecvData)
{
	int byte = 0;

	// �����ID�𓾂�
	int carid = -1;
	memcpy(&carid, &pRecvData[byte], sizeof(int));
	*pByte += sizeof(int);
	byte += sizeof(int);

	// �ʒu���擾
	D3DXVECTOR3 pos = VECTOR3_ZERO;
	memcpy(&pos, &pRecvData[byte], sizeof(D3DXVECTOR3));
	*pByte += sizeof(D3DXVECTOR3);
	byte += sizeof(D3DXVECTOR3);

	// �������擾
	D3DXVECTOR3 rot = VECTOR3_ZERO;
	memcpy(&rot, &pRecvData[byte], sizeof(D3DXVECTOR3));
	*pByte += sizeof(D3DXVECTOR3);
	byte += sizeof(D3DXVECTOR3);

	CCar* pCar = CCarManager::GetInstance()->GetMapList()->Get(carid);

	if (pCar == nullptr)
	{
		CAddPolice* pPolice = CAddPolice::Create(pos, rot, VECTOR3_ZERO, carid);
		pPolice->SetType(CCar::TYPE::TYPE_RECV);

		// �����̌x�@�͉������Ă΂Ȃ��悤�ɂ���
		pPolice->GetAi()->SetCall(true);

		pCar = pPolice;
	}

	if (pCar->GetType() == CCar::TYPE::TYPE_ACTIVE) { return; }

	pCar->SetRecvPosition(pos);
	pCar->SetRecvRotation(rot);
}

//===================================================
// �x�@�̒ǐՊJ�n
//===================================================
void CNetWork::RecvPdChase(int* pByte, const int nId, const char* pRecvData)
{
	int byte = 0;

	// �Ԃ�ID�𓾂�
	int carid = -1;
	memcpy(&carid, &pRecvData[byte], sizeof(int));
	*pByte += sizeof(int);
	byte += sizeof(int);

	// �v���C���[ID���擾
	int plyid = -1;
	memcpy(&plyid, &pRecvData[byte], sizeof(int));
	*pByte += sizeof(int);
	byte += sizeof(int);

	CPolice* pCar = CPoliceManager::GetInstance()->GetMapList()->Get(carid);
	CPlayer* pPlayer = CPlayerManager::GetInstance()->GetPlayer(plyid);

	// �v���C���[�����Ȃ�
	if (pPlayer == nullptr)
	{
		RecvJoin(pByte, nId, pRecvData);
		return;
	}

	// �Ԃ����݂��Ă��Ȃ�
	if (pCar == nullptr)
	{
		pCar = CPolice::Create(VECTOR3_ZERO, VECTOR3_ZERO, VECTOR3_ZERO, carid);
		pCar->SetType(CCar::TYPE::TYPE_RECV);
	}

	pCar->SetChaseNext(CPolice::CHASE::CHASE_BEGIN);

	// �������g
	if (plyid == m_nMyIdx)
	{
		pCar->SetTypeNext(CCar::TYPE::TYPE_ACTIVE);
		pCar->SetNextPlayer(pPlayer);
	}
	// ����ȊO
	else
	{
		pCar->SetTypeNext(CCar::TYPE::TYPE_RECV);
		pCar->SetNextPlayer(pPlayer);
	}
}

//===================================================
// �ǉ��x�@�̒ǐՊJ�n
//===================================================
void CNetWork::RecvAddPdChase(int* pByte, const int nId, const char* pRecvData)
{
	int byte = 0;

	// �Ԃ�ID�𓾂�
	int carid = -1;
	memcpy(&carid, &pRecvData[byte], sizeof(int));
	*pByte += sizeof(int);
	byte += sizeof(int);

	// �v���C���[ID���擾
	int plyid = -1;
	memcpy(&plyid, &pRecvData[byte], sizeof(int));
	*pByte += sizeof(int);
	byte += sizeof(int);

	CPolice* pCar = CPoliceManager::GetInstance()->GetMapList()->Get(carid);
	CPlayer* pPlayer = CPlayerManager::GetInstance()->GetPlayer(plyid);

	// �v���C���[�����Ȃ�
	if (pPlayer == nullptr)
	{
		RecvJoin(pByte, nId, pRecvData);
		return;
	}

	// �Ԃ����݂��Ă��Ȃ�
	if (pCar == nullptr)
	{
		CAddPolice* pPolice = CAddPolice::Create(VECTOR3_ZERO, VECTOR3_ZERO, VECTOR3_ZERO, carid);
		pPolice->SetType(CCar::TYPE::TYPE_RECV);

		// �����̌x�@�͉������Ă΂Ȃ��悤�ɂ���
		pPolice->GetAi()->SetCall(true);

		pCar = pPolice;
	}

	// �ǐՏ�Ԃɂ���
	pCar->SetChaseNext(CPolice::CHASE::CHASE_BEGIN);

	// �������g
	if (plyid == m_nMyIdx)
	{
		pCar->SetTypeNext(CCar::TYPE::TYPE_ACTIVE);
		pCar->SetNextPlayer(pPlayer);
	}
	// ����ȊO
	else
	{
		pCar->SetTypeNext(CCar::TYPE::TYPE_RECV);
		pCar->SetNextPlayer(pPlayer);
	}
}

//===================================================
// �x�@�̒ǐՏI��
//===================================================
void CNetWork::RecvPdChaseEnd(int* pByte, const int nId, const char* pRecvData)
{
	int byte = 0;

	// �Ԃ�ID�𓾂�
	int carid = -1;
	memcpy(&carid, &pRecvData[byte], sizeof(int));
	*pByte += sizeof(int);
	byte += sizeof(int);

	CPolice* pCar = CPoliceManager::GetInstance()->GetMapList()->Get(carid);

	// �Ԃ����݂��Ă��Ȃ�
	if (pCar == nullptr)
	{
		pCar = CPolice::Create(VECTOR3_ZERO, VECTOR3_ZERO, VECTOR3_ZERO, carid);
		pCar->SetType(CCar::TYPE::TYPE_RECV);
	}

	// �ǐՏI����Ԃɂ���
	pCar->SetChaseNext(CPolice::CHASE::CHASE_END);
}

//===================================================
// �ǉ��x�@�̒ǐՏI��
//===================================================
void CNetWork::RecvAddPdChaseEnd(int* pByte, const int nId, const char* pRecvData)
{
	int byte = 0;

	// �Ԃ�ID�𓾂�
	int carid = -1;
	memcpy(&carid, &pRecvData[byte], sizeof(int));
	*pByte += sizeof(int);
	byte += sizeof(int);

	CPolice* pCar = CPoliceManager::GetInstance()->GetMapList()->Get(carid);

	// �Ԃ����݂��Ă��Ȃ�
	if (pCar == nullptr)
	{
		pCar = CAddPolice::Create(VECTOR3_ZERO, VECTOR3_ZERO, VECTOR3_ZERO, carid);
		pCar->SetType(CCar::TYPE::TYPE_RECV);
	}

	// �ǐՏI����Ԃɂ���
	pCar->SetChaseNext(CPolice::CHASE::CHASE_END);
}

//===================================================
// �������M���Ȃ�
//===================================================
void CNetWork::SendNone()
{
	if (!GetActive()) { return; }
}

//===================================================
// �����������Ƃ𑗐M
//===================================================
void CNetWork::SendJoin()
{
	if (!GetActive()) { return; }
}

//===================================================
// ID�擾�𑗐M
//===================================================
void CNetWork::SendGetId()
{
	if (!GetActive()) { return; }

	char aSendData[sizeof(int)] = {};	// ���M�p
	int nProt = NetWork::COMMAND_GETID;

	// protocol��}��
	memcpy(&aSendData[0], &nProt, sizeof(int));

	// ���M
	m_pClient->SetData(&aSendData[0], sizeof(int));
}

//===================================================
// �폜���邱�Ƃ𑗐M
//===================================================
void CNetWork::SendDelete()
{
	if (!GetActive()) { return; }

	char aSendData[sizeof(int)] = {};	// ���M�p
	int nProt = NetWork::COMMAND_DELETE;
	int byte = 0;

	// protocol��}��
	memcpy(&aSendData[byte], &nProt, sizeof(int));
	byte += sizeof(int);

	// ���M
	m_pClient->SetData(&aSendData[0], byte);

	DisConnect();
}

//===================================================
// �v���C���[�̍��W���M
//===================================================
void CNetWork::SendPlPos(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot)
{
	if (!GetActive()) { return; }

	char aSendData[sizeof(int) + sizeof(D3DXVECTOR3) + sizeof(D3DXVECTOR3) + 1] = {};	// ���M�p
	int nProt = NetWork::COMMAND_PL_POS;
	int byte = 0;

	// protocol��}��
	memcpy(&aSendData[byte], &nProt, sizeof(int));
	byte += sizeof(int);

	// ���W��}��
	memcpy(&aSendData[byte], &pos, sizeof(D3DXVECTOR3));
	byte += sizeof(D3DXVECTOR3);

	// ������}��
	memcpy(&aSendData[byte], &rot, sizeof(D3DXVECTOR3));
	byte += sizeof(D3DXVECTOR3);

	// ���M
	m_pClient->SetData(&aSendData[0], byte);
}

//===================================================
// �v���C���[�̃_���[�W��̗͑��M
//===================================================
void CNetWork::SendPlDamage(const float nowlife)
{
	if (!GetActive()) { return; }

	char aSendData[sizeof(int) + sizeof(float)] = {};	// ���M�p
	int nProt = NetWork::COMMAND_PL_DAMAGE;
	int byte = 0;

	// protocol��}��
	memcpy(&aSendData[byte], &nProt, sizeof(int));
	byte += sizeof(int);

	// �̗͂�}��
	memcpy(&aSendData[byte], &nowlife, sizeof(float));
	byte += sizeof(float);

	// ���M
	m_pClient->SetData(&aSendData[0], byte);
}

//===================================================
// �v���C���[���S�[���������Ƃ𑗐M
//===================================================
void CNetWork::SendPlGoal(int nId)
{
	if (!GetActive()) { return; }

	char aSendData[sizeof(int) + sizeof(int) + 1] = {};	// ���M�p
	int nProt = NetWork::COMMAND_PL_GOAL;
	int byte = 0;

	// protocol��}��
	memcpy(&aSendData[byte], &nProt, sizeof(int));
	byte += sizeof(int);

	// �S�[������ID��}��
	memcpy(&aSendData[byte], &nId, sizeof(int));
	byte += sizeof(int);

	// ���M
	m_pClient->SetData(&aSendData[0], byte);
}

//===================================================
// �M�~�b�N�ɏՓ˂������Ƃ𑗐M
//===================================================
void CNetWork::SendGmHit(const int nId, const D3DXVECTOR3& HitPos, const float fSpeed)
{
	if (!GetActive()) { return; }

	char aSendData[sizeof(int) + sizeof(int) + sizeof(D3DXVECTOR3) + sizeof(float) + 1] = {};	// ���M�p
	int nProt = NetWork::COMMAND_GM_HIT;
	int byte = 0;

	// protocol��}��
	memcpy(&aSendData[byte], &nProt, sizeof(int));
	byte += sizeof(int);

	// �Փ˂����M�~�b�N��ID��}��
	memcpy(&aSendData[byte], &nId, sizeof(int));
	byte += sizeof(int);

	// �Փ˂������W��}��
	memcpy(&aSendData[byte], &HitPos, sizeof(D3DXVECTOR3));
	byte += sizeof(D3DXVECTOR3);

	// �Փˑ��x��}��
	memcpy(&aSendData[byte], &fSpeed, sizeof(float));
	byte += sizeof(float);

	// ���M
	m_pClient->SetData(&aSendData[0], byte);
}

//===================================================
// ���̃S�[�����ԍ��𑗐M
//===================================================
void CNetWork::SendNextGoal(const int nId)
{
	if (!GetActive()) { return; }

	char aSendData[sizeof(int) + sizeof(int) + 1] = {};	// ���M�p
	int nProt = NetWork::COMMAND_NEXT_GOAL;
	int byte = 0;

	// protocol��}��
	memcpy(&aSendData[byte], &nProt, sizeof(int));
	byte += sizeof(int);

	// �S�[������ID��}��
	memcpy(&aSendData[byte], &nId, sizeof(int));
	byte += sizeof(int);

	// ���M
	m_pClient->SetData(&aSendData[0], byte);
}

//===================================================
// �Q�[���J�n�\�Ȃ̂𑗐M
//===================================================
void CNetWork::SendGameStartOk()
{
	if (!GetActive()) { return; }

	char aSendData[sizeof(int)] = {};	// ���M�p
	int nProt = NetWork::COMMAND_GAMESTART_OK;
	int byte = 0;

	// protocol��}��
	memcpy(&aSendData[byte], &nProt, sizeof(int));
	byte += sizeof(int);

	// ���M
	m_pClient->SetData(&aSendData[0], byte);
}

//===================================================
// �`���[�g���A��OK
//===================================================
void CNetWork::SendTutorialOk()
{
	if (!GetActive()) { return; }

	char aSendData[sizeof(int)] = {};	// ���M�p
	int nProt = NetWork::COMMAND_TUTORIAL_OK;
	int byte = 0;

	// protocol��}��
	memcpy(&aSendData[byte], &nProt, sizeof(int));
	byte += sizeof(int);

	// ���M
	m_pClient->SetData(&aSendData[0], byte);
}

//===================================================
// �`���[�g���A��Ok�L�����Z��
//===================================================
void CNetWork::SendTutorialNo()
{
	if (!GetActive()) { return; }

	char aSendData[sizeof(int)] = {};	// ���M�p
	int nProt = NetWork::COMMAND_TUTORIAL_NO;
	int byte = 0;

	// protocol��}��
	memcpy(&aSendData[byte], &nProt, sizeof(int));
	byte += sizeof(int);

	// ���M
	m_pClient->SetData(&aSendData[0], byte);
}

//===================================================
// ����ݒu�𑗐M
//===================================================
void CNetWork::SendSetInspection(const int nId, const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, int nIdx, int nStartPdId)
{
	//if (!GetActive()) { return; }

	//char aSendData[sizeof(int) + sizeof(int) + sizeof(D3DXVECTOR3) + sizeof(D3DXVECTOR3) + sizeof(int) + sizeof(int)] = {};	// ���M�p
	//int nProt = NetWork::COMMAND_SET_INSP;
	//int byte = 0;

	//// protocol��}��
	//memcpy(&aSendData[byte], &nProt, sizeof(int));
	//byte += sizeof(int);

	//// ID
	//memcpy(&aSendData[byte], &nId, sizeof(int));
	//byte += sizeof(int);

	//// ���W
	//memcpy(&aSendData[byte], &pos, sizeof(D3DXVECTOR3));
	//byte += sizeof(D3DXVECTOR3);

	//// ����
	//memcpy(&aSendData[byte], &rot, sizeof(D3DXVECTOR3));
	//byte += sizeof(D3DXVECTOR3);

	//// ����ID
	//memcpy(&aSendData[byte], &nIdx, sizeof(int));
	//byte += sizeof(int);

	//// �x�@�̊J�nID
	//memcpy(&aSendData[byte], &nStartPdId, sizeof(int));
	//byte += sizeof(int);

	//// ���M
	//m_pClient->SetData(&aSendData[0], byte);
}

//===================================================
// ����I���𑗐M
//===================================================
void CNetWork::SendEndInspection(int nId)
{
	//if (!GetActive()) { return; }

	//char aSendData[sizeof(int) + sizeof(int)] = {};	// ���M�p
	//int nProt = NetWork::COMMAND_END_INSP;
	//int byte = 0;

	//// protocol��}��
	//memcpy(&aSendData[byte], &nProt, sizeof(int));
	//byte += sizeof(int);

	//// �����ID
	//memcpy(&aSendData[byte], &nId, sizeof(int));
	//byte += sizeof(int);

	//// ���M
	//m_pClient->SetData(&aSendData[0], byte);
}

//===================================================
// �Ԃ̍��W�𑗐M
//===================================================
void CNetWork::SendCarPos(int nId, const D3DXVECTOR3& pos, const D3DXVECTOR3& rot)
{
	if (!GetActive()) { return; }

	char aSendData[sizeof(int) + sizeof(int) + sizeof(D3DXVECTOR3) + sizeof(D3DXVECTOR3)] = {};	// ���M�p
	int nProt = NetWork::COMMAND_CAR_POS;
	int byte = 0;

	// protocol��}��
	memcpy(&aSendData[byte], &nProt, sizeof(int));
	byte += sizeof(int);

	// ID��}��
	memcpy(&aSendData[byte], &nId, sizeof(int));
	byte += sizeof(int);

	// ���W��}��
	memcpy(&aSendData[byte], &pos, sizeof(D3DXVECTOR3));
	byte += sizeof(D3DXVECTOR3);

	// ������}��
	memcpy(&aSendData[byte], &rot, sizeof(D3DXVECTOR3));
	byte += sizeof(D3DXVECTOR3);

	// ���M
	m_pClient->SetData(&aSendData[0], byte);
}

//===================================================
// �x�@�̍��W�𑗐M
//===================================================
void CNetWork::SendPdPos(int nId, const D3DXVECTOR3& pos, const D3DXVECTOR3& rot)
{
	//if (!GetActive()) { return; }

	//char aSendData[sizeof(int) + sizeof(int) + sizeof(D3DXVECTOR3) + sizeof(D3DXVECTOR3)] = {};	// ���M�p
	//int nProt = NetWork::COMMAND_PD_POS;
	//int byte = 0;

	//// protocol��}��
	//memcpy(&aSendData[byte], &nProt, sizeof(int));
	//byte += sizeof(int);

	//// Id��}��
	//memcpy(&aSendData[byte], &nId, sizeof(int));
	//byte += sizeof(int);

	//// ���W��}��
	//memcpy(&aSendData[byte], &pos, sizeof(D3DXVECTOR3));
	//byte += sizeof(D3DXVECTOR3);

	//// ������}��
	//memcpy(&aSendData[byte], &rot, sizeof(D3DXVECTOR3));
	//byte += sizeof(D3DXVECTOR3);

	//// ���M
	//m_pClient->SetData(&aSendData[0], byte);
}

//===================================================
// �ǉ��x�@�̍��W�𑗐M
//===================================================
void CNetWork::SendAddPdPos(int nId, const D3DXVECTOR3& pos, const D3DXVECTOR3& rot)
{
	//if (!GetActive()) { return; }

	//char aSendData[sizeof(int) + sizeof(int) + sizeof(D3DXVECTOR3) + sizeof(D3DXVECTOR3)] = {};	// ���M�p
	//int nProt = NetWork::COMMAND_ADDPD_POS;
	//int byte = 0;

	//// protocol��}��
	//memcpy(&aSendData[byte], &nProt, sizeof(int));
	//byte += sizeof(int);

	//// Id��}��
	//memcpy(&aSendData[byte], &nId, sizeof(int));
	//byte += sizeof(int);

	//// ���W��}��
	//memcpy(&aSendData[byte], &pos, sizeof(D3DXVECTOR3));
	//byte += sizeof(D3DXVECTOR3);

	//// ������}��
	//memcpy(&aSendData[byte], &rot, sizeof(D3DXVECTOR3));
	//byte += sizeof(D3DXVECTOR3);

	//// ���M
	//m_pClient->SetData(&aSendData[0], byte);
}

//===================================================
// �x�@�̒ǐՊJ�n���M
//===================================================
void CNetWork::SendPdChase(int nId, int plyid)
{
	//if (!GetActive()) { return; }

	//char aSendData[sizeof(int) + sizeof(int) + sizeof(int)] = {};	// ���M�p
	//int nProt = NetWork::COMMAND_PD_CHASE;
	//int byte = 0;

	//// protocol��}��
	//memcpy(&aSendData[byte], &nProt, sizeof(int));
	//byte += sizeof(int);

	//// Id��}��
	//memcpy(&aSendData[byte], &nId, sizeof(int));
	//byte += sizeof(int);

	//// �ǐՂ���v���C���[Id��}��
	//memcpy(&aSendData[byte], &plyid, sizeof(int));
	//byte += sizeof(int);

	//// ���M
	//m_pClient->SetData(&aSendData[0], byte);
}

//===================================================
// �ǉ��x�@�̒ǐՊJ�n���M
//===================================================
void CNetWork::SendAddPdChase(int nId, int plyid)
{
	//if (!GetActive()) { return; }

	//char aSendData[sizeof(int) + sizeof(int) + sizeof(int)] = {};	// ���M�p
	//int nProt = NetWork::COMMAND_ADDPD_CHASE;
	//int byte = 0;

	//// protocol��}��
	//memcpy(&aSendData[byte], &nProt, sizeof(int));
	//byte += sizeof(int);

	//// Id��}��
	//memcpy(&aSendData[byte], &nId, sizeof(int));
	//byte += sizeof(int);

	//// �ǐՂ���v���C���[Id��}��
	//memcpy(&aSendData[byte], &plyid , sizeof(int));
	//byte += sizeof(int);

	//// ���M
	//m_pClient->SetData(&aSendData[0], byte);
}

//===================================================
// �x�@�̒ǐՏI�����M
//===================================================
void CNetWork::SendPdChaseEnd(int nId)
{
	//if (!GetActive()) { return; }

	//char aSendData[sizeof(int) + sizeof(int)] = {};	// ���M�p
	//int nProt = NetWork::COMMAND_PD_CHASEEND;
	//int byte = 0;

	//// protocol��}��
	//memcpy(&aSendData[byte], &nProt, sizeof(int));
	//byte += sizeof(int);

	//// Id��}��
	//memcpy(&aSendData[byte], &nId, sizeof(int));
	//byte += sizeof(int);

	//// ���M
	//m_pClient->SetData(&aSendData[0], byte);
}

//===================================================
// �ǉ��x�@�̒ǐՏI�����M
//===================================================
void CNetWork::SendAddPdChaseEnd(int nId)
{
	//if (!GetActive()) { return; }

	//char aSendData[sizeof(int) + sizeof(int)] = {};	// ���M�p
	//int nProt = NetWork::COMMAND_ADDPD_CHASEEND;
	//int byte = 0;

	//// protocol��}��
	//memcpy(&aSendData[byte], &nProt, sizeof(int));
	//byte += sizeof(int);

	//// Id��}��
	//memcpy(&aSendData[byte], &nId, sizeof(int));
	//byte += sizeof(int);

	//// ���M
	//m_pClient->SetData(&aSendData[0], byte);
}