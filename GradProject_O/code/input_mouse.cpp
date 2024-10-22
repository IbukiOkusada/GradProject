//==========================================================
//
//���̓f�o�C�X���� [input_mouse.cpp]
//Author ����ɐ�
//
//==========================================================
#include "input_mouse.h"
#include "manager.h"
#include "debugproc.h"
#include "camera.h"
#include "renderer.h"

#define REPEAT_TIME (15)	//���s�[�g�^�C�}�[

// �ÓI�����o�ϐ�
CInputMouse* CInputMouse::m_pInstance = nullptr;	// �I�u�W�F�N�g�ւ̃|�C���^

//====================================================================================
//�h���N���X(�}�E�X)
//====================================================================================
// �R���X�g���N�^
//==========================================================
CInputMouse::CInputMouse()
{
	m_State = {};
	m_Trigger = {};
	m_Release = {};
	m_Repeat = {};
	m_Point = {};
}

//==========================================================
// �f�X�g���N�^
//==========================================================
CInputMouse::~CInputMouse()
{

}

//==========================================================================
// ��������
//==========================================================================
CInputMouse* CInputMouse::Create(HINSTANCE hInstance, HWND hWnd)
{
	if (m_pInstance != nullptr) {
		return m_pInstance;
	}

	// �C���X�^���X����
	m_pInstance = DEBUG_NEW CInputMouse;
	if (m_pInstance != nullptr) {

		// ����������
		m_pInstance->Init(hInstance, hWnd);
	}

	return m_pInstance;
}

//==========================================================
//�}�E�X�̏�����
//==========================================================
HRESULT CInputMouse::Init(HINSTANCE hInstance, HWND hWnd)
{
	//���̓f�o�C�X�̐���
	CInput::Init(hInstance, hWnd);

	if (FAILED(m_pInput->CreateDevice(GUID_SysMouse, &m_pDevice, nullptr)))
	{
		return E_FAIL;
	}

	//�f�[�^�t�H�[�}�b�g�̐���
	if (FAILED(m_pDevice->SetDataFormat(&c_dfDIMouse2)))
	{
		return E_FAIL;
	}

	//�f�[�^�t�H�[�}�b�g��ݒ�
	if (FAILED(m_pDevice->SetCooperativeLevel(hWnd, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE))))
	{
		return E_FAIL;
	}

	//�f�o�C�X�̐ݒ�
	DIPROPDWORD diprop;
	diprop.diph.dwSize = sizeof(diprop);
	diprop.diph.dwHeaderSize = sizeof(diprop.diph);
	diprop.diph.dwObj = 0;
	diprop.diph.dwHow = DIPH_DEVICE;
	diprop.dwData = DIPROPAXISMODE_REL;	//���Βl���[�h�Őݒ�

	if (FAILED(m_pDevice->SetProperty(DIPROP_AXISMODE, &diprop.diph)))
	{
		return E_FAIL;
	}

	//�}�E�X�ւ̃A�N�Z�X�����l��
	m_pDevice->Acquire();

	return S_OK;
}

//==========================================================
//�}�E�X�̏I��
//==========================================================
void CInputMouse::Uninit(void)
{
	//���̓f�o�C�X�̔p��
	CInput::Uninit();

	delete m_pInstance;
	m_pInstance = nullptr;
}

//==========================================================
//�}�E�X�̍X�V
//==========================================================
void CInputMouse::Update(void)
{
	DIMOUSESTATE2 MouseState;	//�}�E�X�̓��͏��
	CCamera* pCam = CManager::GetInstance()->GetCamera();
	D3DVIEWPORT9 viewport;
	CManager::GetInstance()->GetRenderer()->GetDevice()->GetViewport(&viewport);

	//���̓f�o�C�X����f�[�^���擾
	if (SUCCEEDED(m_pDevice->GetDeviceState(sizeof(MouseState), (LPVOID)&MouseState)))
	{
		for (int nCnt = 0; nCnt < 8; nCnt++)
		{
			m_Trigger.rgbButtons[nCnt] = (m_State.rgbButtons[nCnt] ^ MouseState.rgbButtons[nCnt]) & MouseState.rgbButtons[nCnt];	//�L�[�{�[�h����̃g���K�[����ۑ�
		}

		m_State = MouseState;	//�v���X���擾

		//�}�E�X�̌��݂̃|�C���^���擾����
		GetCursorPos(&m_Point);

		// �X�N���[�����W�ɕϊ�
		ScreenToClient(FindWindowA(CLASS_NAME, nullptr), &m_Point);

		// ���W
		D3DXVECTOR3 vnear = D3DXVECTOR3(m_Point.x, m_Point.y, 0.0f);
		D3DXVECTOR3 vfar = D3DXVECTOR3(m_Point.x, m_Point.y, 1.0f);

		D3DXVec3Unproject(&m_RayInfo.origin, &vnear, &viewport, &pCam->GetMtxProjection(), &pCam->GetMtxView(), nullptr);
		D3DXVec3Unproject(&m_RayInfo.end, &vfar, &viewport, &pCam->GetMtxProjection(), &pCam->GetMtxView(), nullptr);

		// 4. ���C�̕������v�Z
		D3DXVECTOR3 rayDir = m_RayInfo.end - m_RayInfo.origin;
		D3DXVec3Normalize(&rayDir, &rayDir);

		m_RayInfo.vecold = m_RayInfo.vec;
		m_RayInfo.vec = rayDir;
	}
	else
	{
		m_pDevice->Acquire(); //�}�E�X�ւ̃A�N�Z�X�����l��
	}
}

//==========================================================
//�}�E�X�̃v���X���擾
//==========================================================
bool CInputMouse::GetPress(int nKey)
{
	return (m_State.rgbButtons[nKey] & 0x80) ? true : false;
}

//==========================================================
//�}�E�X�̃g���K�[���擾
//==========================================================
bool CInputMouse::GetTrigger(int nKey)
{
	return (m_Trigger.rgbButtons[nKey] & 0x80) ? true : false;
}

//==========================================================
//�}�E�X�̃����[�X���擾
//==========================================================
bool CInputMouse::GetRelease(int nKey)
{
	return (m_Release.rgbButtons[nKey] & 0x80) ? true : false;
}

//==========================================================
//�}�E�X�̃��s�[�g���擾
//==========================================================
bool CInputMouse::GetRepeat(int nKey)
{
	return (m_Repeat.rgbButtons[nKey] & 0x80) ? true : false;
}

//==========================================================
//�}�E�X�̃J�[�\���ړ��ʎ擾
//==========================================================
D3DXVECTOR3 CInputMouse::GetCousorMove(void)
{
	return D3DXVECTOR3((float)m_State.lX, (float)m_State.lY, (float)m_State.lZ);
}