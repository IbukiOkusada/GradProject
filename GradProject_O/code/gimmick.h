//==========================================================
//
// �M�~�b�N [Gimmick.h]
// Author : �O�엳�V��
//
//==========================================================
#ifndef _Gimmick_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _Gimmick_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "task.h"
//#include "task.h"	// ����Ńt�@�C���C���N���[�h�ł��܂�

//==========================================================
// �T���v���̃N���X��`
//==========================================================
class CGimmick : public CTask
{

public:	// �N�ł��A�N�Z�X�\

	CGimmick();	// �R���X�g���N�^(�I�[�o�[���[�h)
	~CGimmick();	// �f�X�g���N�^

	// �����o�֐�
	virtual HRESULT Init(void) = 0;
	virtual void Uninit(void)= 0;
	virtual void Update(void)= 0;

private:	// �����������A�N�Z�X�\

	// �����o�֐�

	// �����o�ϐ�
	D3DXVECTOR3 m_pos;
	D3DXVECTOR3 m_rot;
	D3DXVECTOR3 m_scale;

};

#endif

