//==========================================================
//
// �G�f�B�b�g�� [edit_road.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _EDIT_ROAD_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _EDIT_ROAD_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "edit.h"

//==========================================================
// �T���v���̃N���X��`
//==========================================================
class CEdit_Road : public CEdit
{
public:	// �N�ł��A�N�Z�X�\

	CEdit_Road();	// �R���X�g���N�^(�I�[�o�[���[�h)
	~CEdit_Road();	// �f�X�g���N�^

	// �����o�֐�
	virtual HRESULT Init(void) override;
	virtual void Uninit(void);
	virtual void Update(void) override;

	// �����o�֐�(�擾)

	// �����o�֐�(�ݒ�)

private:	// �����������A�N�Z�X�\

	// �����o�֐�

	// �����o�ϐ�
};

#endif


