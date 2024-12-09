//==========================================================
//
// �G�f�B�b�g��] [edit_arrow.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _EDIT_ROTATE_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _EDIT_ROTATE_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "edit_handle.h"

//==========================================================
// �T���v���̃N���X��`
//==========================================================
class CEdit_Rotate : public CEdit_Handle
{
public:	// �N�ł��A�N�Z�X�\

	CEdit_Rotate();	// �R���X�g���N�^(�I�[�o�[���[�h)
	~CEdit_Rotate() override;	// �f�X�g���N�^

	// �����o�֐�
	virtual HRESULT Init(void) override;
	static CEdit_Rotate* Create(const D3DXVECTOR3& pos);

private:	// �����������A�N�Z�X�\

	void Rotation() override;
};

#endif

