//==========================================================
//
// �S�[�� [goal.h]
// Author : �O�엳�V��
//
//==========================================================
#ifndef _GOAL_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _GOAL_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "task.h"
#include "list.h"
//#include "task.h"	// ����Ńt�@�C���C���N���[�h�ł��܂�
//�O���錾
class CMeshCylinder;
//==========================================================
// �T���v���̃N���X��`
//==========================================================
class CGole : public CTask
{

public:	// �N�ł��A�N�Z�X�\

	CGole();	// �R���X�g���N�^(�I�[�o�[���[�h)
	~CGole();	// �f�X�g���N�^

	// �����o�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	static CGole* Create(D3DXVECTOR3 pos,float fRange,float fLimit);

	
	static Clist<CGole*>* GetInstance() { if (pList == nullptr) { pList = pList->Create(); }return pList; }		// ���X�g�擾
	static void ListRelease() { if (pList != nullptr) { delete pList; pList = nullptr; } }					// ���X�g���
private:	// �����������A�N�Z�X�\

	// �����o�֐�
	bool CheckRange();
	bool CheckSpeed();
	// �����o�ϐ�
	D3DXVECTOR3 m_pos;				// ���W
	float m_fRange;					// �S�[���͈̔�
	float m_fLimit;					// ���x����
	CMeshCylinder* pMesh;

	static Clist<CGole*>* pList;	// �����̃��X�g*GetInstance()�o�R�ŃA�N�Z�X���鎖*
};

#endif