//==========================================================
//
// �f���^�^�C�� [deltatime.cpp]
// Author : Ryosuke Ohara
//
//==========================================================
#include "deltatime.h"

//==========================================================
// �ÓI�����o�ϐ��錾
//==========================================================
CDeltaTime *CDeltaTime::m_pInstance = nullptr;

//==========================================================
// �萔��`
//==========================================================
namespace
{
	const int MAX_SLOW_TIME = 10;    // �X���[�̌��E����
	const int BOOST_CNT = 3;        // �X���[�ʑ����^�C�}�[
	const float SLOW_RATE = 60.0f;  // �X���[�̔{��
}

//==========================================================
// �R���X�g���N�^
//==========================================================
CDeltaTime::CDeltaTime()
{
	m_Timer.CurrentTime = std::chrono::high_resolution_clock::now();
	m_Timer.LastTime = std::chrono::high_resolution_clock::now();
	m_fAdd = 0.0f;
	m_pInstance = this;
}

//==========================================================
// �f�X�g���N�^
//==========================================================
CDeltaTime::~CDeltaTime()
{

}

//==========================================================
// ����������
//==========================================================
HRESULT CDeltaTime::Init(void)
{
	m_fAdd = 1.0f;

	return S_OK;
}

//==========================================================
// �I������
//==========================================================
void CDeltaTime::Uninit(void)
{
	// �g�p���Ă��Ȃ���Ԃɂ���
	m_pInstance = nullptr;
}

//==========================================================
// �X�V����
//==========================================================
void CDeltaTime::Update(void)
{
	m_Timer.CurrentTime = std::chrono::high_resolution_clock::now();  // ���݂̎��Ԃ��擾
	m_Timer.DeltaTime = m_Timer.CurrentTime - m_Timer.LastTime;       // ���݂̎��ԂƑO��̎��Ԃ̍������v�Z
	m_Timer.LastTime = m_Timer.CurrentTime;                           // �O��̎��ԂɌ��݂̎��Ԃ���

	// �X���[
	Slow();
}

//==========================================================
// ��������
//==========================================================
CDeltaTime* CDeltaTime::Create(void)
{
	CDeltaTime* pDeltaTime = nullptr;

	if (pDeltaTime == nullptr)
	{
		pDeltaTime = DEBUG_NEW CDeltaTime;
		pDeltaTime->Init();
	}

	return pDeltaTime;
}

//==========================================================
// �f���^�^�C���擾����
//==========================================================
float CDeltaTime::GetDeltaTime(void)
{
	return m_Timer.DeltaTime.count() * m_fAdd;
}

//==========================================================
// �X���[����
//==========================================================
void CDeltaTime::Slow(void)
{
	//// �t���O�������Ă��Ȃ��Ƃ������𔲂���
	//if (!m_bSlow)
	//	return;

	//if (m_nSlowTime > 0)
	//{// �X���[�ɂ���

	//	m_SlowTimeCount++;	// �J�E���g�A�b�v

	//	if (m_SlowTimeCount % m_nSlowTime == 0)
	//	{// �X���[�̋K��l�𒴂����ꍇ

	//		m_SlowTimeCount = 0;

	//		m_nSlowTime--;
	//	}
	//}

	//if (m_nSlowTime > 0)
	//{
	//	m_fAdd = (SLOW_RATE / (float)(m_nSlowTime)) * m_Timer.DeltaTime.count();	// �ʒu�t���[���ӂ�̔{��
	//}
	//else
	//{
	//	m_fAdd = SLOW_RATE * m_Timer.DeltaTime.count();

	//	m_bSlow = false;
	//}
}