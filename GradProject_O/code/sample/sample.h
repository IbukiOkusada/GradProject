//==========================================================
//
// �T���v���^�X�N(���X�g�Ǘ��^�X�N) [sample.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _SAMPLE_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _SAMPLE_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "../task.h"
//#include "task.h"	// ����Ńt�@�C���C���N���[�h�ł��܂�

//==========================================================
// �T���v���̃N���X��`
//==========================================================
class CSample : public CTask
{

public:	// �N�ł��A�N�Z�X�\

	CSample();	// �R���X�g���N�^(�I�[�o�[���[�h)
	~CSample();	// �f�X�g���N�^

	// �����o�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	static CSample* Create(void);

	// �����o�֐�(�擾)
	CSample* GetNext(void) { return m_pNext; }
	CSample* GetPrev(void) { return m_pPrev; }

	// �����o�֐�(�ݒ�)
	void SetNext(CSample* pNext) { m_pNext = pNext; }
	void SetPrev(CSample* pPrev) { m_pPrev = pPrev; }


private:	// �����������A�N�Z�X�\

	// �����o�֐�

	// �����o�ϐ�
	CSample* m_pPrev;			// �O�̃I�u�W�F�N�g�ւ̃|�C���^
	CSample* m_pNext;			// ���̃I�u�W�F�N�g�ւ̃|�C���^
};

#endif

