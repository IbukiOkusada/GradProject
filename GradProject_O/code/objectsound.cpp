//=============================================================================
//
// サウンド処理 [objectsound.cpp]
// Author : 丹野竜之介
//
//=============================================================================
#include "objectsound.h"
CMasterSound* CMasterSound::m_pInstance = nullptr;
//=============================================================================
// コンストラクタ
//=============================================================================
CMasterSound::CObjectSound::CObjectSound()
{
	m_sPath.clear();			//パス
	m_pSourceVoice = nullptr;	// ソースボイス
	m_nCntLoop = 0;				// ループカウント
	m_fVolume = 1.0f;
	m_fPitch = 1.0f;

	m_pDataAudio = nullptr;		// オーディオデータ
	m_dSizeAudio = 0;			// オーディオデータサイズ
	GetInstance()->m_pList->Regist(this);
}
//=============================================================================
// デストラクタ
//=============================================================================
CMasterSound::CObjectSound::~CObjectSound()
{
	GetInstance()->m_pList->Delete(this);
}
//=============================================================================
// 初期化
//=============================================================================
HRESULT CMasterSound::CObjectSound::Init()
{
	HRESULT hr;
	HWND hWnd = GetInstance()->m_hWnd;
	HANDLE hFile;
	DWORD dwChunkSize = 0;
	DWORD dwChunkPosition = 0;
	DWORD dwFiletype;
	WAVEFORMATEXTENSIBLE wfx;
	XAUDIO2_BUFFER buffer;

	// バッファのクリア
	memset(&wfx, 0, sizeof(WAVEFORMATEXTENSIBLE));
	memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));

	// サウンドデータファイルの生成
	hFile = CreateFile(m_sPath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		MessageBox(hWnd, "サウンドデータファイルの生成に失敗！(1)", "警告！", MB_ICONWARNING);
		return HRESULT_FROM_WIN32(GetLastError());
	}
	if (SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{// ファイルポインタを先頭に移動
		MessageBox(hWnd, "サウンドデータファイルの生成に失敗！(2)", "警告！", MB_ICONWARNING);
		return HRESULT_FROM_WIN32(GetLastError());
	}

	// WAVEファイルのチェック
	hr = CheckChunk(hFile, 'FFIR', &dwChunkSize, &dwChunkPosition);
	if (FAILED(hr))
	{
		MessageBox(hWnd, "WAVEファイルのチェックに失敗！(1)", "警告！", MB_ICONWARNING);
		return S_FALSE;
	}
	hr = ReadChunkData(hFile, &dwFiletype, sizeof(DWORD), dwChunkPosition);
	if (FAILED(hr))
	{
		MessageBox(hWnd, "WAVEファイルのチェックに失敗！(2)", "警告！", MB_ICONWARNING);
		return S_FALSE;
	}
	if (dwFiletype != 'EVAW')
	{
		MessageBox(hWnd, "WAVEファイルのチェックに失敗！(3)", "警告！", MB_ICONWARNING);
		return S_FALSE;
	}

	// フォーマットチェック
	hr = CheckChunk(hFile, ' tmf', &dwChunkSize, &dwChunkPosition);
	if (FAILED(hr))
	{
		MessageBox(hWnd, "フォーマットチェックに失敗！(1)", "警告！", MB_ICONWARNING);
		return S_FALSE;
	}
	hr = ReadChunkData(hFile, &wfx, dwChunkSize, dwChunkPosition);
	if (FAILED(hr))
	{
		MessageBox(hWnd, "フォーマットチェックに失敗！(2)", "警告！", MB_ICONWARNING);
		return S_FALSE;
	}

	// オーディオデータ読み込み
	hr = CheckChunk(hFile, 'atad', &m_dSizeAudio, &dwChunkPosition);
	if (FAILED(hr))
	{
		MessageBox(hWnd, "オーディオデータ読み込みに失敗！(1)", "警告！", MB_ICONWARNING);
		return S_FALSE;
	}
	m_pDataAudio = (BYTE*)malloc(m_dSizeAudio);
	hr = ReadChunkData(hFile, m_pDataAudio, m_dSizeAudio, dwChunkPosition);
	if (FAILED(hr))
	{
		MessageBox(hWnd, "オーディオデータ読み込みに失敗！(2)", "警告！", MB_ICONWARNING);
		return S_FALSE;
	}

	// ソースボイスの生成
	hr = GetInstance()->m_pXAudio2->CreateSourceVoice(&m_pSourceVoice, &(wfx.Format));
	if (FAILED(hr))
	{
		MessageBox(hWnd, "ソースボイスの生成に失敗！", "警告！", MB_ICONWARNING);
		return S_FALSE;
	}

	// バッファの値設定
	memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));
	buffer.AudioBytes = m_dSizeAudio;
	buffer.pAudioData = m_pDataAudio;
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	buffer.LoopCount = m_nCntLoop;

	// オーディオバッファの登録
	m_pSourceVoice->SubmitSourceBuffer(&buffer);
	m_buffer = buffer;
	// ファイルをクローズ
	CloseHandle(hFile);
	return S_OK;
}

