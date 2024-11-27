//==========================================================
//
// winsock�ʐM(�N���C�A���g)
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _TCP_CLIENT_H_
#define _TCP_CLIENT_H_

#include "network.h"

//==========================================================
// �N���C�A���g�N���X�̒�`
//==========================================================
class CClient
{
public:	// �N�ł��A�N�Z�X�\

	CClient();	// �R���X�g���N�^
	~CClient();	// �f�X�g���N�^

	// �����o�֐�
	bool Init(const char *pIPAddress, int nPortNum);
	void BindSock(SOCKET sock);
	void Uninit(void);
	bool Reconnect(const char* pIPAddress, int nPortNum);

	// �ʐM
	int Send();
	int Recv(char *pRecvData, int nRecvDataSize);
	void SetData(const char* pSendData, int nSendDataSize);

	// �����o�֐�(�擾)
	void SetCliePort(int nPort) { m_nClientPort = nPort; }
	const char *GetIP(void) { return m_ClientIP.c_str(); }
	int GetPort(void) { return m_nClientPort; }
	int GetId(void) { return m_nId; }
	int GetByte() { return m_nSendByte; }
	void SetId(int nId) { m_nId = nId; }
	SOCKET GetSock(void) { return m_sock; }

private:	// ���������A�N�Z�X�\

	// �����o�ϐ�
	char m_aSendData[NetWork::MAX_SEND_DATA];
	int m_nSendByte;

	SOCKET m_sock;	// �\�P�b�g
	std::string m_ClientIP;
	int m_nClientPort;
	int m_nId;
};

#endif