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
	float GetDeltaTime(void);                                      // �f���^�^�C��
	static CDeltaTime *GetInstance(void) { return m_pInstance; }  // �������g

	// �����o�֐�(�ݒ�)

private:	// �����������A�N�Z�X�\

	// �^�C�}�[�̏��
	struct Timer
	{
		std::chrono::high_resolution_clock::time_point CurrentTime;  // ���݂̎���
		std::chrono::high_resolution_clock::time_point LastTime;     // �O��̎���
		std::chrono::duration<float> DeltaTime;                      // ����
	};

	// �����o�֐�
	void Slow(void);  // �X���[


	// �����o�ϐ�
	static CDeltaTime *m_pInstance;  // �f���^�^�C���̃|�C���^
	Timer m_Timer;  // �^�C�}�[�̏��
	float m_fTime = 0.0f;
	int m_nSlowTime = 0;
	int m_SlowTimeCount = 0;
	float m_fAdd;
	bool m_bSlow = false;

};

#endif
