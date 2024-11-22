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
	const int MAX_STRING = (2048);
}

//===============================================
// �֐��|�C���^
//===============================================
// ��ԊǗ�
CNetWork::COMMAND_FUNC CNetWork::m_CommandFunc[] =
{
	&CNetWork::CommandNone,		// �����Ȃ�
	&CNetWork::CommandJoin,		// �ڑ�����
	&CNetWork::CommandGetId,	// ID�擾
	&CNetWork::CommandDelete,	// �폜
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

	// �ڑ�
	if (ReConnect())
	{
		m_state = STATE_ONLINE;
	}
	else
	{
		m_nMyIdx = 0;
		m_state = STATE_SINGLE;
	}

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
		m_nMyIdx = 0;
		return false; 
	}

	m_bEnd = false;

	// �}���`�X���b�h
	std::thread th(&CNetWork::Online, this);
	th.detach();

	m_state = STATE_ONLINE;
	return true;
}

//===============================================
// �ؒf
//===============================================
bool CNetWork::DisConnect()
{
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

	m_bEnd = true;

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
		if (m_bEnd == true || m_pClient == nullptr)
		{
			break;
		}

		char* pData = DEBUG_NEW char[MAX_STRING];	// ��M�p

		// ��M
		int* pRecvByte = DEBUG_NEW int;
		*pRecvByte = m_pClient->Recv(&pData[0], MAX_STRING);

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
		(this->*(m_CommandFunc[command]))(&nByte, id, &pRecvData[nByte]);
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
		m_pClient->Send(senddata.c_str(), sizeof(int));
	}
}

//===================================================
// �����v���C���[
//===================================================
CPlayer* CNetWork::CreatePlayer(int nId)
{
	// �v���C���[����
	CPlayer* pPlayer = CPlayer::Create(VECTOR3_ZERO, VECTOR3_ZERO, VECTOR3_ZERO, nullptr, nullptr);
	auto mgr = CPlayerManager::GetInstance();

	// ���X�g�ɑ}�����Ȃ���
	mgr->ListOut(pPlayer);
	pPlayer->BindId(nId);
	mgr->ListIn(pPlayer);

	return pPlayer;
}

//===================================================
// �����Ȃ�
//===================================================
void CNetWork::CommandNone(int* pByte, const int nId, const char* pRecvData)
{

}

//===================================================
// �ڑ�����
//===================================================
void CNetWork::CommandJoin(int* pByte, const int nId, const char* pRecvData)
{
	if (nId < 0 || nId >= NetWork::MAX_CONNECT) { return; }

	m_aConnect[nId] = true;
}

//===================================================
// ID���擾����
//===================================================
void CNetWork::CommandGetId(int* pByte, const int nId, const char* pRecvData)
{
	m_nMyIdx = nId;
}

//===================================================
// �폜
//===================================================
void CNetWork::CommandDelete(int* pByte, const int nId, const char* pRecvData)
{
	if (nId < 0 || nId >= NetWork::MAX_CONNECT) { return; }

	m_aConnect[nId] = false;
}
