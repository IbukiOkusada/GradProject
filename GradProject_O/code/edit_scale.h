//==========================================================
//
// �G�f�B�b�g�X�P�[�� [edit_scale.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _EDIT_SCALE_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _EDIT_SCALE_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "edit_handle.h"

//==========================================================
// �T���v���̃N���X��`
//==========================================================
class CEdit_Scale : public CEdit_Handle
{
public:	// �N�ł��A�N�Z�X�\

	CEdit_Scale();	// �R���X�g���N�^(�I�[�o�[���[�h)
	~CEdit_Scale() override;	// �f�X�g���N�^

	// �����o�֐�
	virtual HRESULT Init(void) override;

private:	// �����������A�N�Z�X�\

	void Scale() override;
};

#endif
