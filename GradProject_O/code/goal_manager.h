//==========================================================
//
// �S�[���Ǘ��̏��� [goal_manager.h]
// Author : Ryosuke Ohara
//
//==========================================================
#ifndef _GOAL_MANAGER_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _GOAL_MANAGER_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

class CGoalManager
{
public:
	CGoalManager();
	~CGoalManager();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);

private:


};

#endif