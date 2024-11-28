//==========================================================
//
// �S�[���Ǘ��̏��� [goal_manager.h]
// Author : Ryosuke Ohara
//
//==========================================================
#ifndef _GOAL_MANAGER_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _GOAL_MANAGER_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "goal.h"

//==========================================================
// �S�[���Ǘ��̃N���X��`
//==========================================================
class CGoalManager
{
public:
	CGoalManager();      // �R���X�g���N�^
	~CGoalManager();     // �f�X�g���N�^

	HRESULT Init(void);  // ����������
	void Update(void);   // �X�V����
	void SetInfoList(std::vector<CGoal::SInfo>& info) { m_InfoList = info; }
	void GoalCreate(int nId);
	bool ListIn(CGoal* pGoal);
	bool ListOut(CGoal* pGoal);

	static CGoalManager *Create(void);  // ��������
	static void Release();

	// �擾�n
	static CGoalManager *GetInstance(void) { 
		Create();
		return m_pInstance; 
	}
	std::vector<CGoal::SInfo>* GetInfoList() { return &m_InfoList; }
	CGoal* GetGoal(int nIdx);
	std::map<int, CGoal*>* GetList() { return &m_List; }
	int GetCreateIdx() { return m_nNumCreate; }
	int GetNextIdx() { return m_nNextIdx; }

private:

	void Uninit(void);   // �I������

	int m_nNextIdx;
	int m_nOldIdx;
	int m_nNumCreate;
	std::vector<CGoal::SInfo> m_InfoList;
	CGoal* m_pGoal;		// �S�[���̃|�C���^
	CGoal* m_pGoalOld;  // �O��̃S�[���̃|�C���^
	static CGoalManager *m_pInstance;  // ���g�̃|�C���^
	std::map<int, CGoal*> m_List;	// ���X�g
};

#endif