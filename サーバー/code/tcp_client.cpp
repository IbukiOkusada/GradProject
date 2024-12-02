//==========================================================
//
// winsock�ʐM(�N���C�A���g) [tcp_client.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include <stdio.h>
#include "tcp_client.h"
#include "tcp_listener.h"
#include <string.h>

//==========================================================
// �R���X�g���N�^
//==========================================================
CClient::CClient()
{
	// �l���N���A����
	m_sock = NULL;
	m_nId = -1;

	m_bSend = false;
	m_nSendByte = 0;
	m_nTempSendByte = 0;
	m_bDeath = false;
}

//==========================================================
// �f�X�g���N�^
//==========================================================
CClient::~CClient()
{

}

//==========================================================
// �\�P�b�g�̊��蓖��
//==========================================================
void CClient::BindSock(SOCKET sock)
{
	m_sock = sock;
}

//==========================================================
// �I������
//==========================================================
void CClient::Uninit(void)
{
	// �ڑ���ؒf����
	closesocket(m_sock);	// �N���C�A���g�Ƃ̐ڑ������
	CListener::SetNum(CListener::GetNum() - 1);
}

//==========================================================
// ���M����
//==========================================================
int CClient::Send(const char *pSendData, int nSendDataSize)
{
	// �f�[�^���M
	int nSendByte = send(m_sock, pSendData, nSendDataSize, 0);	// send�֐�: �f�[�^�𑗐M����

	return nSendByte;
}

//==========================================================
// ��M����
//==========================================================
int CClient::Recv(char *pRecvData, int nRecvDataSize)
{
	int nRecvByte = recv(m_sock, pRecvData, nRecvDataSize, 0);	// recv�֐�: �f�[�^����M����

	return nRecvByte;
}

//==========================================================
// IP�̐ݒ�
//==========================================================
void CClient::SetIP(char *pIp)
{
	memcpy(&m_aClientIP[0], pIp, strlen(pIp));
}

//==========================================================
// ���M�f�[�^�̐ݒ�
//==========================================================
void CClient::SetData(char *pChar, int nByte)
{
	if (!m_bSend)
	{
		if (m_nSendByte + nByte < NetWork::MAX_COMMAND_DATA)
		{
			memcpy(&m_aSendData[m_nSendByte], pChar, nByte);
			m_nSendByte += nByte;
		}
	}
	else
	{
		if (m_nTempSendByte + nByte < NetWork::MAX_COMMAND_DATA)
		{
			// ���ɓ���Ă���
			memcpy(&m_aTempSendData[m_nTempSendByte], pChar, nByte);
			m_nTempSendByte += nByte;
		}
	}
}

//==========================================================
// ���M�f�[�^�̃��Z�b�g
//==========================================================
void CClient::ResetData(void)
{
	memset(&m_aSendData[0], '\0', sizeof(m_aSendData));
	m_nSendByte = 0;

	if (m_nTempSendByte > 0)
	{
		// ���ɓ���Ă���
		memcpy(&m_aSendData[0], &m_aTempSendData[0], m_nTempSendByte);
		m_nSendByte += m_nTempSendByte;
	}

	memset(&m_aTempSendData[0], '\0', sizeof(m_aTempSendData));
	m_nTempSendByte = 0;
}