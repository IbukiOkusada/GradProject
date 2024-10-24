//==========================================================
//
// �f���^�^�C�� [deltatime.cpp]
// Author : Ryosuke Ohara
//
//==========================================================
#include "deltatime.h"
#include "debugproc.h"

//==========================================================
// �R���X�g���N�^
//==========================================================
CDeltaTime::CDeltaTime()
{
	m_Timer.CurrentTime = std::chrono::high_resolution_clock::now();
	m_Timer.LastTime = std::chrono::high_resolution_clock::now();
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
	

	return S_OK;
}

//==========================================================
// �I������
//==========================================================
void CDeltaTime::Uninit(void)
{

}

//==========================================================
// �X�V����
//==========================================================
void CDeltaTime::Update(void)
{
	m_Timer.CurrentTime = std::chrono::high_resolution_clock::now();  // ���݂̎��Ԃ��擾
	m_Timer.DeltaTime = m_Timer.CurrentTime - m_Timer.LastTime;       // ���݂̎��ԂƑO��̎��Ԃ̍������v�Z
	m_Timer.LastTime = m_Timer.CurrentTime;      
	
	CManager::GetInstance()->GetDebugProc()->Print("[ �ł邽�`������ : %f ]\n", m_Timer.DeltaTime.count());
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
	return m_Timer.DeltaTime.count();
}