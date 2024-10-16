//==========================================================
//
// �T���v���^�X�N(���X�g�Ǘ��^�X�N) [sample.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _LIST_MANAGER_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _LIST_MANAGER_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

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

	// �����o�֐�(�擾)
	CListManager* GetNext(void) { return m_pNext; }
	CListManager* GetPrev(void) { return m_pPrev; }

	// �����o�֐�(�ݒ�)
	void SetNext(CListManager* pNext) { m_pNext = pNext; }
	void SetPrev(CListManager* pPrev) { m_pPrev = pPrev; }

private:	// �����������A�N�Z�X�\

	// �����o�֐�

	// �����o�ϐ�
	CListManager* m_pPrev;			// �O�̃I�u�W�F�N�g�ւ̃|�C���^
	CListManager* m_pNext;			// ���̃I�u�W�F�N�g�ւ̃|�C���^
};

#endif


