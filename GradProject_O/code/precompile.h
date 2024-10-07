//==========================================================
//
// プレコンパイル[precompile.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _PRECOMPILE_H_
#define _PRECOMPILE_H_

// winsockの設定
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

//DirectXの初期設定
#include <stdio.h>
#include <windows.h>
#include "d3dx9.h"
#define DIRECTINPUT_VERSION (0x0800)	//ビルド時の警告文対処用マクロ
#include "dinput.h"
#include "xaudio2.h"					//サウンド処理に必要

#endif

