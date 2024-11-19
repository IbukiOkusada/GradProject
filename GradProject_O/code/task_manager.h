//==========================================================
//
// �^�X�N�Ǘ��̏��� [task_manager.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _TASK_MANAGER_H_	// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _TASK_MANAGER_H_	// ��d�C���N���[�h�h�~�p�}�N�����`

#include "list_manager.h"
#include "list.h"

// �O���錾
class CTask;

//==========================================================
// �^�X�N�}�l�[�W���[�̃N���X��`
//==========================================================
class CTaskManager : public CListManager
{
private:
	CTaskManager();	// �R���X�g���N�^
	~CTaskManager();	// �f�X�g���N�^

public:
	// ���X�g�Ǘ������o�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void ListIn(CTask *pTask);

	// �V���O���g��
	static CTaskManager* GetInstance(void);
	static void Release(void);

private:	// �����������A�N�Z�X�\

	// �����o�֐�
	Clist<CTask*>* GetList() { if (m_plist == nullptr) { m_plist = m_plist->Create(); }return m_plist; }	// ���X�g�擾
	void ListRelease() { if (m_plist != nullptr) { delete m_plist; m_plist = nullptr; } }			// ���X�g���
	void ReleaseAll(void);
	void UpdateAll(void);
	void DeathCheck(void);

	// �����o�ϐ�
	Clist<CTask*>* m_plist;
	static CTaskManager *m_pInstance;
};

#endif


