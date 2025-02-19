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
#include <mutex>

//DirectXの初期設定
#include <stdio.h>
#include <windows.h>
#include "d3dx9.h"
#define DIRECTINPUT_VERSION (0x0800)	//ビルド時の警告文対処用マクロ
#include "dinput.h"
#include "xaudio2.h"					//サウンド処理に必要

// C++のstd用
#include <vector>
#include <algorithm>
#include <list>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <iterator>
#include <codecvt>
#include <fstream>
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <stack>
#include <filesystem>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "winmm.lib")		// システム時刻取得に必要

// crtdbg.hのバグ対策用マクロ
#if _DEBUG
#define DEBUG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#else

#define DEBUG_NEW new
#endif

#endif

