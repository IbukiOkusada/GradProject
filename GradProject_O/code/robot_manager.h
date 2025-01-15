//==========================================================
//
// ���{�b�g�}�l�[�W���[ [robot_manager.h]
// Author : Kenta Hashimoto
//
//==========================================================
#ifndef _ROBOTMANAGER_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _ROBOTMANAGER_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

// �O���錾
class CRobot;

#include "list_manager.h"

//==========================================================
// ���{�b�g�}�l�[�W���[�̃N���X��`
//==========================================================
class CRobotManager : public CListManager
{
private:

	CRobotManager();		// �R���X�g���N�^
	~CRobotManager();	// �f�X�g���N�^

public:	// �N�ł��A�N�Z�X�\

	// �����o�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	static CRobotManager* GetInstance(void);
	static void Release(void);
	CRobot* GetTop(void) { return m_pTop; }
	CRobot* GetCur(void) { return m_pCur; }
	void ListIn(CRobot* pSample);
	void ListOut(CRobot* pSample);
	int GetNum(void) { return m_nNum; }

private:	// �����������A�N�Z�X�\

	// �����o�֐�


	// �����o�ϐ�
	CRobot* m_pTop;	// �擪
	CRobot* m_pCur;	// �Ō��
	int m_nNum;
	static CRobotManager* m_pInstance;	// �C���X�^���X
};

#endif

