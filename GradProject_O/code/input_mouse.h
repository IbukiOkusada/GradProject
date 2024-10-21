//==========================================================
//
// ���̓f�o�C�X����(�}�E�X) [input_mouse.h]
// Author Ibuki Okusada
//
//==========================================================
#ifndef _INPUT_MOUSE_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _INPUT_MOUSE_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "input.h"

//==========================================================
//�h���N���X�̒�`(�}�E�X)
//==========================================================
class CInputMouse : public CInput
{
public:	// �N�ł��A�N�Z�X�\

	//�}�E�X
	enum BUTTON
	{
		BUTTON_LBUTTON = 0,	// ���N���b�N
		BUTTON_RBUTTON,		// �E�N���b�N
		BUTTON_WHEEL,			// �z�C�[��
		BUTTON_MAX
	};

	CInputMouse();	// �R���X�g���N�^
	~CInputMouse();	// �f�X�g���N�^

	// �����o�֐�
	HRESULT Init(HINSTANCE hInstance, HWND hWnd);
	void Uninit(void);
	void Update(void);
	bool GetPress(int nKey);
	bool GetTrigger(int nKey);
	bool GetRelease(int nKey);
	bool GetRepeat(int nKey);
	D3DXVECTOR3 GetCousorMove();
	D3DXVECTOR3 GetWorldPos() { return m_WorldPos; }
	static CInputMouse* Create(HINSTANCE hInstance, HWND hWnd);
	static CInputMouse* GetInstance() { return m_pInstance; }

private:	// ���������A�N�Z�X�\

	// �����o�ϐ�
	DIMOUSESTATE2 m_State;	//�v���X���
	DIMOUSESTATE2 m_Trigger;	//�g���K�[���
	DIMOUSESTATE2 m_Release;	//�����[�X���
	DIMOUSESTATE2 m_Repeat;	//���s�[�g���
	POINT m_Point;			//�J�[�\���|�C���^�[
	D3DXVECTOR3 m_WorldPos;
	static CInputMouse* m_pInstance;
};

#endif
