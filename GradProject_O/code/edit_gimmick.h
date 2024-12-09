//==========================================================
//
// �G�f�B�b�g�I�u�W�F�N�g [edit_object.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _EDIT_GIMMICK_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _EDIT_GIMMICK_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "edit.h"

class CGimmick;
class CEdit_Handle;

//==========================================================
// �T���v���̃N���X��`
//==========================================================
class CEdit_Gimmick : public CEdit
{
public:	// �N�ł��A�N�Z�X�\

	CEdit_Gimmick();	// �R���X�g���N�^(�I�[�o�[���[�h)
	~CEdit_Gimmick();	// �f�X�g���N�^

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
	void Scale();		// �X�P�[��
	void Rotate();		// ��]
	void Save();		// �ۑ�
	void Create();		// ����
	void ModeChange();	// �ύX
	bool CursorCollision(CGimmick* pGimmick);
	void ModelChange();	// ���f���ύX

	// �����o�ϐ�
	D3DXVECTOR3 m_startScale;
	D3DXVECTOR3 m_startRotate;
	CEdit_Handle* m_pHandle;	// ���
	float m_fMouseWheel;	// �}�E�X�z�C�[���̈ړ���
	CGimmick* m_pSelect;	// �I�𒆃I�u�W�F�N�g
};

#endif
