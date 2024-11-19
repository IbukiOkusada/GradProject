//=============================================================================
//
// �T�E���h���� [objectsound.cpp]
// Author : �O�엳�V��
//
//=============================================================================
#include "objectsound.h"
CMasterSound* CMasterSound::m_pInstance = nullptr;
//=============================================================================
// �R���X�g���N�^
//=============================================================================
CMasterSound::CObjectSound::CObjectSound()
{
	m_sPath.clear();			//�p�X
	m_pSourceVoice = nullptr;	// �\�[�X�{�C�X
	m_nCntLoop = 0;				// ���[�v�J�E���g
	m_fVolume = 1.0f;
	m_fPitch = 1.0f;

	m_pDataAudio = nullptr;		// �I�[�f�B�I�f�[�^
	m_dSizeAudio = 0;			// �I�[�f�B�I�f�[�^�T�C�Y
	GetInstance()->m_pList->Regist(this);
}
//=============================================================================
// �f�X�g���N�^
//=============================================================================
CMasterSound::CObjectSound::~CObjectSound()
{
	GetInstance()->m_pList->Delete(this);
}
//=============================================================================
// ������
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

	// �o�b�t�@�̃N���A
	memset(&wfx, 0, sizeof(WAVEFORMATEXTENSIBLE));
	memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));

	// �T�E���h�f�[�^�t�@�C���̐���
	hFile = CreateFile(m_sPath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		MessageBox(hWnd, "�T�E���h�f�[�^�t�@�C���̐����Ɏ��s�I(1)", "�x���I", MB_ICONWARNING);
		return HRESULT_FROM_WIN32(GetLastError());
	}
	if (SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{// �t�@�C���|�C���^��擪�Ɉړ�
		MessageBox(hWnd, "�T�E���h�f�[�^�t�@�C���̐����Ɏ��s�I(2)", "�x���I", MB_ICONWARNING);
		return HRESULT_FROM_WIN32(GetLastError());
	}

	// WAVE�t�@�C���̃`�F�b�N
	hr = CheckChunk(hFile, 'FFIR', &dwChunkSize, &dwChunkPosition);
	if (FAILED(hr))
	{
		MessageBox(hWnd, "WAVE�t�@�C���̃`�F�b�N�Ɏ��s�I(1)", "�x���I", MB_ICONWARNING);
		return S_FALSE;
	}
	hr = ReadChunkData(hFile, &dwFiletype, sizeof(DWORD), dwChunkPosition);
	if (FAILED(hr))
	{
		MessageBox(hWnd, "WAVE�t�@�C���̃`�F�b�N�Ɏ��s�I(2)", "�x���I", MB_ICONWARNING);
		return S_FALSE;
	}
	if (dwFiletype != 'EVAW')
	{
		MessageBox(hWnd, "WAVE�t�@�C���̃`�F�b�N�Ɏ��s�I(3)", "�x���I", MB_ICONWARNING);
		return S_FALSE;
	}

	// �t�H�[�}�b�g�`�F�b�N
	hr = CheckChunk(hFile, ' tmf', &dwChunkSize, &dwChunkPosition);
	if (FAILED(hr))
	{
		MessageBox(hWnd, "�t�H�[�}�b�g�`�F�b�N�Ɏ��s�I(1)", "�x���I", MB_ICONWARNING);
		return S_FALSE;
	}
	hr = ReadChunkData(hFile, &wfx, dwChunkSize, dwChunkPosition);
	if (FAILED(hr))
	{
		MessageBox(hWnd, "�t�H�[�}�b�g�`�F�b�N�Ɏ��s�I(2)", "�x���I", MB_ICONWARNING);
		return S_FALSE;
	}

	// �I�[�f�B�I�f�[�^�ǂݍ���
	hr = CheckChunk(hFile, 'atad', &m_dSizeAudio, &dwChunkPosition);
	if (FAILED(hr))
	{
		MessageBox(hWnd, "�I�[�f�B�I�f�[�^�ǂݍ��݂Ɏ��s�I(1)", "�x���I", MB_ICONWARNING);
		return S_FALSE;
	}
	m_pDataAudio = (BYTE*)malloc(m_dSizeAudio);
	hr = ReadChunkData(hFile, m_pDataAudio, m_dSizeAudio, dwChunkPosition);
	if (FAILED(hr))
	{
		MessageBox(hWnd, "�I�[�f�B�I�f�[�^�ǂݍ��݂Ɏ��s�I(2)", "�x���I", MB_ICONWARNING);
		return S_FALSE;
	}

	// �\�[�X�{�C�X�̐���
	hr = GetInstance()->m_pXAudio2->CreateSourceVoice(&m_pSourceVoice, &(wfx.Format));
	if (FAILED(hr))
	{
		MessageBox(hWnd, "�\�[�X�{�C�X�̐����Ɏ��s�I", "�x���I", MB_ICONWARNING);
		return S_FALSE;
	}

	// �o�b�t�@�̒l�ݒ�
	memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));
	buffer.AudioBytes = m_dSizeAudio;
	buffer.pAudioData = m_pDataAudio;
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	buffer.LoopCount = m_nCntLoop;

	// �I�[�f�B�I�o�b�t�@�̓o�^
	m_pSourceVoice->SubmitSourceBuffer(&buffer);
	m_buffer = buffer;
	// �t�@�C�����N���[�Y
	CloseHandle(hFile);
	return S_OK;
}

