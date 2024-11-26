//==========================================================
//
// winsock�ʐM(�N���C�A���g) [tcp_client.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _TCP_CLIENT_H_
#define _TCP_CLIENT_H_

#include <winsock2.h>
#include "protocol_online.h"

#define MAX_NAME	(1024)

//==========================================================
// �N���C�A���g�N���X�̒�`
//==========================================================
class CClient
{
public:	// �N�ł��A�N�Z�X�\

	CClient();	// �R���X�g���N�^
	~CClient();	// �f�X�g���N�^

	// �����o�֐�
	void BindSock(SOCKET sock);
	void Uninit(void);
	int Send(const char *pSendData, int nSendDataSize);
	int Recv(char *pRecvData, int nRecvDataSize);
	void SetIP(char *pIp);
	void SetCliePort(int nPort) { m_nClientPort = nPort; }
	char *GetIP(void) { return &m_aClientIP[0]; }
	int GetPort(void) { return m_nClientPort; }
	int GetId(void) { return m_nId; }
	void SetId(int nId) { m_nId = nId;}
	bool GetSend(void) { return m_bSend; }
	void SetSend(bool bSend) { m_bSend = bSend; }
	void SetData(char *pChar, int nByte);
	void ResetData(void);
	int GetSendByte(void) { return m_nSendByte; }
	char *GetSendData(void) { return &m_aSendData[0]; }
	bool GetDeath(void) { return m_bDeath; }
	void SetDeath(bool bDeath) { m_bDeath = bDeath; }

private:	// ���������A�N�Z�X�\

	// �����o�ϐ�
	SOCKET m_sock;	// �\�P�b�g
	char m_aSendData[NetWork::MAX_SEND_DATA] = {};	// ���M����f�[�^
	char m_aTempSendData[NetWork::MAX_SEND_DATA] = {};	// ���M����f�[�^
	char m_aClientIP[64];	// �N���C�A���g��IP�ԍ�
	int m_nClientPort;	// �N���C�A���g�̃|�[�g�ԍ�
	int m_nId;			// ID
	bool m_bSend;		// �f�[�^���M��
	int m_nTempSendByte;	// �f�[�^��
	int m_nSendByte;	// �f�[�^��
	bool m_bDeath;		// �ؒf���Ă��邩�ۂ�
};

#endif