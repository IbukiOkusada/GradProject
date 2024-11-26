//===============================================
//
// �ʐM�����̊Ǘ����� [network.cpp]
// Author : Ibuki Okusada
//
//===============================================
#include "network.h"
#include "tcp_client.h"
#include "player.h"
#include "player_manager.h"

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
	&CNetWork::RecvPlRot,	// �v���C���[����
	&CNetWork::RecvPlDamage,	// �v���C���[�_���[�W
	&CNetWork::RecvPlGoal,	// �v���C���[�S�[��
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

	// �N���C�A���g�̏I��
	if (m_pClient != nullptr)
	{
		m_pClient->Uninit();
		delete m_pClient;
		m_pClient = nullptr;
	}

	delete this;
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
	m_pClient->Uninit();

	for (int i = 0; i < NetWork::MAX_CONNECT; i++)
	{
		m_aConnect[i] = false;
	}

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
		*pRecvByte = m_pClient->Recv(&pData[0], NetWork::MAX_SEND_DATA);

		// �}���`�X���b�h
		std::thread th(&CNetWork::ByteCheck, this, pData, pRecvByte);
		th.detach();
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
	while (nByte < *pRecvByte)
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
		if (command >= NetWork::COMMAND_MAX)
		{
			// �f�[�^�폜
			DeleteData(pRecvData, pRecvByte);

			m_nSledCnt--;
			m_mutex.unlock();
			return;
		}

		// �R�}���h�����s
		if (nByte < *pRecvByte)
		{
			(this->*(m_RecvFunc[command]))(&nByte, id, &pRecvData[nByte]);
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
	if (GetActive())
	{
		std::string senddata;
		senddata.resize(sizeof(int) + 1);
		int nProt = NetWork::COMMAND_DELETE;

		// protocol��}��
		memcpy(&senddata[0], &nProt, sizeof(int));

		// ���M
		m_pClient->Send(senddata.c_str(), sizeof(int));
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
	auto mgr = CPlayerManager::GetInstance();

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

	// �v���C���[�̑��݊m�F
	if (nId < 0 || nId >= NetWork::MAX_CONNECT) { return; }
	CPlayer* pPlayer = CPlayerManager::GetInstance()->GetPlayer(nId);

	// �������Ă��Ȃ��ꍇ
	if (pPlayer == nullptr) { 
		RecvJoin(pByte, nId, pRecvData);
		return; 
	}

	// ���W�ݒ�
	pPlayer->SetRecvPosition(pos);
}

//===================================================
// �v���C���[�̌������擾
//===================================================
void CNetWork::RecvPlRot(int* pByte, const int nId, const char* pRecvData)
{
	// �����ɕϊ�
	D3DXVECTOR3 rot = VECTOR3_ZERO;
	memcpy(&rot, pRecvData, sizeof(D3DXVECTOR3));

	// �m�F�o�C�g�������Z
	*pByte += sizeof(D3DXVECTOR3);

	// �v���C���[�̑��݊m�F
	if (nId < 0 || nId >= NetWork::MAX_CONNECT) { return; }
	CPlayer* pPlayer = CPlayerManager::GetInstance()->GetPlayer(nId);

	// �������Ă��Ȃ��ꍇ
	if (pPlayer == nullptr) {
		RecvJoin(pByte, nId, pRecvData);
		return;
	}

	// ���W�ݒ�
	pPlayer->SetRecvRotation(rot);
}

//===================================================
// �v���C���[�̃_���[�W���擾
//===================================================
void CNetWork::RecvPlDamage(int* pByte, const int nId, const char* pRecvData)
{
	
}

//===================================================
// �v���C���[�̃S�[�����擾
//===================================================
void CNetWork::RecvPlGoal(int* pByte, const int nId, const char* pRecvData)
{

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
	m_pClient->Send(&aSendData[0], sizeof(int));
}

//===================================================
// �폜���邱�Ƃ𑗐M
//===================================================
void CNetWork::SendDelete()
{
	if (!GetActive()) { return; }

	char aSendData[sizeof(int)] = {};	// ���M�p
	int nProt = NetWork::COMMAND_DELETE;

	// protocol��}��
	memcpy(&aSendData[0], &nProt, sizeof(int));

	// ���M
	m_pClient->Send(&aSendData[0], sizeof(int));

	DisConnect();
}

//===================================================
// �v���C���[�̍��W���M
//===================================================
void CNetWork::SendPlPos(const D3DXVECTOR3& pos)
{
	if (!GetActive()) { return; }

	char aSendData[sizeof(int) + sizeof(D3DXVECTOR3) + 1] = {};	// ���M�p
	int nProt = NetWork::COMMAND_PL_POS;

	// protocol��}��
	memcpy(&aSendData[0], &nProt, sizeof(int));

	// ���W��}��
	memcpy(&aSendData[sizeof(int)], &pos, sizeof(D3DXVECTOR3));

	// ���M
	m_pClient->Send(&aSendData[0], sizeof(int) + sizeof(D3DXVECTOR3));
}

//===================================================
// �v���C���[�̌������M
//===================================================
void CNetWork::SendPlRot(const D3DXVECTOR3& rot)
{
	if (!GetActive()) { return; }

	char aSendData[sizeof(int) + sizeof(D3DXVECTOR3) + 1] = {};	// ���M�p
	int nProt = NetWork::COMMAND_PL_ROT;

	// protocol��}��
	memcpy(&aSendData[0], &nProt, sizeof(int));

	// ���W��}��
	memcpy(&aSendData[sizeof(int)], &rot, sizeof(D3DXVECTOR3));

	// ���M
	m_pClient->Send(&aSendData[0], sizeof(int) + sizeof(D3DXVECTOR3));
}

//===================================================
// �v���C���[�̃_���[�W��̗͑��M
//===================================================
void CNetWork::SendPlDamage(const float nowlife)
{
	if (!GetActive()) { return; }
}

//===================================================
// �v���C���[���S�[���������Ƃ𑗐M
//===================================================
void CNetWork::SendPlGoal(int nId)
{
	if (!GetActive()) { return; }
}