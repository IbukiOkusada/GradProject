//===============================================
//
// �t�F�[�h�̏����S�� [fade.h]
// Author : Ibuki Okusada
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

private:	// �����������A�N�Z�X�\�Ȓ�`

	void CalTime();

	// �����o�ϐ�
	CNumber* m_pObject[5];		// �`��I�u�W�F�N�g
	CObject2D* m_pDecPoint;		// �����_�̃I�u�W�F�N�g

	float m_LimitTime;
	int m_Time[5];
};

#endif
