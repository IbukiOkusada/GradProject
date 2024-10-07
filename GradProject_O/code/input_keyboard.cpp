//==========================================================
//
//���̓f�o�C�X����(�L�[�{�[�h) [input_keyboard.cpp]
//Author ����ɐ�
//
//==========================================================
#include "input_keyboard.h"
#include "manager.h"
#include "debugproc.h"

#define REPEAT_TIME (15)	//���s�[�g�^�C�}�[

// �ÓI�����o�ϐ��錾
CInputKeyboard* CInputKeyboard::m_pInstance = nullptr;	// �I�u�W�F�N�g�ւ̃|�C���^

//====================================================================================
//�h���N���X(�L�[�{�[�h)
//====================================================================================
// �R���X�g���N�^
//==========================================================
CInputKeyboard::CInputKeyboard()
{
	// �l���N���A����
	for (int nCntKey = 0; nCntKey < NUM_KEY_MAX; nCntKey++)
	{
		m_aKeyState[nCntKey] = 0;
		m_aKeyStateTrigger[nCntKey] = 0;
		m_aKeyStateRelease[nCntKey] = 0;
		m_aKeyStateRepeat[nCntKey] = 0;
	}
}

//==========================================================
// �f�X�g���N�^
//==========================================================
CInputKeyboard::~CInputKeyboard()
{

}

//==========================================================================
// ��������
//==========================================================================
CInputKeyboard* CInputKeyboard::Create(HINSTANCE hInstance, HWND hWnd)
{
	if (m_pInstance != nullptr) {
		return m_pInstance;
	}

	// �C���X�^���X����
	m_pInstance = DEBUG_NEW CInputKeyboard;
	if (m_pInstance != nullptr) {

		// ����������
		m_pInstance->Init(hInstance, hWnd);
	}

	return m_pInstance;
}

//==========================================================
//����������
//==========================================================
HRESULT CInputKeyboard::Init(HINSTANCE hInstance, HWND hWnd)
{
	CInput::Init(hInstance, hWnd);

	//���̓f�o�C�X(�L�[�{�[�h)�̐���
	if (FAILED(m_pInput->CreateDevice(GUID_SysKeyboard, &m_pDevice, nullptr)))
	{
		return E_FAIL;
	}

	//�f�[�^�t�H�[�}�b�g��ݒ�
	if (FAILED(m_pDevice->SetDataFormat(&c_dfDIKeyboard)))
	{
		return E_FAIL;
	}

	//�f�[�^�t�H�[�}�b�g��ݒ�
	if (FAILED(m_pDevice->SetCooperativeLevel(hWnd, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE))))
	{
		return E_FAIL;
	}

	//�L�[�{�[�h�ւ̃A�N�Z�X�����擾
	m_pDevice->Acquire();

	return S_OK;
}

//==========================================================
//�I������
//==========================================================
void CInputKeyboard::Uninit(void)
{
	CInput::Uninit();

	delete m_pInstance;
	m_pInstance = nullptr;
}

//==========================================================
//�X�V����
//==========================================================
void CInputKeyboard::Update(void)
{
	BYTE aKeyState[NUM_KEY_MAX];	//�L�[�{�[�h�̓��͏��
	int nCntKey = 0;

	//���̓f�o�C�X����f�[�^���擾
	if (SUCCEEDED(m_pDevice->GetDeviceState(sizeof(aKeyState), &aKeyState[0])))
	{
		for (nCntKey = 0; nCntKey < NUM_KEY_MAX; nCntKey++)
		{
			m_aKeyStateTrigger[nCntKey] = (m_aKeyState[nCntKey] ^ aKeyState[nCntKey]) & aKeyState[nCntKey];	//�L�[�{�[�h����̃g���K�[����ۑ�
			m_aKeyStateRelease[nCntKey] = (m_aKeyState[nCntKey] ^ aKeyState[nCntKey]) & m_aKeyState[nCntKey];	//�L�[�{�[�h����̃����[�X����ۑ�
			m_aKeyState[nCntKey] = aKeyState[nCntKey];	//�L�[�{�[�h����̃v���X����ۑ�
			m_aKeyStateRepeat[nCntKey] = m_aKeyStateTrigger[nCntKey];

			//���s�[�g�擾
			if (GetPress(nCntKey) == true)
			{//�v���X�œ��͂���Ă���Ƃ�
				m_aRepeatCnt[nCntKey]++;

				if (m_aRepeatCnt[nCntKey] >= REPEAT_TIME)
				{//���s�[�g�J�E���^�[���^�C�}�[�𒴂����Ƃ�
					m_aRepeatCnt[nCntKey] = 0;
					m_aKeyStateRepeat[nCntKey] = m_aKeyState[nCntKey];
				}
				else
				{
					m_aKeyStateRepeat[nCntKey] = m_aKeyStateRelease[nCntKey];
				}
			}

			if (GetRelease(nCntKey) == true)
			{//�����[�X���͂��ꂽ�Ƃ�
				m_aRepeatCnt[nCntKey] = 0;
			}
		}
	}
	else
	{
		m_pDevice->Acquire();	//�L�[�{�[�h�ւ̃A�N�Z�X�����l��
	}

}

//==========================================================
//�v���X�����擾
//==========================================================
bool CInputKeyboard::GetPress(int nKey)
{
	return (m_aKeyState[nKey] & 0x80) ? true : false;
}

//==========================================================
//�g���K�[�����擾
//==========================================================
bool CInputKeyboard::GetTrigger(int nKey)
{
	return (m_aKeyStateTrigger[nKey] & 0x80) ? true : false;
}

//==========================================================
//�����[�X�����擾
//==========================================================
bool CInputKeyboard::GetRelease(int nKey)
{
	return (m_aKeyStateRelease[nKey] & 0x80) ? true : false;
}

//==========================================================
//���s�[�g�����擾
//==========================================================
bool CInputKeyboard::GetRepeat(int nKey)
{
	return (m_aKeyStateRepeat[nKey] & 0x80) ? true : false;
}