//==========================================================
//
// �p�g�J�[AI���� [police_AI.h]
// Author : Riku Nakamura
//
//==========================================================
#ifndef _POLICE_AI_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _POLICE_AI_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "main.h"
#include <list>

// �O���錾
class CRoad;

//==========================================================
// �x�@AI�̃N���X��`
//==========================================================
class CPoliceAI
{
public:	// �N�ł��A�N�Z�X�\

	CPoliceAI();	// �R���X�g���N�^
	virtual ~CPoliceAI();	// �f�X�g���N�^

	// �����o�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	static CPoliceAI*Create(void);

	// �����o�֐�(�擾)

	// �����o�֐�(�ݒ�)

protected:

	// ��Ԋ֐�

	// ��Ԑݒ�֐�
	
	// �����o�֐�

	// �����o�ϐ�

private:	// �����������A�N�Z�X�\

	// �����o�֐�

	// �����o�ϐ�

};

#endif