//=============================================================================
// 終了
//=============================================================================
void CMasterSound::CObjectSound::Uninit()
{

	// 一時停止
	XAUDIO2_VOICE_STATE xa2state;
	// 状態取得
	m_pSourceVoice->GetState(&xa2state);
	if (xa2state.BuffersQueued != 0)
	{// 再生中
		// 一時停止
		m_pSourceVoice->Stop(0);
	}
	// ソースボイスの破棄
	m_pSourceVoice->DestroyVoice();
	m_pSourceVoice = NULL;

	// オーディオデータの開放
	free(m_pDataAudio);
	m_pDataAudio = NULL;
}
//=============================================================================
// 最初から再生
//=============================================================================
void CMasterSound::CObjectSound::Play()
{
	XAUDIO2_VOICE_STATE xa2state;
	// 状態取得
	m_pSourceVoice->GetState(&xa2state);
	if (xa2state.BuffersQueued != 0)
	{// 再生中
		// 一時停止
		m_pSourceVoice->Stop(0);
	}
	// オーディオバッファの削除
	m_pSourceVoice->FlushSourceBuffers();
	// オーディオバッファの登録
	m_pSourceVoice->SubmitSourceBuffer(&m_buffer);

	// 再生
	m_pSourceVoice->Start(0);

}
//=============================================================================
// 停止
//=============================================================================
void CMasterSound::CObjectSound::Stop()
{

	// 一時停止
	m_pSourceVoice->Stop(0);
	
	// オーディオバッファの削除
	m_pSourceVoice->FlushSourceBuffers();
	// オーディオバッファの登録
	m_pSourceVoice->SubmitSourceBuffer(&m_buffer);

}
//=============================================================================
// 一時停止
//=============================================================================
void CMasterSound::CObjectSound::Pause()
{
	//一時停止
	m_pSourceVoice->Start(0);
}
//=============================================================================
// 再生
//=============================================================================
void CMasterSound::CObjectSound::Start()
{
	// 再生
	m_pSourceVoice->Start(0);
}
//=============================================================================
// ボリューム設定
//=============================================================================
void CMasterSound::CObjectSound::SetVolume(float fVolume)
{
	m_fVolume = fVolume;
	m_pSourceVoice->SetVolume(fVolume);
}
//=============================================================================
// ピッチ設定
//=============================================================================
void CMasterSound::CObjectSound::SetPitch(float fPitch)
{
	m_fPitch = fPitch;
	m_pSourceVoice->SetFrequencyRatio(fPitch);
}
//=============================================================================
// 生成
//=============================================================================
CMasterSound::CObjectSound* CMasterSound::CObjectSound::Create(std::string sPath, int nLoop)
{
	CObjectSound* pSound = DEBUG_NEW CObjectSound;
	pSound->m_sPath = sPath;
	pSound->m_nCntLoop = nLoop;
	pSound->Init();
	
	pSound->Play();
	return pSound;
}
//=============================================================================
// チャンクのチェック
//=============================================================================
HRESULT CMasterSound::CObjectSound::CheckChunk(HANDLE hFile, DWORD format, DWORD* pChunkSize, DWORD* pChunkDataPosition)
{
	HRESULT hr = S_OK;
	DWORD dwRead;
	DWORD dwChunkType;
	DWORD dwChunkDataSize;
	DWORD dwRIFFDataSize = 0;
	DWORD dwFileType;
	DWORD dwBytesRead = 0;
	DWORD dwOffset = 0;

	if (SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{// ファイルポインタを先頭に移動
		return HRESULT_FROM_WIN32(GetLastError());
	}

	while (hr == S_OK)
	{
		if (ReadFile(hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL) == 0)
		{// チャンクの読み込み
			hr = HRESULT_FROM_WIN32(GetLastError());
		}

		if (ReadFile(hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL) == 0)
		{// チャンクデータの読み込み
			hr = HRESULT_FROM_WIN32(GetLastError());
		}

		switch (dwChunkType)
		{
		case 'FFIR':
			dwRIFFDataSize = dwChunkDataSize;
			dwChunkDataSize = 4;
			if (ReadFile(hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL) == 0)
			{// ファイルタイプの読み込み
				hr = HRESULT_FROM_WIN32(GetLastError());
			}
			break;

		default:
			if (SetFilePointer(hFile, dwChunkDataSize, NULL, FILE_CURRENT) == INVALID_SET_FILE_POINTER)
			{// ファイルポインタをチャンクデータ分移動
				return HRESULT_FROM_WIN32(GetLastError());
			}
		}

		dwOffset += sizeof(DWORD) * 2;
		if (dwChunkType == format)
		{
			*pChunkSize = dwChunkDataSize;
			*pChunkDataPosition = dwOffset;

			return S_OK;
		}

		dwOffset += dwChunkDataSize;
		if (dwBytesRead >= dwRIFFDataSize)
		{
			return S_FALSE;
		}
	}

	return S_OK;
}

//=============================================================================
// チャンクデータの読み込み
//=============================================================================
HRESULT CMasterSound::CObjectSound::ReadChunkData(HANDLE hFile, void* pBuffer, DWORD dwBuffersize, DWORD dwBufferoffset)
{
	DWORD dwRead;

	if (SetFilePointer(hFile, dwBufferoffset, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{// ファイルポインタを指定位置まで移動
		return HRESULT_FROM_WIN32(GetLastError());
	}

	if (ReadFile(hFile, pBuffer, dwBuffersize, &dwRead, NULL) == 0)
	{// データの読み込み
		return HRESULT_FROM_WIN32(GetLastError());
	}

	return S_OK;
}

//=============================================================================
// コンストラクタ
//=============================================================================
CMasterSound::CMasterSound()
{
	m_pXAudio2 = nullptr;
	m_pMasteringVoice = nullptr;
}
//=============================================================================
// デストラクタ
//=============================================================================
CMasterSound::~CMasterSound()
{
}
//=============================================================================
// 初期化
//=============================================================================
HRESULT CMasterSound::Init(HWND hWnd)
{
	HRESULT hr;

	// COMライブラリの初期化
	hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);

	if (FAILED(hr))
	{
		MessageBox(hWnd, "COMライブラリの初期化に失敗！", "警告！", MB_ICONWARNING);

		return E_FAIL;
	}

	// XAudio2オブジェクトの作成
	hr = XAudio2Create(&m_pXAudio2, 0);
	if (FAILED(hr))
	{
		MessageBox(hWnd, "XAudio2オブジェクトの作成に失敗！", "警告！", MB_ICONWARNING);

		// COMライブラリの終了処理
		CoUninitialize();

		return E_FAIL;
	}

	// マスターボイスの生成
	hr = m_pXAudio2->CreateMasteringVoice(&m_pMasteringVoice);
	if (FAILED(hr))
	{
		MessageBox(hWnd, "マスターボイスの生成に失敗！", "警告！", MB_ICONWARNING);

		if (m_pXAudio2 != NULL)
		{
			// XAudio2オブジェクトの開放
			m_pXAudio2->Release();
			m_pXAudio2 = NULL;
		}

		// COMライブラリの終了処理
		CoUninitialize();

		return E_FAIL;
	}
	m_hWnd = hWnd;
	m_pList = DEBUG_NEW Clist<CObjectSound*>;
	return S_OK;
}
//=============================================================================
//	終了
//=============================================================================
void CMasterSound::Uninit()
{
	SAFE_DELETE(m_pList);
	// マスターボイスの破棄
	m_pMasteringVoice->DestroyVoice();
	m_pMasteringVoice = NULL;

	if (m_pXAudio2 != NULL)
	{
		// XAudio2オブジェクトの開放
		m_pXAudio2->Release();
		m_pXAudio2 = NULL;
	}

	// COMライブラリの終了処理
	CoUninitialize();
}
//=============================================================================
// 一時停止
//=============================================================================
void CMasterSound::Pause()
{
	int nSize = m_pList->GetNum();
	for (int i = 0; i < nSize; i++)
	{
		m_pList->Get(i)->Pause();
	}
}
//=============================================================================
// 再生
//=============================================================================
void CMasterSound::Start()
{
	int nSize = m_pList->GetNum();
	for (int i = 0; i < nSize; i++)
	{
		m_pList->Get(i)->Start();
	}
}