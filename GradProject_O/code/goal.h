//==========================================================
//
// �S�[�� [goal.h]
// Author : �O�엳�V��
//
//==========================================================
#ifndef _GOAL_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _GOAL_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "task.h"
#include "list.h"
//#include "task.h"	// ����Ńt�@�C���C���N���[�h�ł��܂�

//==========================================================
// �T���v���̃N���X��`
//==========================================================
class CGole : public CTask
{

public:	// �N�ł��A�N�Z�X�\

	CGole();	// �R���X�g���N�^(�I�[�o�[���[�h)
	~CGole();	// �f�X�g���N�^

	// �����o�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	static CGole* Create(void);

	
	Clist<CGole *> GetList() {
		if (pList == NULL)
		{
			pList = new Clist<CGole *>;
		}
	}

private:	// �����������A�N�Z�X�\

	// �����o�֐�

	// �����o�ϐ�
	static Clist<CGole *> * pList
};

#endif