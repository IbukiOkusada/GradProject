//==========================================================
//
// �X�s�[�h���[�^�[�̏��� [speedmeter.h]
// Author : Ryosuke Ohara
//
//==========================================================
#ifndef _SPEEDMETER_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _SPEEDMETER_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "object.h"

// �O���錾
class CObject2D;

//**********************************************************
// �X�s�[�h���[�^�[�N���X�̒�`
//**********************************************************
class CSpeedMeter : CObject
{
public:		// �N�ł��A�N�Z�X�\

	CSpeedMeter();	// �R���X�g���N�^
	~CSpeedMeter();	// �f�X�g���N�^

	// �����o�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	static CSpeedMeter* Create(void);

	// �擾�n
	static CSpeedMeter* GetInstance(void) { return m_pInstance; }  // ���g

private:	// �����������A�N�Z�X�\

	CObject2D* m_pFrame;   // ���[�^�[�̘g�̃|�C���^
	CObject2D* m_pNeedle;  // ���[�^�[�̐j�|�C���^

	static CSpeedMeter* m_pInstance;  // �X���[�̃|�C���^
};

#endif

