//==========================================================
//
//���̓f�o�C�X���� [input.cpp]
//Author ����ɐ�
//
//==========================================================
#include "input.h"
#include "input_gamepad.h"
#include "manager.h"
#include "debugproc.h"

#define REPEAT_TIME (15)	//���s�[�g�^�C�}�[

// �ÓI�����o�ϐ��錾
CInputPad* CInputPad::m_pInstance = nullptr;	// �I�u�W�F�N�g�ւ̃|�C���^

#include "input_gamepad.h"

//==========================================================
//�p�b�h�̃R���X�g���N�^
//==========================================================
CInputPad::CInputPad()
{

}

//==========================================================
//�f�X�g���N�^
//==========================================================
CInputPad::~CInputPad()
{

}

//==========================================================================
// ��������
//==========================================================================
CInputPad* CInputPad::Create(HINSTANCE hInstance, HWND hWnd)
{
	if (m_pInstance != nullptr) {
		return m_pInstance;
	}

	// �C���X�^���X����
	m_pInstance = DEBUG_NEW CInputPad;
	if (m_pInstance != nullptr) {

		// ����������
		m_pInstance->Init(hInstance, hWnd);
	}

	return m_pInstance;
}

//==========================================================
//����������
//==========================================================
HRESULT CInputPad::Init(HINSTANCE hInstance, HWND hWnd)
{
	CInput::Init(hInstance, hWnd);

	//Xinput�̃X�e�[�g��ݒ�
	XInputEnable(true);
	int nCnt;

	for (nCnt = 0; nCnt < PLAYER_MAX; nCnt++)
	{
		//�������[���N���A
		memset(&m_State[nCnt], 0, sizeof(XINPUT_STATE));
		memset(&m_Trigger[nCnt], 0, sizeof(XINPUT_STATE));
	}

	return S_OK;
}

//==========================================================
//�I������
//==========================================================
void CInputPad::Uninit(void)
{
	//Xinput�X�e�[�g��ݒ�
	XInputEnable(false);

	CInput::Uninit();

	delete m_pInstance;
	m_pInstance = nullptr;
}

//==========================================================
// �X�V����
//==========================================================
void CInputPad::Update(void)
{
	XINPUT_STATE aGamepadState[PLAYER_MAX];	//�Q�[���p�b�h�̓��͏��
	static int nRepeatJoyKeyCnt[PLAYER_MAX] = {};
	int nCntPad = 0;

	for (nCntPad = 0; nCntPad < PLAYER_MAX; nCntPad++)
	{
		//���̓f�o�C�X����f�[�^���擾
		if (XInputGetState(nCntPad, &aGamepadState[nCntPad]) == ERROR_SUCCESS)
		{
			m_Trigger[nCntPad].Gamepad.wButtons =
				~m_State[nCntPad].Gamepad.wButtons
				& aGamepadState[nCntPad].Gamepad.wButtons;	//�g���K�[(�{�^��)

			m_Release[nCntPad].Gamepad.wButtons =
				(m_State[nCntPad].Gamepad.wButtons
					^ aGamepadState[nCntPad].Gamepad.wButtons)
				& m_State[nCntPad].Gamepad.wButtons;	//�����[�X(�{�^��)

			m_State[nCntPad] = aGamepadState[nCntPad];	//�v���X

			//���s�[�g�擾

		}
	}
}

//==========================================================
// �v���X����
//==========================================================
bool CInputPad::GetPress(int nKey, int nPlayer)
{
	return (m_State[nPlayer].Gamepad.wButtons & (0x01 << nKey)) ? true : false;
}

//==========================================================
// �g���K�[����
//==========================================================
bool CInputPad::GetTrigger(int nKey, int nPlayer)
{
	return (m_Trigger[nPlayer].Gamepad.wButtons & (0x01 << nKey)) ? true : false;
}

//==========================================================
// �����[�X����
//==========================================================
bool CInputPad::GetRelease(int nKey, int nPlayer)
{
	return (m_Release[nPlayer].Gamepad.wButtons & (0x01 << nKey)) ? true : false;
}

//==========================================================
// ���s�[�g����
//==========================================================
bool CInputPad::GetRepeat(int nKey, int nPlayer)
{
	return (m_Repeat[nPlayer].Gamepad.wButtons & (0x01 << nKey)) ? true : false;
}

//==========================================================
// �X�e�B�b�N�擾
//==========================================================
bool CInputPad::GetStickPress(int nPlayer, int nKey, float DeadZone, Stick PlusStick)
{
	float fAnswer = 0.0f;

	switch (nKey)
	{
	case BUTTON_LEFT_X:
		fAnswer = (float)m_State[nPlayer].Gamepad.sThumbLX / SHRT_MAX;
		break;
	case BUTTON_LEFT_Y:
		fAnswer = (float)m_State[nPlayer].Gamepad.sThumbLY / SHRT_MAX;
		break;
	case BUTTON_RIGHT_X:
		fAnswer = (float)m_State[nPlayer].Gamepad.sThumbRX / SHRT_MAX;
		break;
	case BUTTON_RIGHT_Y:
		fAnswer = (float)m_State[nPlayer].Gamepad.sThumbRY / SHRT_MAX;
		break;
	default:
		break;
	}

	if ((fAnswer < 0.0f && PlusStick == STICK_MINUS && fAnswer < -DeadZone) || (fAnswer > 0.0f && PlusStick == STICK_PLUS && fAnswer > DeadZone))
	{
		return true;
	}


	return false;
}

//==========================================================
// �X�e�B�b�N�̒l�擾
//==========================================================
float CInputPad::GetStickAdd(int nPlayer, int nKey, float DeadZone, Stick PlusStick)
{
	float fAnswer = 0.0f;

	switch (nKey)
	{
	case BUTTON_LEFT_X:
		fAnswer = (float)m_State[nPlayer].Gamepad.sThumbLX / SHRT_MAX;
		break;
	case BUTTON_LEFT_Y:
		fAnswer = (float)m_State[nPlayer].Gamepad.sThumbLY / SHRT_MAX;
		break;
	case BUTTON_RIGHT_X:
		fAnswer = (float)m_State[nPlayer].Gamepad.sThumbRX / SHRT_MAX;
		break;
	case BUTTON_RIGHT_Y:
		fAnswer = (float)m_State[nPlayer].Gamepad.sThumbRY / SHRT_MAX;
		break;
	default:
		break;
	}

	return fAnswer;
}

//==========================================================
// �E�g���K�[�擾
//==========================================================
BYTE CInputPad::GetRightTriggerPress(int nPlayer)
{
	return (m_State[nPlayer].Gamepad.bRightTrigger);
}

//==========================================================
// ���g���K�[�擾
//==========================================================
BYTE CInputPad::GetLeftTriggerPress(int nPlayer)
{
	return (m_State[nPlayer].Gamepad.bLeftTrigger);
}