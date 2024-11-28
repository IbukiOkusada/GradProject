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

	void Update(void);   // �X�V����
	void SetInfoList(std::vector<CGoal::SInfo>& info) { m_InfoList = info; }

	static CGoalManager *Create(void);  // ��������
	static void Release();

	// �擾�n
	static CGoalManager *GetInstance(void) { return m_pInstance; }
	std::vector<CGoal::SInfo>* GetInfoList() { return &m_InfoList; }

private:

	HRESULT Init(void);  // ����������
	void Uninit(void);   // �I������

	std::vector<CGoal::SInfo> m_InfoList;
	CGoal*m_pGoal;  // �S�[���̃|�C���^
	static CGoalManager *m_pInstance;  // ���g�̃|�C���^
};

#endif