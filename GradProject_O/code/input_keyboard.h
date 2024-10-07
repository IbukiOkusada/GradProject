//==========================================================
//
// ���̓f�o�C�X����(�L�[�{�[�h) [input_keyboard.h]
// Author Ibuki Okusada
//
//==========================================================
#ifndef _INPUT_KEYBOARD_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _INPUT_KEYBOARD_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "input.h"

//==========================================================
//�}�N����`
//==========================================================
namespace
{
	const int NUM_KEY_MAX = (256);	//�L�[�̍ő吔
}

//==========================================================
//�h���N���X�̒�`(�L�[�{�[�h)
//==========================================================
class CInputKeyboard : public CInput
{
public:	// �N�ł��A�N�Z�X�\

	CInputKeyboard();	// �R���X�g���N�^
	~CInputKeyboard();	// �f�X�g���N�^

	// �����o�֐�
	HRESULT Init(HINSTANCE hInstance, HWND hWnd);
	void Uninit(void);
	void Update(void);
	bool GetPress(int nKey);
	bool GetTrigger(int nKey);
	bool GetRelease(int nKey);
	bool GetRepeat(int nKey);
	static CInputKeyboard* Create(HINSTANCE hInstance, HWND hWnd);
	static CInputKeyboard* GetInstance() { return m_pInstance; }

private:	// ���������A�N�Z�X�\

	// �����o�ϐ�
	BYTE m_aKeyState[NUM_KEY_MAX];	//�v���X���
	BYTE m_aKeyStateTrigger[NUM_KEY_MAX];	//�g���K�[���
	BYTE m_aKeyStateRelease[NUM_KEY_MAX];	//�����[�X���
	BYTE m_aKeyStateRepeat[NUM_KEY_MAX];	//���s�[�g���
	int m_aRepeatCnt[NUM_KEY_MAX];	// ���s�[�g�^�C�}�[
	static CInputKeyboard* m_pInstance;
};

#endif
