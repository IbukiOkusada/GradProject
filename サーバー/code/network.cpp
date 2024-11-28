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
	const float SEND_TIME = 0.0f;			// ���M��t����
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
	&CNetWork::CommandGmHit,	// �M�~�b�N�Փ�
	&CNetWork::CommandNextGoal,	// ���̃S�[��
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

			/*printf("�ڑ��� :");
			for (int j = 0; j < NetWork::MAX_CONNECT; j++)
			{
				if (m_apClient[j] == nullptr) { continue; }
				printf("%d, ", j);
			}

			printf("\n");*/
		}
	}
}

//==========================================================
// ���M
//==========================================================
void CNetWork::Send(CServer** ppServer)
{
	auto delta = CDeltaTime::GetInstance();
	
	// �v���p
	NetWork::CTime time;
	time.Start();

	// �T�[�o�[��������܂ŌJ��Ԃ�
	while (*ppServer != nullptr)
	{	
		time.End();

		// �w�著�M���Ԃ��Ƃ�
		if(time.IsOK())
		{// ���M���Ԍo��

			// �J�n���ԃ��Z�b�g
			time.Start();

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
					pClient->ResetData();		// �f�[�^�̃��Z�b�g
					pClient->SetSend(false);	// ���������\�ȏ�Ԃɂ���
				}
				else
				{
					if (pClient != nullptr)
					{
						pClient->ResetData();		// �f�[�^�̃��Z�b�g
						pClient->Uninit();
						delete pClient;
						pClient = nullptr;
						m_apClient[i] = nullptr;

						printf("�N���C�A���g�ؒf\n");
					}
				}
			}


			if (m_nSendByte > 0)
			{
				for (int i = 0; i < NetWork::MAX_CONNECT; i++)
				{// �g�p����Ă��Ȃ���Ԃ܂�
					// �f�[�^�̍���
					CClient* pClient = m_apClient[i];	// �擪���擾

					if (pClient == nullptr) { continue; }

					// ���M
					if (pClient->Send(&m_aSendData[0], m_nSendByte) <= 0)
					{
						pClient->Uninit();
						delete pClient;
						pClient = nullptr;
						m_apClient[i] = nullptr;
						printf("�N���C�A���g�ؒf\n");
					}
				}

				printf("%d �o�C�g��������\n", m_nSendByte);
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
	int nowbyte = 0;

	// �ݒ�
	CommandJoin(pClient->GetId(), nullptr, pClient, &nowbyte);

	while (1)
	{
		char recvdata[NetWork::MAX_COMMAND_DATA] = {};	// ��M�p
		int command = NetWork::COMMAND_NONE;

		// ��M
		int recvbyte = pClient->Recv(&recvdata[0], NetWork::MAX_COMMAND_DATA);
		nowbyte = 0;

		if (recvbyte <= 0)
		{
			if (!pClient->GetDeath())
			{
				Leave(pClient->GetId(), pClient);
			}
			return;
		}

		while (1)
		{
			memcpy(&command, &recvdata[nowbyte], sizeof(int));
			nowbyte += sizeof(int);

			// �R�}���h�����s
			(this->*(m_CommandFunc[command]))(id, &recvdata[nowbyte], pClient, &nowbyte);

			if (command == NetWork::COMMAND_DELETE)
			{
				return;
			}

			if (nowbyte >= recvbyte)
			{
				break;
			}
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
	int byte = 0;

	// ID��}��
	memcpy(&aSendData[byte], &nId, sizeof(int));
	byte += sizeof(int);

	// �v���g�R���}��
	memcpy(&aSendData[byte], &nProt, sizeof(int));
	byte += sizeof(int);

	// �v���g�R����}��
	pClient->SetData(&aSendData[0], byte);
	pClient->SetDeath(true);
}

//==========================================================
// �������Ȃ�
//==========================================================
void CNetWork::CommandNone(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte)
{
	int nProt = -1;	// �v���g�R���ԍ�
	char aSendData[sizeof(int) * 2] = {};	// ���M�p�܂Ƃ߃f�[�^
	int byte = 0;

	// ID��}��
	nProt = NetWork::COMMAND_NONE;
	memcpy(&aSendData[byte], &nId, sizeof(int));
	byte += sizeof(int);

	// �v���g�R����}��
	memcpy(&aSendData[byte], &nProt, sizeof(int));
	byte += sizeof(int);

	// �v���g�R����}��
	pClient->SetData(&aSendData[0], byte);

	// ID��Ԃ�
	CommandGetId(nId, pRecvData, pClient, pNowByte);
}

//==========================================================
// ����
//==========================================================
void CNetWork::CommandJoin(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte)
{
	int nProt = -1;	// �v���g�R���ԍ�
	char aSendData[sizeof(int) * 2] = {};	// ���M�p�܂Ƃ߃f�[�^
	int byte = 0;

	// ID��}��
	nProt = NetWork::COMMAND_JOIN;

	// ID��}��
	memcpy(&aSendData[byte], &nId, sizeof(int));
	byte += sizeof(int);

	// �v���g�R����}��
	memcpy(&aSendData[byte], &nProt, sizeof(int));
	byte += sizeof(int);

	pClient->SetData(&aSendData[0], byte);

	// ID��Ԃ�
	CommandGetId(nId, pRecvData, pClient, pNowByte);
}

//==========================================================
// ID���擾
//==========================================================
void CNetWork::CommandGetId(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte)
{
	char aRecv[sizeof(int) * 2] = {};
	int command = NetWork::COMMAND_GETID;
	int byte = 0;

	// ID��}��
	memcpy(&aRecv[byte], &nId, sizeof(int));
	byte += sizeof(int);

	// �v���g�R����}��
	memcpy(&aRecv[byte], &command, sizeof(int));
	byte += sizeof(int);


	// �v���g�R���𑗐M
	pClient->Send(&aRecv[0], byte);
}

//==========================================================
// �폜
//==========================================================
void CNetWork::CommandDelete(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte)
{
	int nProt = NetWork::COMMAND_DELETE;	// �v���g�R���ԍ�
	char aSendData[sizeof(int) * 2] = {};	// ���M�p�܂Ƃ߃f�[�^
	int byte = 0;

	// ID��}��
	memcpy(&aSendData[byte], &nId, sizeof(int));
	byte += sizeof(int);

	// �v���g�R����}��
	memcpy(&aSendData[byte], &nProt, sizeof(int));
	byte += sizeof(int);

	// �f�[�^���Z�b�g
	pClient->ResetData();

	// �}��
	pClient->SetData(&aSendData[0], sizeof(int) * 2);

	// �g�p���Ȃ���Ԃ�
	pClient->SetDeath(true);
}

//==========================================================
// �v���C���[�̍��W
//==========================================================
void CNetWork::CommandPlPos(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte)
{
	int nProt = -1;	// �v���g�R���ԍ�
	char aSendData[sizeof(int) * 2 + sizeof(D3DXVECTOR3)] = {};	// ���M�p�܂Ƃ߃f�[�^
	int byte = 0;

	nProt = NetWork::COMMAND_PL_POS;

	// ID��}��
	memcpy(&aSendData[byte], &nId, sizeof(int));
	byte += sizeof(int);

	// �v���g�R���}��
	memcpy(&aSendData[byte], &nProt, sizeof(int));
	byte += sizeof(int);

	// ���W�}��
	memcpy(&aSendData[byte], pRecvData, sizeof(D3DXVECTOR3));
	*pNowByte += sizeof(D3DXVECTOR3);
	byte += sizeof(D3DXVECTOR3);

	// �}��
	pClient->SetData(&aSendData[0], byte);
}

//==========================================================
// �v���C���[�̌���
//==========================================================
void CNetWork::CommandPlRot(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte)
{
	int nProt = -1;	// �v���g�R���ԍ�
	char aSendData[sizeof(int) * 2 + sizeof(D3DXVECTOR3)] = {};	// ���M�p�܂Ƃ߃f�[�^
	int byte = 0;

	nProt = NetWork::COMMAND_PL_ROT;

	// ID��}��
	memcpy(&aSendData[byte], &nId, sizeof(int));
	byte += sizeof(int);

	// �v���g�R���}��
	memcpy(&aSendData[byte], &nProt, sizeof(int));
	byte += sizeof(int);

	// ���W�}��
	memcpy(&aSendData[byte], pRecvData, sizeof(D3DXVECTOR3));
	*pNowByte += sizeof(D3DXVECTOR3);
	byte += sizeof(D3DXVECTOR3);

	// �}��
	pClient->SetData(&aSendData[0], byte);
}

//==========================================================
// �v���C���[�̃_���[�W��̗�
//==========================================================
void CNetWork::CommandPlDamage(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte)
{

}

//==========================================================
// �v���C���[���S�[��
//==========================================================
void CNetWork::CommandPlGoal(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte)
{
	int nProt = -1;	// �v���g�R���ԍ�
	char aSendData[sizeof(int) * 2 + sizeof(int)] = {};	// ���M�p�܂Ƃ߃f�[�^
	int byte = 0;

	nProt = NetWork::COMMAND_PL_GOAL;

	// ID��}��
	memcpy(&aSendData[byte], &nId, sizeof(int));
	byte += sizeof(int);

	// �v���g�R���}��
	memcpy(&aSendData[byte], &nProt, sizeof(int));
	byte += sizeof(int);

	// �S�[��ID�}��
	memcpy(&aSendData[byte], pRecvData, sizeof(int));
	*pNowByte += sizeof(int);
	byte += sizeof(int);

	// �}��
	pClient->SetData(&aSendData[0], byte);
}

//==========================================================
// �M�~�b�N�Ƀq�b�g
//==========================================================
void CNetWork::CommandGmHit(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte)
{
	int nProt = -1;	// �v���g�R���ԍ�
	char aSendData[sizeof(int) * 2 + sizeof(int) + sizeof(D3DXVECTOR3) + sizeof(float)] = {};	// ���M�p�܂Ƃ߃f�[�^
	int byte = 0;
	int recvbyte = 0;

	nProt = NetWork::COMMAND_GM_HIT;

	// ID��}��
	memcpy(&aSendData[byte], &nId, sizeof(int));
	byte += sizeof(int);

	// �v���g�R���}��
	memcpy(&aSendData[byte], &nProt, sizeof(int));
	byte += sizeof(int);

	// �M�~�b�NID�}��
	memcpy(&aSendData[byte], &pRecvData[recvbyte], sizeof(int));
	*pNowByte += sizeof(int);
	byte += sizeof(int);
	recvbyte += sizeof(int);

	// �Փˍ��W�}��
	memcpy(&aSendData[byte], &pRecvData[recvbyte], sizeof(D3DXVECTOR3));
	*pNowByte += sizeof(D3DXVECTOR3);
	byte += sizeof(D3DXVECTOR3);
	recvbyte += sizeof(D3DXVECTOR3);

	// ���x�}��
	memcpy(&aSendData[byte], &pRecvData[recvbyte], sizeof(float));
	*pNowByte += sizeof(float);
	byte += sizeof(float);
	recvbyte += sizeof(float);

	// �}��
	pClient->SetData(&aSendData[0], byte);
}

//==========================================================
// ID���擾
//==========================================================
void CNetWork::CommandNextGoal(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte)
{
	char aRecv[sizeof(int) * 2 + sizeof(int)] = {};
	int command = NetWork::COMMAND_NEXT_GOAL;
	int byte = 0;

	// ID��}��
	memcpy(&aRecv[byte], &nId, sizeof(int));
	byte += sizeof(int);

	// �v���g�R����}��
	memcpy(&aRecv[byte], &command, sizeof(int));
	byte += sizeof(int);

	// �S�[��ID��}��
	memcpy(&aRecv[byte], pRecvData, sizeof(int));
	byte += sizeof(int);
	*pNowByte += sizeof(int);


	// �v���g�R���𑗐M
	pClient->SetData(&aRecv[0], byte);
}