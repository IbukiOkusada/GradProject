//==========================================================
//
// �G�f�B�^�[�Ǘ��̏��� [edit_manager.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _EDIT_MANAGER_H_	// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _EDIT_MANAGER_H_	// ��d�C���N���[�h�h�~�p�}�N�����`

#include "edit.h"

//==========================================================
// �^�X�N�}�l�[�W���[�̃N���X��`
//==========================================================
class CEditManager
{
private:
	CEditManager();		// �R���X�g���N�^
	~CEditManager();	// �f�X�g���N�^

public:
	// ���X�g�Ǘ������o�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);

	// �V���O���g��
	static CEditManager* Create(void);
	static CEditManager* GetInstance(void);
	static void Release(void);

private:	// �����������A�N�Z�X�\

	// �����o�֐�
	void ChangeEdit(CEdit* pEdit);

	// �����o�ϐ�
	bool m_bFlag;	// �I���t���O
	CEdit::TYPE m_SelectType;
	CEdit* m_pEdit;	// �G�f�B�^�[�p�|�C���^
	static CEditManager* m_pInstance;
};

#endif
