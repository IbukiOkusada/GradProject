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

namespace {
	const int IF = 0;
}

// �^�C�}�[�̏��
struct Timer
{
	std::chrono::high_resolution_clock::time_point CurrentTime;  // ���݂̎���
	std::chrono::high_resolution_clock::time_point LastTime;     // �O��̎���
	std::chrono::duration<float> DeltaTime;                      // ����
};

#if IF
void SetMemCpy();
void SetStr();
void SetStrCpy();
#endif

Timer m_Timer;  // �^�C�}�[�̏��

//==========================================================
// ���C���֐�
//==========================================================
int main(void)
{
#if IF
	
	SetMemCpy();

	SetStr();

	SetStrCpy();

#else
	CDeltaTime::Create();
	auto net = CNetWork::Create();

	while (1)
	{
		CDeltaTime::GetInstance()->Update();
	}

	net->Release();

	CDeltaTime::GetInstance()->Uninit();
#endif

	printf("*** Enter�L�[�������ďI�����Ă������� ***\n");

	// �L�[���͑҂�
	rewind(stdin);
	getchar();

	return S_OK;
}

#if IF
//==========================================================
// memcpy
//==========================================================
void SetMemCpy()
{
	m_Timer.CurrentTime = std::chrono::high_resolution_clock::now();  // ���݂̎��Ԃ��擾
	m_Timer.LastTime = m_Timer.CurrentTime;                           // �O��̎��ԂɌ��݂̎��Ԃ���

	char str[1024] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
	char cpy[1024] = "";

	memcpy(&cpy[0], &str[0], 1024);

	m_Timer.CurrentTime = std::chrono::high_resolution_clock::now();  // ���݂̎��Ԃ��擾
	m_Timer.DeltaTime = m_Timer.CurrentTime - m_Timer.LastTime;       // ���݂̎��ԂƑO��̎��Ԃ̍������v�Z
	printf("time %f\n", m_Timer.DeltaTime.count());
}
#endif // !IF

#if IF
//==========================================================
// str
//==========================================================
void SetStr()
{
	m_Timer.CurrentTime = std::chrono::high_resolution_clock::now();  // ���݂̎��Ԃ��擾
	m_Timer.LastTime = m_Timer.CurrentTime;                           // �O��̎��ԂɌ��݂̎��Ԃ���

	std::string str = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
	std::string cpy = "";

	cpy = str;

	m_Timer.CurrentTime = std::chrono::high_resolution_clock::now();  // ���݂̎��Ԃ��擾
	m_Timer.DeltaTime = m_Timer.CurrentTime - m_Timer.LastTime;       // ���݂̎��ԂƑO��̎��Ԃ̍������v�Z

	printf("time %f\n", m_Timer.DeltaTime.count());
}
#endif

#if IF
//==========================================================
// strcpy
//==========================================================
void SetStrCpy()
{
	m_Timer.CurrentTime = std::chrono::high_resolution_clock::now();  // ���݂̎��Ԃ��擾
	m_Timer.LastTime = m_Timer.CurrentTime;                           // �O��̎��ԂɌ��݂̎��Ԃ���

	char str[1024] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
	char cpy[1024] = "";

	strcpy(&cpy[0], &str[0]);

	m_Timer.CurrentTime = std::chrono::high_resolution_clock::now();  // ���݂̎��Ԃ��擾
	m_Timer.DeltaTime = m_Timer.CurrentTime - m_Timer.LastTime;       // ���݂̎��ԂƑO��̎��Ԃ̍������v�Z

	printf("time %f\n", m_Timer.DeltaTime.count());
}
#endif