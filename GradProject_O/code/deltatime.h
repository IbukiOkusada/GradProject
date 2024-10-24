//==========================================================
//
// �f���^�^�C�� [deltatime.cpp]
// Author : Ryosuke Ohara
//
//==========================================================
#ifndef _DELTATIME_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _DELTATIME_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include <iostream>
#include <chrono>

//==========================================================
// �f���^�^�C���̃N���X��`
//==========================================================
class CDeltaTime
{

public:	// �N�ł��A�N�Z�X�\

	CDeltaTime();	// �R���X�g���N�^(�I�[�o�[���[�h)
	~CDeltaTime();	// �f�X�g���N�^

	// �����o�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	static CDeltaTime* Create(void);

	// �����o�֐�(�擾)
	float GetDeltaTime(void);

	// �����o�֐�(�ݒ�)

private:	// �����������A�N�Z�X�\

	// �^�C�}�[�̏��
	struct Timer
	{
		std::chrono::high_resolution_clock::time_point CurrentTime;  // ���݂̎���
		std::chrono::high_resolution_clock::time_point LastTime;     // �O��̎���
		std::chrono::duration<float> DeltaTime;                      // �f���^�^�C��
	};

	Timer m_Timer;  // �^�C�}�[�̏��

	// �����o�֐�

	// �����o�ϐ�
};

#endif
