//==========================================================
//
// �G�f�B�b�g�S�[�� [edit_robot.h]
// Author : Ryosuke Oohara
//
//==========================================================
#ifndef _EDIT_ROBOT_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _EDIT_ROBOT_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "edit.h"

// �O���錾
class CGoal;
class CEdit_Handle;
class CRobot;

//==========================================================
// ���{�b�g�z�u�G�f�B�^�̃N���X��`
//==========================================================
class CEdit_Robot : public CEdit
{
public:	// �N�ł��A�N�Z�X�\

	CEdit_Robot();	// �R���X�g���N�^(�I�[�o�[���[�h)
	~CEdit_Robot();	// �f�X�g���N�^

	// �����o�֐�
	virtual HRESULT Init(void) override;
	virtual void Uninit(void);
	virtual void Update(void) override;

	// �����o�֐�(�擾)

	// �����o�֐�(�ݒ�)

private:	// �����������A�N�Z�X�\

	// �����o�֐�
	void ClickCheck();	// �I���m�F
	void Delete();		// �폜
	void Move();		// �ړ�
	void Save();		// �ۑ�
	void Create();		// ����
	void ModeChange();  // �n���h���̏�ԕω�
	bool CursorCollision(CRobot* pRobot);
	bool TriangleCollision(const D3DXVECTOR3& rayOrigin, const D3DXVECTOR3& rayDir,
		const D3DXVECTOR3& pos0, const D3DXVECTOR3& pos1, const D3DXVECTOR3& pos2);

	// �����o�ϐ�
	CGoal* m_pSelect;	// �I��������
	CRobot* m_pRobot;   // ���󂵂����{�b�g
	CEdit_Handle* m_pHandle;	// ���
	float m_fMouseWheel;	// �}�E�X�z�C�[���̈ړ���
	float m_fMoveLength;	// �ړ�����
};

#endif