//==========================================================
//
// ���̓f�o�C�X����(�Q�[���p�b�h) [input_gamepad.h]
// Author Ibuki Okusada
//
//==========================================================
#ifndef _INPUT_PAD_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _INPUT_PAD_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "input.h"
#include "Xinput.h"

#pragma comment(lib, "xinput.lib")	//���͏���

//==========================================================
//�}�N����`
//==========================================================
namespace
{
	const int PLAYER_MAX = (4);	//�v���C���[�̍ő吔
}

//==========================================================
//�h���N���X�̒�`(�p�b�h)
//==========================================================
class CInputPad : public CInput
{
public:	// �N�ł��A�N�Z�X�\

	//�Q�[���p�b�h�{�^��
	enum BUTTON
	{
		BUTTON_UP = 0,		// ��{�^��
		BUTTON_DOWN,			// ���{�^��
		BUTTON_LEFT,			// ���{�^��
		BUTTON_RIGHT,			// �E�{�^��
		BUTTON_START,			// start
		BUTTON_BACK,			// back
		BUTTON_LEFTSTCPUSH,	// ���X�e�B�b�N��������
		BUTTON_RIGHTSTCPUSH,	// �E�X�e�B�b�N��������
		BUTTON_LEFTBUTTON,	// LB(L)�{�^��
		BUTTON_RIGHTBUTTON,	// RB(R)�{�^��
		BUTTON_11,			// L�g���K�[
		BUTTON_12,			// R�g���K�[
		BUTTON_A,			// A�{�^��
		BUTTON_B,			// B�{�^��
		BUTTON_X,			// X�{�^��
		BUTTON_Y,			// Y�{�^��
		BUTTON_LEFT_Y,		// ��(Y)
		BUTTON_LEFT_X,		// ��(X)
		BUTTON_RIGHT_Y,		// �E(Y)
		BUTTON_RIGHT_X,		// �E(X)
		BUTTON_MAX
	};

	//�X�e�B�b�N
	typedef enum
	{
		STICK_PLUS = 0,		// �l���v���X
		STICK_MINUS,			// �l���}�C�i�X
		STICK_MAX
	}Stick;

	CInputPad();	// �R���X�g���N�^
	~CInputPad();	// �f�X�g���N�^

	// �����o�֐�
	HRESULT Init(HINSTANCE hInstance, HWND hWnd);
	void Uninit(void);
	void Update(void);
	bool GetPress(int nKey, int nPlayer);
	bool GetTrigger(int nKey, int nPlayer);
	bool GetRelease(int nKey, int nPlayer);
	bool GetRepeat(int nKey, int nPlayer);
	bool GetStickPress(int nPlayer, int nKey, float DeadZone, Stick PlusStick);
	BYTE GetRightTriggerPress(int nPlayer);
	BYTE GetLeftTriggerPress(int nPlayer);
	float GetStickAdd(int nPlayer, int nKey, float DeadZone, Stick PlusStick);
	D3DXVECTOR3 GetLStick(int nPlayer, float DeadZone);
	D3DXVECTOR3 GetRStick(int nPlayer, float DeadZone);
	static CInputPad* Create(HINSTANCE hInstance, HWND hWnd);
	static CInputPad* GetInstance() { return m_pInstance; }

private:	// ���������A�N�Z�X�\

	// �����o�ϐ�
	XINPUT_STATE m_State[PLAYER_MAX];		// �v���X
	XINPUT_STATE m_Trigger[PLAYER_MAX];		// �g���K�[
	XINPUT_STATE m_Release[PLAYER_MAX];		// �����[�X
	XINPUT_STATE m_Repeat[PLAYER_MAX];	// ���s�[�g
	int m_aRepeatCnt[PLAYER_MAX];			// �p�b�h�̃��s�[�g�J�E���g
	static CInputPad* m_pInstance;
};

#endif
