//==========================================================
//
// �v���R���p�C��[precompile.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _PRECOMPILE_H_
#define _PRECOMPILE_H_

// winsock�̐ݒ�
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

// C++��std�p
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

//DirectX�̏����ݒ�
#include <stdio.h>
#include <windows.h>
#include "d3dx9.h"
#define DIRECTINPUT_VERSION (0x0800)	//�r���h���̌x�����Ώ��p�}�N��
#include "dinput.h"
#include "xaudio2.h"					//�T�E���h�����ɕK�v


// �G�t�F�N�V�A
#ifndef _DEBUG
#pragma comment(lib, "Effekseer.lib")
#else
#pragma comment(lib, "Effekseerd.lib")
#endif
#include <Effekseer.h>

#ifndef _DEBUG
#pragma comment(lib, "EffekseerRendererDX9.lib")
#else
#pragma comment(lib, "EffekseerRendererDX9d.lib")
#endif
#include <EffekseerRendererDX9.h>

// ���������[�N�o�͗p�}�N��
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>		// ���������[�N���o�ɕK�v

// �����蔻��
#include "collision.h"

// �֗��֐�
#include "convenience.h"

// crtdbg.h�̃o�O�΍��p�}�N��
#if _DEBUG
#define DEBUG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#else

#define DEBUG_NEW new
#endif

#endif

