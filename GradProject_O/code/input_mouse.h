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
public:	// �N�ł��A�N�Z�X�\�Ȓ�`

	//�}�E�X
	enum BUTTON
	{
		BUTTON_LBUTTON = 0,	// ���N���b�N
		BUTTON_RBUTTON,		// �E�N���b�N
		BUTTON_WHEEL,			// �z�C�[��
		BUTTON_MAX
	};

	// ���C���\����
	struct SRayInfo
	{
		D3DXVECTOR3 origin;	// �J�n�_
		D3DXVECTOR3 end;	// �I�_
		D3DXVECTOR3 vec;	// ����
		D3DXVECTOR3 vecold;	// �O��̕���

		SRayInfo() : origin(D3DXVECTOR3(0.0f, 0.0f, 0.0f)), end(D3DXVECTOR3(0.0f, 0.0f, 0.0f)), 
			vec(D3DXVECTOR3(0.0f, 0.0f, 0.0f)), vecold(D3DXVECTOR3(0.0f, 0.0f, 0.0f)) {}
	};

public:	// �N�ł��A�N�Z�X�\
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
	SRayInfo GetRayInfo() { return m_RayInfo; }
	static CInputMouse* Create(HINSTANCE hInstance, HWND hWnd);
	static CInputMouse* GetInstance() { return m_pInstance; }

private:	// ���������A�N�Z�X�\

	// �����o�ϐ�
	DIMOUSESTATE2 m_State;	//�v���X���
	DIMOUSESTATE2 m_Trigger;	//�g���K�[���
	DIMOUSESTATE2 m_Release;	//�����[�X���
	DIMOUSESTATE2 m_Repeat;	//���s�[�g���
	POINT m_Point;			//�J�[�\���|�C���^�[
	SRayInfo m_RayInfo;
	static CInputMouse* m_pInstance;
};

#endif
