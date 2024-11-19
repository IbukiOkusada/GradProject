//===============================================
//
// �^�C�}�[�̏��� [timer.h]
// Author : Kenta Hashimoto
//
//===============================================
#ifndef _TIMER_H_
#define _TIMER_H_

#include "main.h"	//main.h�Œ�`���Ă�����̂��K�v�Ȃ���include
#include "manager.h"
#include "Number.h"
#include "task.h"

//===============================================
// �^�C�}�[�N���X�̒�`(�h���N���X)
//===============================================
class CTimer
{
public:	// �N�ł��A�N�Z�X�\�Ȓ�`


public:	// �N�ł��A�N�Z�X�\

	CTimer();	// �R���X�g���N�^
	~CTimer();	// �f�X�g���N�^

	// �����o�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	static CTimer* Create();

	static float GetTime() { return m_LimitTime; }

private:	// �����������A�N�Z�X�\�Ȓ�`

	void CalTime();				// �^�C�}�[�̌v�Z
	void BlinkingTime();		// �_�ŏ���

	// �����o�ϐ�
	CNumber* m_pObject[5];		// �`��I�u�W�F�N�g
	CObject2D* m_pDecPoint;		// �����_�̃I�u�W�F�N�g

	static float m_LimitTime;
	int m_Time[5];

	float m_Ratio;
};

#endif
