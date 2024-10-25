//==========================================================
//
// �T���v���^�X�N(���X�g�Ǘ��^�X�N) [sample.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _LIST_MANAGER_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _LIST_MANAGER_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "list.h"

//==========================================================
// �T���v���̃N���X��`
//==========================================================
class CListManager
{

public:	// �N�ł��A�N�Z�X�\

	CListManager();	// �R���X�g���N�^(�I�[�o�[���[�h)
	~CListManager();	// �f�X�g���N�^

	// �����o�֐�
	virtual void Uninit(void);

	static void Release();

private:	// �����������A�N�Z�X�\

	// �����o�֐�

	// �����o�ϐ�
	static Clist<CListManager*>* m_pList;
};

#endif


