//==========================================================
//
// winsock�ʐM(�T�[�o�[)
// Author : Ibuki Okusada
//
//==========================================================
#include <stdio.h>
#include <winsock2.h>
#include <time.h>
#include <string.h>
#include "tcp_server.h"
#include "tcp_client.h"
#include <thread>
#include <stdlib.h>
#include "protocol_online.h"
#include "d3dx9.h"
#include "network.h"
#include "deltatime.h"

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "winmm.lib")		// �V�X�e�������擾�ɕK�v

// �}�N����`
#define MAX_STRING	(2048)	// �ő啶����
#define DEF_PORT	(22333)	// �|�[�g�ԍ�

// IP���N���X��`
void Access(CClient *pClient);
void Join(int nId, CClient *pClient);
void SetPosition(int nId, D3DXVECTOR3 pos, CClient *pClient);
void SetRotation(int nId, D3DXVECTOR3 pos, CClient *pClient);
void SetDamage(int nId, int nDamage, CClient *pClient);
void SetLife(int nId, int nLife, CClient *pClient);
void SetUp(int nId, CClient *pClient);
void Leave(int nId, CClient *pClient);
void Goal(int nId, CClient *pClient);
void Accept(CServer *pServer);
void Send(CServer **ppServer);

// �O���[�o���ϐ��錾
char g_aSendData[MAX_STRING] = {};	// ���M�f�[�^
int g_nSendByte = 0;	// ���M�f�[�^��

//==========================================================
// ���C���֐�
//==========================================================
int main(void)
{
	CDeltaTime::Create();
	auto net = CNetWork::Create();

	while (1)
	{
		CDeltaTime::GetInstance()->Update();
	}

	net->Release();

	CDeltaTime::GetInstance()->Uninit();

	printf("*** Enter�L�[�������ďI�����Ă������� ***\n");

	// �L�[���͑҂�
	rewind(stdin);
	getchar();

	return S_OK;
}
