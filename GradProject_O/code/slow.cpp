//==========================================================
//
// �X���[�̏����Ǘ� [slow.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "main.h"
#include "slow.h"
#include "manager.h"
#include "deltatime.h"
#include "debugproc.h"
#include "input_keyboard.h"

//==========================================================
// �ÓI�����o�ϐ��錾
//==========================================================
CSlow *CSlow::m_pInstance = nullptr;

//==========================================================
// �萔��`
//==========================================================
namespace {

	const int MAX_SLOWTIME = 10;  // �X���[�̌��E����
	const int BOOST_CNT = 3;      // �X���[�ʑ����^�C�}�[
	const float SLOW_RATE = 60.0f;
}

//==========================================================
// �R���X�g���N�^
//==========================================================
CSlow::CSlow()
{
	// �l�̃N���A
	m_b = false;
	m_bUse = false;
	m_nTimeCnt = 0;
	m_nTimer = 0;
	m_fAdd = 0.0f;
	m_DeltaTime = 0.0f;
	m_pInstance = this;
}

//==========================================================
// �f�X�g���N�^
//==========================================================
CSlow::~CSlow()
{

}

//==========================================================
// ����������
//==========================================================
void CSlow::Init(void)
{
	// �l�̃N���A
	m_b = false;
	m_bUse = false;
	m_nTimeCnt = 0;
	m_nTimer = 0;

	CDeltaTime* pDeltaTime = CDeltaTime::GetInstance();
	
	if (pDeltaTime != nullptr)
		m_DeltaTime = pDeltaTime->GetDeltaTime();
	

	m_fAdd = SLOW_RATE * m_DeltaTime;
}

//==========================================================
// �I������
//==========================================================
void CSlow::Uninit(void)
{
	// �g�p���Ă��Ȃ���Ԃɂ���
	m_pInstance = nullptr;
}

//==========================================================
// �X�V����
//==========================================================
void CSlow::Update(void)
{
	CDeltaTime* pDeltaTime = CDeltaTime::GetInstance();
	
	if (pDeltaTime != nullptr)
		m_DeltaTime = pDeltaTime->GetDeltaTime();

	m_fMulOld = m_fAdd;

	//if (m_nTimer > 0)
	//{// �X���[�ɂ���
	//	m_nTimeCnt++;	// �J�E���g�A�b�v
	//	if (m_nTimeCnt % m_nTimer == 0)
	//	{// �X���[�̋K��l�𒴂����ꍇ
	//		m_nTimeCnt = 0;

	//		if (m_bUse == false)
	//		{//�g�p���Ȃ��ꍇ
	//			m_nTimer--;
	//		}
	//		m_b = true;
	//	}
	//	else
	//	{// �K��l�ł͂Ȃ�
	//		m_b = false;
	//	}
	//}
	//else
	//{// ���Ȃ��ꍇ
	//	m_b = true;
	//}

	if (m_nTimer > 0)
	{
		m_fAdd = (SLOW_RATE / (float)(m_nTimer)) * m_DeltaTime;	// �ʒu�t���[���ӂ�̔{��
	}
	else
	{
		m_fAdd = SLOW_RATE * m_DeltaTime;
	}

	//if (CInputKeyboard::GetInstance()->GetTrigger(DIK_F3))
	//{
	//	m_nTimer = 10;
	//}
}

//==========================================================
// �X���[��Ԏ擾
//==========================================================
float CSlow::Get(void)
{
	return m_fAdd;
}

//==========================================================
// �X���[��ԗʐݒ�
//==========================================================
void CSlow::SetSlow(bool bUse)
{
	// �g�p��Ԑݒ�
	m_bUse = bUse;

	if (m_bUse == true)
	{// �g�p����ꍇ
		m_nBoostCnt++;

		if (m_nBoostCnt % BOOST_CNT == 0)
		{// �K��l�ɂȂ����ꍇ
			m_nBoostCnt = 0;

			if (m_nTimer < MAX_SLOWTIME)
			{// �X���[�̍ő�l�𒴂��Ă��Ȃ��ꍇ
				m_nTimer++;
			}
		}
	}
}

//==========================================================
// �O��̔{�����擾
//==========================================================
float CSlow::GetOld(void)
{
	return m_fMulOld;
}