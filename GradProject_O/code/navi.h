//==========================================================
//
// �i�r [navi.h]
// Author : �O�엳�V��
//
//==========================================================
#ifndef _Navi_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _Navi_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "task.h"
#include "camera.h"
//#include "task.h"	// ����Ńt�@�C���C���N���[�h�ł��܂�

//==========================================================
// �T���v���̃N���X��`
//==========================================================
class CNavi : public CTask
{

public:	// �N�ł��A�N�Z�X�\

	CNavi();	// �R���X�g���N�^(�I�[�o�[���[�h)
	~CNavi();	// �f�X�g���N�^

	// �����o�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	static CNavi* Create(void);

	// �����o�֐�(�擾)

	// �����o�֐�(�ݒ�)

private:	// �����������A�N�Z�X�\

	// �����o�֐�
	
	// �����o�ϐ�
	CMultiCamera* m_pCamera;
};

#endif
