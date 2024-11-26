//===============================================
//
// �ʐM�����̊Ǘ����� [network.cpp]
// Author : Ibuki Okusada
//
//===============================================
#include "network.h"
#include "tcp_client.h"
#include "tcp_server.h"
#include "deltatime.h"

//===============================================
// ���O���
//===============================================
namespace
{
	const int DEF_PORT = (22333);			// �f�t�H���g�̃|�[�g�ԍ�
	const float SEND_TIME = 1.0f;			// ���M����
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
	&CNetWork::CommandPlPos,	// �v���C���[�ʒu
	&CNetWork::CommandPlRot,	// �v���C���[����
	&CNetWork::CommandPlDamage,	// �v���C���[�_���\�W
	&CNetWork::CommandPlGoal,	// �v���C���[�S�[��
};

// �ÓI�����o�ϐ�
CNetWork* CNetWork::m_pInstance = nullptr;

//===============================================
// �R���X�g���N�^
//===============================================
CNetWork::CNetWork()
{
	// �l�̃N���A
	m_bEnd = false;
	m_nSendByte = 0;
	m_nSledCnt = 0;
	m_pServer = nullptr;

	memset(m_aSendData, NULL, sizeof(m_aSendData));

	for (int i = 0; i < NetWork::MAX_CONNECT; i++)
	{
		m_apClient[i] = nullptr;
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
		if (FAILED(m_pInstance->Init()))
		{
			Release();
		}
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
	// Winsock�̏���������
	WSADATA wsaData;
	int nErr = WSAStartup(WINSOCK_VERSION, &wsaData);	// winsock�̏������֐�

	if (nErr != 0)
	{// �������Ɏ��s�����ꍇ

		printf("*** Winsock�̏������Ɏ��s���܂��� ***\n");

		// �L�[���͑҂�
		rewind(stdin);
		getchar();

		return E_FAIL;	// �I��
	}

	// �T�[�o�[�̐���
	m_pServer = DEBUG_NEW CServer;

	// ����������
	if (m_pServer->Init(DEF_PORT) == false)
	{
		return E_FAIL;
	}

	// �ʃX���b�h�ő��M����
	{
		std::thread th(&CNetWork::Send, this, &m_pServer);
		th.detach();
	}

	// �ʃX���b�h�Ŏ󂯓���҂�
	{
		std::thread th(&CNetWork::Accept, this, m_pServer);
		th.detach();
	}

	return S_OK;
}

//===============================================
// �I��
//===============================================
void CNetWork::Uninit()
{
	//Winsock�I������
	WSACleanup();	// WSACleanup�֐� : winsock�̏I������

	m_bEnd = true;

	while (1)
	{
		if (m_nSledCnt <= 0)
		{
			break;
		}
	}

	// �T�[�o�[�̏I��
	if (m_pServer != nullptr)
	{
		m_pServer->Uninit();
		delete m_pServer;
		m_pServer = nullptr;
	}

	// �N���C�A���g�̏I��
	for (int i = 0; i < NetWork::MAX_CONNECT; i++)
	{
		if (m_apClient[i] == nullptr) { continue; }

		m_apClient[i]->Uninit();
		delete m_apClient[i];
		m_apClient[i] = nullptr;
	}

	delete this;
}

//==========================================================
// �󂯓���҂�
//==========================================================
void CNetWork::Accept(CServer* pServer)
{
	while (1)
	{
		for (int i = 0; i < NetWork::MAX_CONNECT; i++)
		{
			if (m_apClient[i] != nullptr) { continue; }

			// ����M�p����
			CClient* pClient = new CClient;
			pClient->BindSock(pServer->Accept());
			pClient->SetCliePort(pServer->GetPort());
			pClient->SetIP(pServer->GetIP());

			// ID�ݒ�
			pClient->SetId(i);

			// �}���`�X���b�h
			std::thread th(&CNetWork::Access, this, pClient);
			th.detach();

			m_apClient[i] = pClient;
		}
	}
}

//==========================================================
// ���M
//==========================================================
void CNetWork::Send(CServer** ppServer)
{
	auto delta = CDeltaTime::GetInstance();
	float time = 0.0f;
	// �T�[�o�[��������܂ŌJ��Ԃ�
	while (*ppServer != nullptr)
	{
		time += delta->GetDeltaTime();

		if (time >= SEND_TIME)
		{// ���M���Ԍo��
			time = 0.0f;

			for(int i = 0; i < NetWork::MAX_CONNECT; i++)
			{// �g�p����Ă��Ȃ���Ԃ܂�
				// �f�[�^�̍���
				CClient* pClient = m_apClient[i];	// �擪���擾

				if (pClient == nullptr) { continue; }

				pClient->SetSend(true);	// ���������s�\�ȏ�Ԃɂ���

				memcpy(&m_aSendData[m_nSendByte], pClient->GetSendData(), pClient->GetSendByte());
				m_nSendByte += pClient->GetSendByte();

				if (pClient->GetDeath() == false)
				{
					pClient->ResetData();	// �f�[�^�̃��Z�b�g
					pClient->SetSend(false);	// ���������\�ȏ�Ԃɂ���
				}
				else
				{
					if (pClient != nullptr)
					{
						pClient->Uninit();
						delete pClient;
						pClient = nullptr;
						m_apClient[i] = nullptr;
					}
				}
			}

			for (int i = 0; i < NetWork::MAX_CONNECT; i++)
			{// �g�p����Ă��Ȃ���Ԃ܂�
				// �f�[�^�̍���
				CClient* pClient = m_apClient[i];	// �擪���擾

				if (pClient == nullptr) { continue; }

				// ���M
				pClient->Send(&m_aSendData[0], m_nSendByte);
			}

			// ���M�f�[�^���N���A
			memset(&m_aSendData[0], '\0', sizeof(m_aSendData));
			m_nSendByte = 0;

		}
	}
}

//==========================================================
// ��M
//==========================================================
void CNetWork::Access(CClient* pClient)
{
	int id = pClient->GetId();

	char aSendData[sizeof(int) * 2] = {};	// ���M�p

	// �ݒ�
	CommandJoin(pClient->GetId(), nullptr, pClient);

	while (1)
	{
		D3DXCOLOR col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		D3DXVECTOR3 pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		char recvdata[NetWork::MAX_COMMAND_DATA] = {};	// ��M�p
		int command = NetWork::COMMAND_NONE;

		// ��M
		int recvbite = pClient->Recv(&recvdata[0], NetWork::MAX_COMMAND_DATA);

		memcpy(&command, &recvdata[0], sizeof(int));

		if (recvbite <= 0)
		{
			Leave(pClient->GetId(), pClient);
			break;
		}
		else
		{
			if (pClient->GetSend() == true)
			{
				continue;
			}

			// �R�}���h�����s
			(this->*(m_CommandFunc[command]))(id, &recvdata[sizeof(int)], pClient);
		}
	}
}

//==========================================================
// �ގ�
//==========================================================
void CNetWork::Leave(int nId, CClient* pClient)
{
	int nProt = NetWork::COMMAND_DELETE;	// �v���g�R���ԍ�
	char aSendData[sizeof(int) * 2] = {};	// ���M�p�܂Ƃ߃f�[�^

	// ID��}��
	memcpy(&aSendData[0], &nId, sizeof(int));
	memcpy(&aSendData[sizeof(int)], &nProt, sizeof(int));

	// �v���g�R����}��
	pClient->ResetData();
	pClient->SetData(&aSendData[0], sizeof(int) * 2);
	pClient->SetDeath(true);
}

//==========================================================
// �������Ȃ�
//==========================================================
void CNetWork::CommandNone(const int nId, const char* pRecvData, CClient* pClient)
{

}

//==========================================================
// ����
//==========================================================
void CNetWork::CommandJoin(const int nId, const char* pRecvData, CClient* pClient)
{
	int nProt = -1;	// �v���g�R���ԍ�
	char aSendData[sizeof(int) * 2] = {};	// ���M�p�܂Ƃ߃f�[�^

	// ID��}��
	nProt = NetWork::COMMAND_JOIN;

	// ID��}��
	memcpy(&aSendData[0], &nId, sizeof(int));
	memcpy(&aSendData[sizeof(int)], &nProt, sizeof(int));

	// �v���g�R����}��
	pClient->SetData(&aSendData[0], sizeof(int) * 2);

	// ID��Ԃ�
	CommandGetId(nId, pRecvData, pClient);
}

//==========================================================
// ID���擾
//==========================================================
void CNetWork::CommandGetId(const int nId, const char* pRecvData, CClient* pClient)
{
	char aRecv[sizeof(int) * 2] = {};
	int command = NetWork::COMMAND_GETID;

	// ID��}��
	memcpy(&aRecv[0], &nId, sizeof(int));
	memcpy(&aRecv[sizeof(int)], &command, sizeof(int));

	// �v���g�R���𑗐M
	pClient->Send(&aRecv[0], sizeof(int) * 2);
}

//==========================================================
// �폜
//==========================================================
void CNetWork::CommandDelete(const int nId, const char* pRecvData, CClient* pClient)
{
	int nProt = NetWork::COMMAND_DELETE;	// �v���g�R���ԍ�
	char aSendData[sizeof(int) * 2] = {};	// ���M�p�܂Ƃ߃f�[�^

	// ID��}��
	memcpy(&aSendData[0], &nId, sizeof(int));
	memcpy(&aSendData[sizeof(int)], &nProt, sizeof(int));

	// �v���g�R����}��
	pClient->ResetData();
	pClient->SetData(&aSendData[0], sizeof(int) * 2);
	pClient->SetDeath(true);
}

//==========================================================
// �v���C���[�̍��W
//==========================================================
void CNetWork::CommandPlPos(const int nId, const char* pRecvData, CClient* pClient)
{
	int nProt = -1;	// �v���g�R���ԍ�
	char aSendData[sizeof(int) * 2 + sizeof(D3DXVECTOR3) + 1] = {};	// ���M�p�܂Ƃ߃f�[�^

	D3DXVECTOR3 pos;

	// ���W�}��
	memcpy(&pos, pRecvData, sizeof(D3DXVECTOR3));

	// ID��}��
	nProt = NetWork::COMMAND_PL_POS;

	// ID��}��
	memcpy(&aSendData[0], &nId, sizeof(int));

	// �R�}���h�}��
	memcpy(&aSendData[sizeof(int)], &nProt, sizeof(int));

	// ���W�}��
	memcpy(&aSendData[sizeof(int) * 2], &pos, sizeof(D3DXVECTOR3));

	// �v���g�R����}��
	pClient->SetData(&aSendData[0], sizeof(int) * 2 + sizeof(D3DXVECTOR3));
}

//==========================================================
// �v���C���[�̌���
//==========================================================
void CNetWork::CommandPlRot(const int nId, const char* pRecvData, CClient* pClient)
{
	int nProt = -1;	// �v���g�R���ԍ�
	char aSendData[sizeof(int) * 2 + sizeof(D3DXVECTOR3) + 1] = {};	// ���M�p�܂Ƃ߃f�[�^

	// ID��}��
	nProt = NetWork::COMMAND_PL_ROT;

	D3DXVECTOR3 rot;

	// ���W�}��
	memcpy(&rot, pRecvData, sizeof(D3DXVECTOR3));

	// ID��}��
	memcpy(&aSendData[0], &nId, sizeof(int));

	// �R�}���h�}��
	memcpy(&aSendData[sizeof(int)], &nProt, sizeof(int));

	// ���W�}��
	memcpy(&aSendData[sizeof(int) * 2], &rot, sizeof(D3DXVECTOR3));

	// �v���g�R����}��
	pClient->SetData(&aSendData[0], sizeof(int) * 2 + sizeof(D3DXVECTOR3));

}

//==========================================================
// �v���C���[�̃_���[�W��̗�
//==========================================================
void CNetWork::CommandPlDamage(const int nId, const char* pRecvData, CClient* pClient)
{

}

//==========================================================
// �v���C���[���S�[��
//==========================================================
void CNetWork::CommandPlGoal(const int nId, const char* pRecvData, CClient* pClient)
{
	int nProt = -1;	// �v���g�R���ԍ�
	char aSendData[sizeof(int) * 2 + sizeof(int) + 1] = {};	// ���M�p�܂Ƃ߃f�[�^

	// ID��}��
	nProt = NetWork::COMMAND_PL_GOAL;

	// ID��}��
	memcpy(&aSendData[0], &nId, sizeof(int));

	// �R�}���h�}��
	memcpy(&aSendData[sizeof(int)], &nProt, sizeof(int));

	// �S�[��ID�}��
	memcpy(&aSendData[sizeof(int) * 2], pRecvData, sizeof(int));

	// �v���g�R����}��
	pClient->SetData(&aSendData[0], sizeof(int) * 2 + sizeof(int));
}