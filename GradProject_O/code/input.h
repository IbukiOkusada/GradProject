//==========================================================
//
// ���̓f�o�C�X���� [input.h]
// Author Ibuki Okusada
//
//==========================================================
#ifndef _INPUT_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _INPUT_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "main.h"

// �O���錾
class CInputKeyboard;
class CInputMouse;
class CInputPad;

//==========================================================
//���N���X�̒�`(���̓f�o�C�X)
//==========================================================
class CInput
{
public:	// �N�ł��A�N�Z�X�\
	
	CInput();	// �R���X�g���N�^
	virtual ~CInput();	// �f�X�g���N�^

	// �����o�֐�
	virtual HRESULT Init(HINSTANCE hInstance, HWND hWnd);
	virtual void Uninit(void);
	virtual void Update(void);
	void Release();
	static CInput* Create(HINSTANCE hInstance, HWND hWnd);

protected:	// �h���N���X������A�N�Z�X�\
	static LPDIRECTINPUT8 m_pInput;	//DirectInput�I�u�W�F�N�g�ւ̃|�C���^
	LPDIRECTINPUTDEVICE8 m_pDevice;	//���̓f�o�C�X�ւ̃|�C���^

private:	// ���������A�N�Z�X�\

	// �����o�ϐ�
	CInputKeyboard *m_pKeyboard;	// �L�[�{�[�h
	CInputMouse* m_pMouse;			// �}�E�X
	CInputPad* m_pPad;				// �p�b�h
	static CInput* m_pInstance;		// ���g�̃|�C���^
	
};

#endif
