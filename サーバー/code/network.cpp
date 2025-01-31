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
	&CNetWork::CommandNone,				// �����Ȃ�
	&CNetWork::CommandJoin,				// �ڑ�����
	&CNetWork::CommandGetId,			// ID�擾
	&CNetWork::CommandDelete,			// �폜
	&CNetWork::CommandPlPos,			// �v���C���[�ʒu
	&CNetWork::CommandPlDamage,			// �v���C���[�_���\�W
	&CNetWork::CommandPlGoal,			// �v���C���[�S�[��
	&CNetWork::CommandGmHit,			// �M�~�b�N�Փ�
	&CNetWork::CommandNextGoal,			// ���̃S�[��
	&CNetWork::CommandGameStartOk,		// �Q�[���J�n�\�ɂȂ�����
	&CNetWork::CommandGameStart,		// �Q�[���J�n
	&CNetWork::CommandTutorialOk,		// �Q�[���J�n�\�ɂȂ�����
	&CNetWork::CommandTutorialNo,		// �Q�[���J�n�\�L�����Z��
	&CNetWork::CommandTutorialEnd,		// �Q�[���J�n
	&CNetWork::CommandSetInspection,	// ����z�u
	&CNetWork::CommandEndInspection,	// ����p��
	&CNetWork::CommandCarPos,			// �Ԉʒu
	&CNetWork::CommandPdPos,			// �x�@�ʒu
	&CNetWork::CommandAddPdPos,			// �ǉ��x�@�ʒu
	&CNetWork::CommandPdChase,			// �x�@�ʒu
	&CNetWork::CommandAddPdChase,		// �ǉ��x�@�ʒu
	&CNetWork::CommandPdChaseEnd,		// �x�@�ʒu
	&CNetWork::CommandAddPdChaseEnd,	// �ǉ��x�@�ʒu
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
	m_nConnectCnt = 0;
	m_pServer = nullptr;
	m_ServerFlag = SFlagInfo();

	memset(m_aSendData, NULL, sizeof(m_aSendData));

	for (int i = 0; i < NetWork::MAX_CONNECT; i++)
	{
		m_apClient[i] = nullptr;
		m_aFlag[i] = SFlagInfo();
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

			int id = i;	// �ڑ���id

			//// �擪����l�߂�
			//for (int j = 0; j < NetWork::MAX_CONNECT; j++)
			//{
			//	if (m_apClient[j] != nullptr) { continue; }

			//	id = j;

			//	break;
			//}

			// ID�ݒ�
			pClient->SetId(id);

			// �}���`�X���b�h
			std::thread th(&CNetWork::Access, this, pClient);
			th.detach();

			// �t���O������
			m_aFlag[id] = SFlagInfo();

			// ���g���o����
			m_apClient[id] = pClient;

			m_nConnectCnt++;
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

				CClient* pClient = m_apClient[i];	// �擪���擾
				if (pClient == nullptr) { continue; }

				// �S�����̃f�[�^���ꊇ�ɂ���
				pClient->SetSend(true);	// ���������s�\�ȏ�Ԃɂ���
				
				int sendbyte = pClient->GetSendByte();

				// �e�ʂ͈͓̔�
				if (m_nSendByte + sendbyte < NetWork::MAX_SEND_DATA)
				{
					memcpy(&m_aSendData[m_nSendByte], pClient->GetSendData(), pClient->GetSendByte());
					m_nSendByte += pClient->GetSendByte();
				}
				else
				{
					// �����ꍇ���̕��ɓ����
					pClient->SetData(pClient->GetSendData(), pClient->GetSendByte());
				}

				// �ؒf����Ă��邩�m�F
				if (pClient->GetDeath() == false)
				{
					// ����Ă��Ȃ�
					pClient->ResetData();		// �f�[�^�̃��Z�b�g
					pClient->SetSend(false);	// ���������\�ȏ�Ԃɂ���
				}
				else
				{
					// ����Ă���
					if (pClient != nullptr)
					{
						pClient->ResetData();		// �f�[�^�̃��Z�b�g
						pClient->Uninit();
						delete pClient;
						pClient = nullptr;
						m_apClient[i] = nullptr;
						m_nConnectCnt--;
						printf("�N���C�A���g�ؒf\n");
						printf("�u�]���ď�������\n");
					}
				}
			}

			// ���M����f�[�^������
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
						// ���s�����ꍇ
						pClient->Uninit();
						delete pClient;
						pClient = nullptr;
						m_apClient[i] = nullptr;

						m_nConnectCnt--;
						printf("�N���C�A���g�ؒf\n");
						printf("���M�ł����ɏ�������\n");
					}
				}

				printf("%d �o�C�g��������\n", m_nSendByte);
			}

			// ���M�f�[�^���N���A
			std::fill(std::begin(m_aSendData), std::end(m_aSendData), '\0');
			m_nSendByte = 0;

			// �t���O�����Z�b�g
			if (m_ServerFlag.bStart) { m_ServerFlag.bStart = false; }
			if (m_ServerFlag.bTutorial) { m_ServerFlag.bTutorial = false; }
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

	printf("ID %d \n", pClient->GetId());

	// �ݒ�
	CommandJoin(pClient->GetId(), nullptr, pClient, &nowbyte);

	while (1)
	{
		char recvdata[NetWork::MAX_SEND_DATA] = {};	// ��M�p
		int command = NetWork::COMMAND_NONE;

		// ��M
		int recvbyte = pClient->Recv(&recvdata[0], NetWork::MAX_SEND_DATA);
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

			if (command > 0 && command <= NetWork::COMMAND::COMMAND_MAX)
			{

				// �R�}���h�����s
				(this->*(m_CommandFunc[command]))(id, &recvdata[nowbyte], pClient, &nowbyte);
			}

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

	printf("deletecommand�ŏ�������\n");
}

