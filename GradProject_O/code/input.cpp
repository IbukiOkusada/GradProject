//==========================================================
//
//���̓f�o�C�X���� [input.cpp]
//Author ����ɐ�
//
//==========================================================
#include "input.h"
#include "input_gamepad.h"
#include "input_keyboard.h"
#include "input_mouse.h"
#include "manager.h"
#include "debugproc.h"

#define REPEAT_TIME (15)	//���s�[�g�^�C�}�[

// �ÓI�����o�ϐ��錾
LPDIRECTINPUT8 CInput::m_pInput = nullptr;	// �I�u�W�F�N�g�ւ̃|�C���^
CInput* CInput::m_pInstance = nullptr;	// �I�u�W�F�N�g�ւ̃|�C���^

//====================================================================================
//���N���X(���̓f�o�C�X)
//====================================================================================
// �R���X�g���N�^
//==========================================================
CInput::CInput()
{
	m_pDevice = nullptr;
}

//==========================================================
// �f�X�g���N�^
//==========================================================
CInput::~CInput()
{
	m_pDevice = nullptr;
}

//==========================================================================
// ��������
//==========================================================================
CInput* CInput::Create(HINSTANCE hInstance, HWND hWnd)
{
	if (m_pInstance != nullptr) {
		return m_pInstance;
	}

	// ���g
	m_pInstance = DEBUG_NEW CInput;
	if (m_pInstance == nullptr) {
		return nullptr;
	}

	// �}�E�X
	m_pInstance->m_pMouse = CInputMouse::Create(hInstance, hWnd);
	if (m_pInstance->m_pMouse == nullptr) {
		return nullptr;
	}

	// �L�[�{�[�h
	m_pInstance->m_pKeyboard = CInputKeyboard::Create(hInstance, hWnd);
	if (m_pInstance->m_pKeyboard == nullptr) {
		return nullptr;
	}

	// �Q�[���p�b�h
	m_pInstance->m_pPad = CInputPad::Create(hInstance, hWnd);
	if (m_pInstance->m_pPad == nullptr) {
		return nullptr;
	}

	//// �L�[�R���t�B�O
	//m_pInstance->m_pKeyConfig = CKeyConfigManager::Create();
	//if (m_pInstance->m_pKeyConfig == nullptr) {
	//	return nullptr;
	//}

	return m_pInstance;
}

//==========================================================
// ����������
//==========================================================
HRESULT CInput::Init(HINSTANCE hInstance, HWND hWnd)
{
	if (m_pInput == nullptr)
	{// �܂��N���g�p�J�n���Ă��Ȃ��ꍇ

		//DirectInput�I�u�W�F�N�g�̐���
		if (FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pInput, nullptr)))
		{// �������s�����ꍇ
			return E_FAIL;
		}
	}

	return S_OK;
}

//==========================================================
//�I������
//==========================================================
void CInput::Uninit(void)
{
	//�f�o�C�X�̔p��(+�A�N�Z�X���J��)
	if (m_pDevice != nullptr)
	{
		m_pDevice->Unacquire();
		m_pDevice->Release();
		m_pDevice = nullptr;
	}

	//�I�u�W�F�N�g�̔p��
	if (m_pInput != nullptr)
	{
		m_pInput->Release();
		m_pInput = nullptr;
	}
}

#if 1	// �������z�֐��Ȃ̂Œʂ�Ȃ�
//==========================================================
//�X�V����
//==========================================================
void CInput::Update(void)
{
	// �e�X�V
	m_pMouse->Update();
	m_pKeyboard->Update();
	m_pPad->Update();
}
#endif

//==========================================================
// �J������
//==========================================================
void CInput::Release()
{
	if (m_pMouse != nullptr) {
		m_pMouse->Uninit();
		m_pMouse = nullptr;
	}

	if (m_pKeyboard != nullptr) {
		m_pKeyboard->Uninit();
		m_pKeyboard = nullptr;
	}

	if (m_pPad != nullptr) {
		m_pPad->Uninit();
		m_pPad = nullptr;
	}

	/*if (m_pKeyConfig != nullptr) {
		m_pKeyConfig->Uninit();
		m_pKeyConfig = nullptr;
	}*/

	// �I�u�W�F�N�g�̔j��
	if (m_pInput != nullptr) {
		m_pInput->Release();
		m_pInput = nullptr;
	}

	if (m_pInstance != nullptr) {
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}
