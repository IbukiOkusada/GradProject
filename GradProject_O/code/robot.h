//==========================================================
//
// �T���v���^�X�N(���X�g�Ǘ��^�X�N) [sample.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _SAMPLE_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _SAMPLE_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "task.h"
//#include "task.h"	// ����Ńt�@�C���C���N���[�h�ł��܂�

//==========================================================
// �T���v���̃N���X��`
//==========================================================
class CRobot : public CTask
{

public:	// �N�ł��A�N�Z�X�\

	CRobot();	// �R���X�g���N�^(�I�[�o�[���[�h)
	~CRobot();	// �f�X�g���N�^

	// �����o�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	static CRobot* Create(void);

	// �����o�֐�(�擾)
	CRobot* GetNext(void) { return m_pNext; }
	CRobot* GetPrev(void) { return m_pPrev; }

	// �����o�֐�(�ݒ�)
	void SetNext(CRobot* pNext) { m_pNext = pNext; }
	void SetPrev(CRobot* pPrev) { m_pPrev = pPrev; }

private:	// �����������A�N�Z�X�\

	// �����o�֐�

	// �����o�ϐ�
	CRobot* m_pPrev;			// �O�̃I�u�W�F�N�g�ւ̃|�C���^
	CRobot* m_pNext;			// ���̃I�u�W�F�N�g�ւ̃|�C���^
};

#endif