//==========================================================
// �v���C���[�̍��W
//==========================================================
void CNetWork::CommandPlPos(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte)
{
	int nProt = -1;	// �v���g�R���ԍ�
	char aSendData[sizeof(int) * 2 + sizeof(D3DXVECTOR3) + sizeof(float)] = {};	// ���M�p�܂Ƃ߃f�[�^
	int byte = 0;
	int recvbyte = 0;

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
	recvbyte += sizeof(D3DXVECTOR3);

	// �����}��
	memcpy(&aSendData[byte], &pRecvData[recvbyte], sizeof(float));
	*pNowByte += sizeof(float);
	byte += sizeof(float);

	// �}��
	pClient->SetData(&aSendData[0], byte);
}

//==========================================================
// �v���C���[�̃_���[�W��̗�
//==========================================================
void CNetWork::CommandPlDamage(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte)
{
	int nProt = -1;	// �v���g�R���ԍ�
	char aSendData[sizeof(int) * 2 + sizeof(float)] = {};	// ���M�p�܂Ƃ߃f�[�^
	int byte = 0;

	nProt = NetWork::COMMAND_PL_DAMAGE;

	// ID��}��
	memcpy(&aSendData[byte], &nId, sizeof(int));
	byte += sizeof(int);

	// �v���g�R���}��
	memcpy(&aSendData[byte], &nProt, sizeof(int));
	byte += sizeof(int);

	// �̗͑}��
	memcpy(&aSendData[byte], pRecvData, sizeof(float));
	*pNowByte += sizeof(float);
	byte += sizeof(float);

	// �}��
	pClient->SetData(&aSendData[0], byte);
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

//==========================================================
// �Q�[���J�n��������
//==========================================================
void CNetWork::CommandGameStartOk(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte)
{
	// �t���O�I��
	m_aFlag[nId].bStart = true;

	CommandGameStart(nId, pRecvData, pClient, pNowByte);
}

//==========================================================
// ID���擾
//==========================================================
void CNetWork::CommandGameStart(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte)
{
	if (m_ServerFlag.bStart) { return; }

	int cnt = 0;

	// �t���O�m�F
	for (int i = 0; i < NetWork::MAX_CONNECT; i++)
	{
		if (m_apClient[i] != nullptr && m_aFlag[i].bStart)
		{
			cnt++;
		}
	}

	// ������葽��
	if (cnt >= m_nConnectCnt)
	{
		m_ServerFlag.bStart = true;

		// ���Z�b�g
		for (int i = 0; i < NetWork::MAX_CONNECT; i++)
		{
			m_aFlag[i].bStart = false;
		}
	}
	else
	{
		return;
	}

	char aRecv[sizeof(int) * 2] = {};
	int command = NetWork::COMMAND_GAMESTART;
	int byte = 0;

	// ID��}��
	memcpy(&aRecv[byte], &nId, sizeof(int));
	byte += sizeof(int);

	// �v���g�R����}��
	memcpy(&aRecv[byte], &command, sizeof(int));
	byte += sizeof(int);

	// �v���g�R���𑗐M
	pClient->SetData(&aRecv[0], byte);
}

//==========================================================
// �`���[�g���A��OK
//==========================================================
void CNetWork::CommandTutorialOk(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte)
{
	// �t���O�I��
	m_aFlag[nId].bTutorial = true;

	char aRecv[sizeof(int) * 2] = {};
	int command = NetWork::COMMAND_TUTORIAL_OK;
	int byte = 0;

	// ID��}��
	memcpy(&aRecv[byte], &nId, sizeof(int));
	byte += sizeof(int);

	// �v���g�R����}��
	memcpy(&aRecv[byte], &command, sizeof(int));
	byte += sizeof(int);

	// �v���g�R���𑗐M
	pClient->SetData(&aRecv[0], byte);

	// �I���m�F
	CommandTutorialEnd(nId, pRecvData, pClient, pNowByte);
}

//==========================================================
// �`���[�g���A��OK
//==========================================================
void CNetWork::CommandTutorialNo(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte)
{
	// �t���O�I�t
	m_aFlag[nId].bTutorial = false;

	char aRecv[sizeof(int) * 2] = {};
	int command = NetWork::COMMAND_TUTORIAL_NO;
	int byte = 0;

	// ID��}��
	memcpy(&aRecv[byte], &nId, sizeof(int));
	byte += sizeof(int);

	// �v���g�R����}��
	memcpy(&aRecv[byte], &command, sizeof(int));
	byte += sizeof(int);

	// �v���g�R���𑗐M
	pClient->SetData(&aRecv[0], byte);
}

//==========================================================
// �`���[�g���A���I��
//==========================================================
void CNetWork::CommandTutorialEnd(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte)
{
	if (m_ServerFlag.bTutorial) { return; }

	int cnt = 0;

	// �t���O�m�F
	for (int i = 0; i < NetWork::MAX_CONNECT; i++)
	{
		if (m_apClient[i] != nullptr && m_aFlag[i].bTutorial)
		{
			cnt++;
		}
	}

	// ������葽��
	if (cnt >= m_nConnectCnt)
	{
		m_ServerFlag.bTutorial = true;

		// ���Z�b�g
		for (int i = 0; i < NetWork::MAX_CONNECT; i++)
		{
			m_aFlag[i].bTutorial = false;
		}
	}
	else
	{
		return;
	}

	char aRecv[sizeof(int) * 2] = {};
	int command = NetWork::COMMAND_TUTORIAL_END;
	int byte = 0;

	// ID��}��
	memcpy(&aRecv[byte], &nId, sizeof(int));
	byte += sizeof(int);

	// �v���g�R����}��
	memcpy(&aRecv[byte], &command, sizeof(int));
	byte += sizeof(int);

	// �v���g�R���𑗐M
	pClient->SetData(&aRecv[0], byte);
}

//==========================================================
// ����ݒu
//==========================================================
void CNetWork::CommandSetInspection(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte)
{
	char aRecv[sizeof(int) * 2 + sizeof(int) + sizeof(D3DXVECTOR3) + sizeof(D3DXVECTOR3) + sizeof(int) + sizeof(int)] = {};
	int command = NetWork::COMMAND_SET_INSP;
	int byte = 0;
	int recvbyte = 0;

	// ID��}��
	memcpy(&aRecv[byte], &nId, sizeof(int));
	byte += sizeof(int);

	// �v���g�R����}��
	memcpy(&aRecv[byte], &command, sizeof(int));
	byte += sizeof(int);

	// ����ID��}��
	memcpy(&aRecv[byte], &pRecvData[recvbyte], sizeof(int));
	byte += sizeof(int);
	*pNowByte += sizeof(int);
	recvbyte += sizeof(int);

	// ���W�}��
	memcpy(&aRecv[byte], &pRecvData[recvbyte], sizeof(D3DXVECTOR3));
	*pNowByte += sizeof(D3DXVECTOR3);
	byte += sizeof(D3DXVECTOR3);
	recvbyte += sizeof(D3DXVECTOR3);

	// �����}��
	memcpy(&aRecv[byte], &pRecvData[recvbyte], sizeof(D3DXVECTOR3));
	*pNowByte += sizeof(D3DXVECTOR3);
	byte += sizeof(D3DXVECTOR3);
	recvbyte += sizeof(D3DXVECTOR3);

	// ��ID��}��
	memcpy(&aRecv[byte], &pRecvData[recvbyte], sizeof(int));
	byte += sizeof(int);
	*pNowByte += sizeof(int);
	recvbyte += sizeof(int);

	// �J�nID��}��
	memcpy(&aRecv[byte], &pRecvData[recvbyte], sizeof(int));
	byte += sizeof(int);
	*pNowByte += sizeof(int);
	recvbyte += sizeof(int);

	// �v���g�R���𑗐M
	pClient->SetData(&aRecv[0], byte);
}

//==========================================================
// ����p��
//==========================================================
void CNetWork::CommandEndInspection(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte)
{
	char aRecv[sizeof(int) * 2 + sizeof(int)] = {};
	int command = NetWork::COMMAND_END_INSP;
	int byte = 0;

	// ID��}��
	memcpy(&aRecv[byte], &nId, sizeof(int));
	byte += sizeof(int);

	// �v���g�R����}��
	memcpy(&aRecv[byte], &command, sizeof(int));
	byte += sizeof(int);

	// ����ID��}��
	memcpy(&aRecv[byte], pRecvData, sizeof(int));
	byte += sizeof(int);
	*pNowByte += sizeof(int);

	// �v���g�R���𑗐M
	pClient->SetData(&aRecv[0], byte);
}

//==========================================================
// �Ԃ̍��W
//==========================================================
void CNetWork::CommandCarPos(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte)
{
	int nProt = -1;	// �v���g�R���ԍ�
	char aSendData[sizeof(int) * 2 + sizeof(int) + sizeof(D3DXVECTOR3) + sizeof(float)] = {};	// ���M�p�܂Ƃ߃f�[�^
	int byte = 0;
	int recvbyte = 0;

	nProt = NetWork::COMMAND_CAR_POS;

	// ID��}��
	memcpy(&aSendData[byte], &nId, sizeof(int));
	byte += sizeof(int);

	// �v���g�R����}��
	memcpy(&aSendData[byte], &nProt, sizeof(int));
	byte += sizeof(int);

	// �Ԃ�ID�}��
	memcpy(&aSendData[byte], &pRecvData[recvbyte], sizeof(int));
	*pNowByte += sizeof(int);
	byte += sizeof(int);
	recvbyte += sizeof(int);

	// ���W�}��
	memcpy(&aSendData[byte], &pRecvData[recvbyte], sizeof(D3DXVECTOR3));
	*pNowByte += sizeof(D3DXVECTOR3);
	byte += sizeof(D3DXVECTOR3);
	recvbyte += sizeof(D3DXVECTOR3);

	// �����}��
	memcpy(&aSendData[byte], &pRecvData[recvbyte], sizeof(float));
	*pNowByte += sizeof(float);
	byte += sizeof(float);
	recvbyte += sizeof(float);

	// �}��
	pClient->SetData(&aSendData[0], byte);
}

//==========================================================
// �x�@�̍��W
//==========================================================
void CNetWork::CommandPdPos(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte)
{
	int nProt = -1;	// �v���g�R���ԍ�
	char aSendData[sizeof(int) * 2 + sizeof(int) + sizeof(D3DXVECTOR3) + sizeof(float)] = {};	// ���M�p�܂Ƃ߃f�[�^
	int byte = 0;
	int recvbyte = 0;

	nProt = NetWork::COMMAND_PD_POS;

	// ID��}��
	memcpy(&aSendData[byte], &nId, sizeof(int));
	byte += sizeof(int);

	// �v���g�R����}��
	memcpy(&aSendData[byte], &nProt, sizeof(int));
	byte += sizeof(int);

	// �Ԃ�ID�}��
	memcpy(&aSendData[byte], &pRecvData[recvbyte], sizeof(int));
	*pNowByte += sizeof(int);
	byte += sizeof(int);
	recvbyte += sizeof(int);

	// ���W�}��
	memcpy(&aSendData[byte], &pRecvData[recvbyte], sizeof(D3DXVECTOR3));
	*pNowByte += sizeof(D3DXVECTOR3);
	byte += sizeof(D3DXVECTOR3);
	recvbyte += sizeof(D3DXVECTOR3);

	// �����}��
	memcpy(&aSendData[byte], &pRecvData[recvbyte], sizeof(float));
	*pNowByte += sizeof(float);
	byte += sizeof(float);
	recvbyte += sizeof(float);

	// �}��
	pClient->SetData(&aSendData[0], byte);
}

//==========================================================
// �ǉ��x�@�̍��W
//==========================================================
void CNetWork::CommandAddPdPos(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte)
{
	int nProt = -1;	// �v���g�R���ԍ�
	char aSendData[sizeof(int) * 2 + sizeof(int) + sizeof(D3DXVECTOR3) + sizeof(float)] = {};	// ���M�p�܂Ƃ߃f�[�^
	int byte = 0;
	int recvbyte = 0;

	nProt = NetWork::COMMAND_ADDPD_POS;

	// ID��}��
	memcpy(&aSendData[byte], &nId, sizeof(int));
	byte += sizeof(int);

	// �v���g�R����}��
	memcpy(&aSendData[byte], &nProt, sizeof(int));
	byte += sizeof(int);

	// �Ԃ�ID�}��
	memcpy(&aSendData[byte], &pRecvData[recvbyte], sizeof(int));
	*pNowByte += sizeof(int);
	byte += sizeof(int);
	recvbyte += sizeof(int);

	// ���W�}��
	memcpy(&aSendData[byte], &pRecvData[recvbyte], sizeof(D3DXVECTOR3));
	*pNowByte += sizeof(D3DXVECTOR3);
	byte += sizeof(D3DXVECTOR3);
	recvbyte += sizeof(D3DXVECTOR3);

	// �����}��
	memcpy(&aSendData[byte], &pRecvData[recvbyte], sizeof(float));
	*pNowByte += sizeof(float);
	byte += sizeof(float);
	recvbyte += sizeof(float);

	// �}��
	pClient->SetData(&aSendData[0], byte);
}

//==========================================================
// �x�@�̒ǐՊJ�n
//==========================================================
void CNetWork::CommandPdChase(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte)
{
	int nProt = -1;	// �v���g�R���ԍ�
	char aSendData[sizeof(int) * 2 + sizeof(int) + sizeof(int)] = {};	// ���M�p�܂Ƃ߃f�[�^
	int byte = 0;
	int recvbyte = 0;

	nProt = NetWork::COMMAND_PD_CHASE;

	// ID��}��
	memcpy(&aSendData[byte], &nId, sizeof(int));
	byte += sizeof(int);

	// �v���g�R����}��
	memcpy(&aSendData[byte], &nProt, sizeof(int));
	byte += sizeof(int);

	// �Ԃ�ID�}��
	memcpy(&aSendData[byte], &pRecvData[recvbyte], sizeof(int));
	*pNowByte += sizeof(int);
	byte += sizeof(int);
	recvbyte += sizeof(int);

	// �v���C���[��ID�}��
	memcpy(&aSendData[byte], &pRecvData[recvbyte], sizeof(int));
	*pNowByte += sizeof(int);
	byte += sizeof(int);
	recvbyte += sizeof(int);

	// �}��
	pClient->SetData(&aSendData[0], byte);
}

//==========================================================
// �ǉ��x�@�̒ǐՊJ�n
//==========================================================
void CNetWork::CommandAddPdChase(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte)
{
	int nProt = -1;	// �v���g�R���ԍ�
	char aSendData[sizeof(int) * 2 + sizeof(int) + sizeof(int)] = {};	// ���M�p�܂Ƃ߃f�[�^
	int byte = 0;
	int recvbyte = 0;

	nProt = NetWork::COMMAND_ADDPD_CHASE;

	// ID��}��
	memcpy(&aSendData[byte], &nId, sizeof(int));
	byte += sizeof(int);

	// �v���g�R����}��
	memcpy(&aSendData[byte], &nProt, sizeof(int));
	byte += sizeof(int);

	// �Ԃ�ID�}��
	memcpy(&aSendData[byte], &pRecvData[recvbyte], sizeof(int));
	*pNowByte += sizeof(int);
	byte += sizeof(int);
	recvbyte += sizeof(int);

	// �v���C���[��ID�}��
	memcpy(&aSendData[byte], &pRecvData[recvbyte], sizeof(int));
	*pNowByte += sizeof(int);
	byte += sizeof(int);
	recvbyte += sizeof(int);

	// �}��
	pClient->SetData(&aSendData[0], byte);
}

//==========================================================
// �x�@�̒ǐՏI��
//==========================================================
void CNetWork::CommandPdChaseEnd(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte)
{
	int nProt = -1;	// �v���g�R���ԍ�
	char aSendData[sizeof(int) * 2 + sizeof(int)] = {};	// ���M�p�܂Ƃ߃f�[�^
	int byte = 0;
	int recvbyte = 0;

	nProt = NetWork::COMMAND_PD_CHASEEND;

	// ID��}��
	memcpy(&aSendData[byte], &nId, sizeof(int));
	byte += sizeof(int);

	// �v���g�R����}��
	memcpy(&aSendData[byte], &nProt, sizeof(int));
	byte += sizeof(int);

	// �Ԃ�ID�}��
	memcpy(&aSendData[byte], &pRecvData[recvbyte], sizeof(int));
	*pNowByte += sizeof(int);
	byte += sizeof(int);
	recvbyte += sizeof(int);

	// �}��
	pClient->SetData(&aSendData[0], byte);
}

//==========================================================
// �ǐՌx�@�̒ǐՏI��
//==========================================================
void CNetWork::CommandAddPdChaseEnd(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte)
{
	int nProt = -1;	// �v���g�R���ԍ�
	char aSendData[sizeof(int) * 2 + sizeof(int)] = {};	// ���M�p�܂Ƃ߃f�[�^
	int byte = 0;
	int recvbyte = 0;

	nProt = NetWork::COMMAND_ADDPD_CHASEEND;

	// ID��}��
	memcpy(&aSendData[byte], &nId, sizeof(int));
	byte += sizeof(int);

	// �v���g�R����}��
	memcpy(&aSendData[byte], &nProt, sizeof(int));
	byte += sizeof(int);

	// �Ԃ�ID�}��
	memcpy(&aSendData[byte], &pRecvData[recvbyte], sizeof(int));
	*pNowByte += sizeof(int);
	byte += sizeof(int);
	recvbyte += sizeof(int);

	// �}��
	pClient->SetData(&aSendData[0], byte);
}