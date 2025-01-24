//==========================================================
//
// winsock通信(サーバー)
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
#pragma comment(lib, "winmm.lib")		// システム時刻取得に必要

namespace {
	const int IF = 0;
}

// タイマーの情報
struct Timer
{
	std::chrono::high_resolution_clock::time_point CurrentTime;  // 現在の時間
	std::chrono::high_resolution_clock::time_point LastTime;     // 前回の時間
	std::chrono::duration<float> DeltaTime;                      // 差分
};

#if IF
void SetMemCpy();
void SetStr();
void SetStrCpy();
#endif

Timer m_Timer;  // タイマーの情報

//==========================================================
// メイン関数
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

	printf("*** Enterキーを押して終了してください ***\n");

	// キー入力待ち
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
	m_Timer.CurrentTime = std::chrono::high_resolution_clock::now();  // 現在の時間を取得
	m_Timer.LastTime = m_Timer.CurrentTime;                           // 前回の時間に現在の時間を代入

	char str[1024] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
	char cpy[1024] = "";

	memcpy(&cpy[0], &str[0], 1024);

	m_Timer.CurrentTime = std::chrono::high_resolution_clock::now();  // 現在の時間を取得
	m_Timer.DeltaTime = m_Timer.CurrentTime - m_Timer.LastTime;       // 現在の時間と前回の時間の差分を計算
	printf("time %f\n", m_Timer.DeltaTime.count());
}
#endif // !IF

#if IF
//==========================================================
// str
//==========================================================
void SetStr()
{
	m_Timer.CurrentTime = std::chrono::high_resolution_clock::now();  // 現在の時間を取得
	m_Timer.LastTime = m_Timer.CurrentTime;                           // 前回の時間に現在の時間を代入

	std::string str = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
	std::string cpy = "";

	cpy = str;

	m_Timer.CurrentTime = std::chrono::high_resolution_clock::now();  // 現在の時間を取得
	m_Timer.DeltaTime = m_Timer.CurrentTime - m_Timer.LastTime;       // 現在の時間と前回の時間の差分を計算

	printf("time %f\n", m_Timer.DeltaTime.count());
}
#endif

#if IF
//==========================================================
// strcpy
//==========================================================
void SetStrCpy()
{
	m_Timer.CurrentTime = std::chrono::high_resolution_clock::now();  // 現在の時間を取得
	m_Timer.LastTime = m_Timer.CurrentTime;                           // 前回の時間に現在の時間を代入

	char str[1024] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
	char cpy[1024] = "";

	strcpy(&cpy[0], &str[0]);

	m_Timer.CurrentTime = std::chrono::high_resolution_clock::now();  // 現在の時間を取得
	m_Timer.DeltaTime = m_Timer.CurrentTime - m_Timer.LastTime;       // 現在の時間と前回の時間の差分を計算

	printf("time %f\n", m_Timer.DeltaTime.count());
}
#endif