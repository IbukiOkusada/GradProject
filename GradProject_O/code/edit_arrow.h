//==========================================================
//
// �G�f�B�b�g��� [edit_arrow.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _EDIT_ARROW_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _EDIT_ARROW_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "edit_handle.h"

//==========================================================
// �T���v���̃N���X��`
//==========================================================
class CEdit_Arrow : public CEdit_Handle
{
public:	// �N�ł��A�N�Z�X�\

	CEdit_Arrow();	// �R���X�g���N�^(�I�[�o�[���[�h)
	~CEdit_Arrow() override;	// �f�X�g���N�^

	// �����o�֐�
	virtual HRESULT Init(void) override;
	static CEdit_Arrow* Create(const D3DXVECTOR3& pos);

private:	// �����������A�N�Z�X�\

	void Move() override;
};

#endif
