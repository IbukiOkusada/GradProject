//=============================================================================
//
// サウンド処理 [sound.h]
// Author : Ibuki Okusada
//
//=============================================================================
#ifndef _OBJECTSOUND_H_
#define _OBJECTSOUND_H_

#include "main.h"
#include "convenience.h"
#include "list.h"
//=============================================================================
// サウンドクラスの定義
//=============================================================================
class CMasterSound
{
public:	// 誰でもアクセス可能な定義

	// 種類列挙
	typedef enum
	{
		LABEL_BGM_TITLE = 0,		// BGMタイトル
		//LABEL_SE_ENTER,			// PRESSENTER音
		LABEL_MAX
	} LABEL;
	class CObjectSound
	{
	public:
		CObjectSound();
		~CObjectSound();
		// メンバ関数
		HRESULT Init();
		void Uninit();
		void Play();
		void Stop();
		void Pause();
		void Start();
		bool IsStart() { return m_bStart; }
	
		HRESULT Load(std::string sPath);
		
		float GetVolume() { return m_fVolume; }
		void SetVolume(float fVolume/*初期値は1.0f*/);
		float GetPitch() { return m_fPitch; }
		void SetPitch(float fPitch/*初期値は1.0f*/);
		bool GetPlay() {
			XAUDIO2_VOICE_STATE state;
			m_pSourceVoice->GetState(&state);

			// BuffersQueued が0の場合、再生が終了している
			return (state.BuffersQueued > 0);
		}

		static CObjectSound* Create(std::string sPath,int nLoop);
	private:
		std::string m_sPath;	//パス
		IXAudio2SourceVoice* m_pSourceVoice;	// ソースボイス
		int m_nCntLoop;		// ループカウント
		float m_fVolume;
		float m_fPitch;
		BYTE* m_pDataAudio;					// オーディオデータ
		DWORD m_dSizeAudio;					// オーディオデータサイズ
		XAUDIO2_BUFFER m_buffer;
		bool m_bStart;
		// メンバ関数
		HRESULT CheckChunk(HANDLE hFile, DWORD format, DWORD* pChunkSize, DWORD* pChunkDataPosition);
		HRESULT ReadChunkData(HANDLE hFile, void* pBuffer, DWORD dwBuffersize, DWORD dwBufferoffset);
	};

public:	// 誰でもアクセス可能

	CMasterSound();	// コンストラクタ
	~CMasterSound();	// デストラクタ

	// メンバ関数
	HRESULT Init(HWND hWnd);
	void Uninit();
	void Pause();
	void Start();

	static CMasterSound* GetInstance() { if (m_pInstance == nullptr) { m_pInstance = DEBUG_NEW CMasterSound;}return m_pInstance; }
	static void Release() { if (m_pInstance != nullptr) { delete m_pInstance; } }
private:	// 自分だけがアクセス可能
	// メンバ変数
	static CMasterSound* m_pInstance;
	IXAudio2* m_pXAudio2;							// XAudio2オブジェクトへのインターフェイス
	IXAudio2MasteringVoice* m_pMasteringVoice;		// マスターボイス
	
protected:
	
	// メンバ変数
	HWND m_hWnd;
	Clist<CObjectSound*>* m_pList;
};

#endif
