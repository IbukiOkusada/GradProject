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
	CGoalManager();
	~CGoalManager();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);

	// �擾�n
	static CGoalManager *GetInstance(void) { return m_Instance; }

private:

	CGoal *m_pGoal;  // �S�[���̃|�C���^
	static CGoalManager *m_Instance;  // ���g�̃|�C���^
};

#endif