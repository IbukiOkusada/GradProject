//==========================================================
//
// �G�f�B�b�g�S�[�� [edit_goal.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _EDIT_GOAL_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _EDIT_GOAL_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "edit.h"

// �O���錾
class CGoal;
class CEdit_Handle;

//==========================================================
// �T���v���̃N���X��`
//==========================================================
class CEdit_Goal : public CEdit
{
public:	// �N�ł��A�N�Z�X�\

	CEdit_Goal();	// �R���X�g���N�^(�I�[�o�[���[�h)
	~CEdit_Goal();	// �f�X�g���N�^

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
	bool CursorCollision(CGoal* pGoal);
	bool TriangleCollision(const D3DXVECTOR3& rayOrigin, const D3DXVECTOR3& rayDir,
		const D3DXVECTOR3& pos0, const D3DXVECTOR3& pos1, const D3DXVECTOR3& pos2);

	// �����o�ϐ�
	CGoal* m_pSelect;	// �I��������
	CEdit_Handle* m_pHandle;	// ���
	float m_fMouseWheel;	// �}�E�X�z�C�[���̈ړ���
	float m_fMoveLength;	// �ړ�����
};

#endif


