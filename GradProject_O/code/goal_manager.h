//==========================================================
//
// �S�[���Ǘ��̏��� [goal_manager.h]
// Author : Ryosuke Ohara
//
//==========================================================
#ifndef _GOAL_MANAGER_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _GOAL_MANAGER_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

// �O���錾
class CGoal;

//==========================================================
// �S�[���Ǘ��̃N���X��`
//==========================================================
class CGoalManager
{
public:
	CGoalManager();      // �R���X�g���N�^
	~CGoalManager();     // �f�X�g���N�^

	HRESULT Init(void);  // ����������
	void Uninit(void);   // �I������
	void Update(void);   // �X�V����

	static CGoalManager *Create(void);  // ��������

	// �擾�n
	static CGoalManager *GetInstance(void) { return m_pInstance; }

private:

	CGoal*m_pGoal;  // �S�[���̃|�C���^
	static CGoalManager *m_pInstance;  // ���g�̃|�C���^
};

#endif