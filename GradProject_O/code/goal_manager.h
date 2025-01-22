//==========================================================
//
// �S�[���Ǘ��̏��� [goal_manager.h]
// Author : Ryosuke Ohara
//
//==========================================================
#ifndef _GOAL_MANAGER_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _GOAL_MANAGER_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "goal.h"
#include "map_list.h"
#include "list_manager.h"

//==========================================================
// �S�[���Ǘ��̃N���X��`
//==========================================================
class CGoalManager : public CListManager
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
	Cmaplist<CGoal*>& GetList() { return m_List; }
	int GetCreateIdx() { return m_nNumCreate; }
	int GetNextIdx() { return m_nNextIdx; }

	// �ݒ�n
	void SetNetId(int nIdx) { m_nNetId = nIdx; }
	void ResetGoal() { m_pGoal = nullptr; }

private:

	void Uninit(void);   // �I������

	int m_nNextIdx;	// ���̐���ID
	int m_nOldIdx;	// �O��̐���ID
	int m_nNowIdx;	// ���݂̐���ID
	int m_nNumCreate;	// ��������
	int m_nNetId;	// �ʐM�Ŏ󂯎�������̐����ԍ�
	std::vector<CGoal::SInfo> m_InfoList;
	CGoal* m_pGoal;		// �S�[���̃|�C���^
	CGoal* m_pGoalOld;  // �O��̃S�[���̃|�C���^
	bool m_bNextGoal;	// ���̃S�[�����ݒ肳��Ă���
	static CGoalManager *m_pInstance;  // ���g�̃|�C���^
	Cmaplist<CGoal*> m_List;
};

#endif