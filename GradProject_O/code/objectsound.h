//=============================================================================
//
// �T�E���h���� [sound.h]
// Author : Ibuki Okusada
//
//=============================================================================
#ifndef _OBJECTSOUND_H_
#define _OBJECTSOUND_H_

#include "main.h"
#include "convenience.h"
#include "list.h"
//=============================================================================
// �T�E���h�N���X�̒�`
//=============================================================================
class CMasterSound
{
public:	// �N�ł��A�N�Z�X�\�Ȓ�`

	// ��ޗ�
	typedef enum
	{
		LABEL_BGM_TITLE = 0,		// BGM�^�C�g��
		//LABEL_SE_ENTER,			// PRESSENTER��
		LABEL_MAX
	} LABEL;
	class CObjectSound
	{
	public:
		CObjectSound();
		~CObjectSound();
		// �����o�֐�
		HRESULT Init();
		void Uninit();
		void Play();
		void Stop();
		void Pause();
		void Start();
		bool IsStart() { return m_bStart; }
	
		HRESULT Load(std::string sPath);
		
		float GetVolume() { return m_fVolume; }
		void SetVolume(float fVolume/*�����l��1.0f*/);
		float GetPitch() { return m_fPitch; }
		void SetPitch(float fPitch/*�����l��1.0f*/);
		bool GetPlay() {
			XAUDIO2_VOICE_STATE state;
			m_pSourceVoice->GetState(&state);

			// BuffersQueued ��0�̏ꍇ�A�Đ����I�����Ă���
			return (state.BuffersQueued > 0);
		}

		static CObjectSound* Create(std::string sPath,int nLoop);
	private:
		std::string m_sPath;	//�p�X
		IXAudio2SourceVoice* m_pSourceVoice;	// �\�[�X�{�C�X
		int m_nCntLoop;		// ���[�v�J�E���g
		float m_fVolume;
		float m_fPitch;
		BYTE* m_pDataAudio;					// �I�[�f�B�I�f�[�^
		DWORD m_dSizeAudio;					// �I�[�f�B�I�f�[�^�T�C�Y
		XAUDIO2_BUFFER m_buffer;
		bool m_bStart;
		// �����o�֐�
		HRESULT CheckChunk(HANDLE hFile, DWORD format, DWORD* pChunkSize, DWORD* pChunkDataPosition);
		HRESULT ReadChunkData(HANDLE hFile, void* pBuffer, DWORD dwBuffersize, DWORD dwBufferoffset);
	};

public:	// �N�ł��A�N�Z�X�\

	CMasterSound();	// �R���X�g���N�^
	~CMasterSound();	// �f�X�g���N�^

	// �����o�֐�
	HRESULT Init(HWND hWnd);
	void Uninit();
	void Pause();
	void Start();

	static CMasterSound* GetInstance() { if (m_pInstance == nullptr) { m_pInstance = DEBUG_NEW CMasterSound;}return m_pInstance; }
	static void Release() { if (m_pInstance != nullptr) { delete m_pInstance; } }
private:	// �����������A�N�Z�X�\
	// �����o�ϐ�
	static CMasterSound* m_pInstance;
	IXAudio2* m_pXAudio2;							// XAudio2�I�u�W�F�N�g�ւ̃C���^�[�t�F�C�X
	IXAudio2MasteringVoice* m_pMasteringVoice;		// �}�X�^�[�{�C�X
	
protected:
	
	// �����o�ϐ�
	HWND m_hWnd;
	Clist<CObjectSound*>* m_pList;
};

#endif