//=============================================================================
// �I��
//=============================================================================
void CMasterSound::CObjectSound::Uninit()
{

	// �ꎞ��~
	XAUDIO2_VOICE_STATE xa2state;
	// ��Ԏ擾
	m_pSourceVoice->GetState(&xa2state);
	if (xa2state.BuffersQueued != 0)
	{// �Đ���
		// �ꎞ��~
		m_pSourceVoice->Stop(0);
	}
	// �\�[�X�{�C�X�̔j��
	m_pSourceVoice->DestroyVoice();
	m_pSourceVoice = NULL;

	// �I�[�f�B�I�f�[�^�̊J��
	free(m_pDataAudio);
	m_pDataAudio = NULL;
}
//=============================================================================
// �ŏ�����Đ�
//=============================================================================
void CMasterSound::CObjectSound::Play()
{
	XAUDIO2_VOICE_STATE xa2state;
	// ��Ԏ擾
	m_pSourceVoice->GetState(&xa2state);
	if (xa2state.BuffersQueued != 0)
	{// �Đ���
		// �ꎞ��~
		m_pSourceVoice->Stop(0);
	}
	// �I�[�f�B�I�o�b�t�@�̍폜
	m_pSourceVoice->FlushSourceBuffers();
	// �I�[�f�B�I�o�b�t�@�̓o�^
	m_pSourceVoice->SubmitSourceBuffer(&m_buffer);

	// �Đ�
	m_pSourceVoice->Start(0);

}
//=============================================================================
// ��~
//=============================================================================
void CMasterSound::CObjectSound::Stop()
{

	// �ꎞ��~
	m_pSourceVoice->Stop(0);
	
	// �I�[�f�B�I�o�b�t�@�̍폜
	m_pSourceVoice->FlushSourceBuffers();
	// �I�[�f�B�I�o�b�t�@�̓o�^
	m_pSourceVoice->SubmitSourceBuffer(&m_buffer);

}
//=============================================================================
// �ꎞ��~
//=============================================================================
void CMasterSound::CObjectSound::Pause()
{
	//�ꎞ��~
	m_pSourceVoice->Start(0);
}
//=============================================================================
// �Đ�
//=============================================================================
void CMasterSound::CObjectSound::Start()
{
	// �Đ�
	m_pSourceVoice->Start(0);
}
//=============================================================================
// �{�����[���ݒ�
//=============================================================================
void CMasterSound::CObjectSound::SetVolume(float fVolume)
{
	m_fVolume = fVolume;
	m_pSourceVoice->SetVolume(fVolume);
}
//=============================================================================
// �s�b�`�ݒ�
//=============================================================================
void CMasterSound::CObjectSound::SetPitch(float fPitch)
{
	m_fPitch = fPitch;
	m_pSourceVoice->SetFrequencyRatio(fPitch);
}
//=============================================================================
// ����
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
// �`�����N�̃`�F�b�N
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
	{// �t�@�C���|�C���^��擪�Ɉړ�
		return HRESULT_FROM_WIN32(GetLastError());
	}

	while (hr == S_OK)
	{
		if (ReadFile(hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL) == 0)
		{// �`�����N�̓ǂݍ���
			hr = HRESULT_FROM_WIN32(GetLastError());
		}

		if (ReadFile(hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL) == 0)
		{// �`�����N�f�[�^�̓ǂݍ���
			hr = HRESULT_FROM_WIN32(GetLastError());
		}

		switch (dwChunkType)
		{
		case 'FFIR':
			dwRIFFDataSize = dwChunkDataSize;
			dwChunkDataSize = 4;
			if (ReadFile(hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL) == 0)
			{// �t�@�C���^�C�v�̓ǂݍ���
				hr = HRESULT_FROM_WIN32(GetLastError());
			}
			break;

		default:
			if (SetFilePointer(hFile, dwChunkDataSize, NULL, FILE_CURRENT) == INVALID_SET_FILE_POINTER)
			{// �t�@�C���|�C���^���`�����N�f�[�^���ړ�
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
// �`�����N�f�[�^�̓ǂݍ���
//=============================================================================
HRESULT CMasterSound::CObjectSound::ReadChunkData(HANDLE hFile, void* pBuffer, DWORD dwBuffersize, DWORD dwBufferoffset)
{
	DWORD dwRead;

	if (SetFilePointer(hFile, dwBufferoffset, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{// �t�@�C���|�C���^���w��ʒu�܂ňړ�
		return HRESULT_FROM_WIN32(GetLastError());
	}

	if (ReadFile(hFile, pBuffer, dwBuffersize, &dwRead, NULL) == 0)
	{// �f�[�^�̓ǂݍ���
		return HRESULT_FROM_WIN32(GetLastError());
	}

	return S_OK;
}

//=============================================================================
// �R���X�g���N�^
//=============================================================================
CMasterSound::CMasterSound()
{
	m_pXAudio2 = nullptr;
	m_pMasteringVoice = nullptr;
}
//=============================================================================
// �f�X�g���N�^
//=============================================================================
CMasterSound::~CMasterSound()
{
}
//=============================================================================
// ������
//=============================================================================
HRESULT CMasterSound::Init(HWND hWnd)
{
	HRESULT hr;

	// COM���C�u�����̏�����
	hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);

	if (FAILED(hr))
	{
		MessageBox(hWnd, "COM���C�u�����̏������Ɏ��s�I", "�x���I", MB_ICONWARNING);

		return E_FAIL;
	}

	// XAudio2�I�u�W�F�N�g�̍쐬
	hr = XAudio2Create(&m_pXAudio2, 0);
	if (FAILED(hr))
	{
		MessageBox(hWnd, "XAudio2�I�u�W�F�N�g�̍쐬�Ɏ��s�I", "�x���I", MB_ICONWARNING);

		// COM���C�u�����̏I������
		CoUninitialize();

		return E_FAIL;
	}

	// �}�X�^�[�{�C�X�̐���
	hr = m_pXAudio2->CreateMasteringVoice(&m_pMasteringVoice);
	if (FAILED(hr))
	{
		MessageBox(hWnd, "�}�X�^�[�{�C�X�̐����Ɏ��s�I", "�x���I", MB_ICONWARNING);

		if (m_pXAudio2 != NULL)
		{
			// XAudio2�I�u�W�F�N�g�̊J��
			m_pXAudio2->Release();
			m_pXAudio2 = NULL;
		}

		// COM���C�u�����̏I������
		CoUninitialize();

		return E_FAIL;
	}
	m_hWnd = hWnd;
	m_pList = DEBUG_NEW Clist<CObjectSound*>;
	return S_OK;
}
//=============================================================================
//	�I��
//=============================================================================
void CMasterSound::Uninit()
{
	SAFE_DELETE(m_pList);
	// �}�X�^�[�{�C�X�̔j��
	m_pMasteringVoice->DestroyVoice();
	m_pMasteringVoice = NULL;

	if (m_pXAudio2 != NULL)
	{
		// XAudio2�I�u�W�F�N�g�̊J��
		m_pXAudio2->Release();
		m_pXAudio2 = NULL;
	}

	// COM���C�u�����̏I������
	CoUninitialize();
}
//=============================================================================
// �ꎞ��~
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
// �Đ�
//=============================================================================
void CMasterSound::Start()
{
	int nSize = m_pList->GetNum();
	for (int i = 0; i < nSize; i++)
	{
		m_pList->Get(i)->Start();
	}
}