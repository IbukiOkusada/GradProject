//==========================================================
//
// ���W�I [Radio.cpp]
// Author : �O�엳�V��
//
//==========================================================
#include "Radio.h"
#include "input.h"
#include "input_gamepad.h"
#include "input_keyboard.h"
// �}�N����`
namespace
{
	std::vector<std::vector<const char*>> Path =
	{
		{//�����b�N�X
			"data\\BGM\\morning_jazz.wav",
			"data\\BGM\\moment.wav",
		},

		{//���^��
			"data\\BGM\\power_is_you.wav",
			"data\\BGM\\rammstein_style.wav",
		},

		{//�e�N�m
			"data\\BGM\\mantra.wav",
			"data\\BGM\\theme_90s.wav",
		},

		{//�|�b�v
			"data\\BGM\\maou_14_shining_star.wav",
			"data\\BGM\\typhoon.wav",
		},

		{//�E��
			"data\\BGM\\AcceleHeartLove.wav",
			"data\\BGM\\BGM_ShinobiDrift.wav",
		},

		{//�N���V�b�N
			"data\\BGM\\gline.wav",
			"data\\BGM\\nocturne-op9-2_1.wav",
		},
	};
	const char* BUTTON_SE = { "data\\SE\\switch_04_button.wav" };
};
//==========================================================
// �R���X�g���N�^
//==========================================================
CRadio::CRadio()
{
	// �l�̃N���A
	 m_fVolume = 0.25f;
	 m_nChannel = 0;
	 m_bSwitch = false;
	 m_fFade = 0.0f;
}

//==========================================================
// �f�X�g���N�^
//==========================================================
CRadio::~CRadio()
{

}

//==========================================================
// ����������
//==========================================================
HRESULT CRadio::Init(void)
{
	for (int i = 0; i < CRadio::CHANNEL_MAX; i++)
	{
		m_pRadio[i].m_pList = DEBUG_NEW Clist<CMasterSound::CObjectSound*>;
		m_pRadio[i].nCurrent = rand()% Path[i].size();
		for (int j = 0; j < (int)Path[i].size(); j++)
		{
			m_pRadio[i].m_pList->Regist(CMasterSound::CObjectSound::Create(Path[i][j], 0));
			m_pRadio[i].m_pList->Get(j)->SetVolume(0.0f);
			if (m_pRadio[i].nCurrent != j)
			{
				m_pRadio[i].m_pList->Get(j)->Stop();
			}
		}
	}
	m_pSE = CMasterSound::CObjectSound::Create(BUTTON_SE, 0);
	m_pSE->Stop();
	return S_OK;
}

//==========================================================
// �I������
//==========================================================
void CRadio::Uninit(void)
{
	for (int i = 0; i < CRadio::CHANNEL_MAX; i++)
	{
		int nNum = m_pRadio[i].m_pList->GetNum();
		for (int j = nNum - 1; j >= 0; j--)
		{
			CMasterSound::CObjectSound* pSound = m_pRadio[i].m_pList->Get(j);
		
			m_pRadio[i].m_pList->Delete(pSound);
		
			SAFE_UNINIT_DELETE(pSound);
		
		}
		SAFE_DELETE(m_pRadio[i].m_pList);
	}
	SAFE_UNINIT_DELETE(m_pSE);

}

//==========================================================
// �X�V����
//==========================================================
void CRadio::Update(void)
{
	m_fFade += (1.0f - m_fFade) * 0.05f;
	VolumeChange();
	ChannelChange();
	for (int i = 0; i < CRadio::CHANNEL_MAX; i++)
	{
	
		if (m_nChannel == i)
		{
			m_pRadio[i].m_pList->Get(m_pRadio[i].nCurrent)->SetVolume(m_fVolume * m_fFade);
		}
		else
		{
			m_pRadio[i].m_pList->Get(m_pRadio[i].nCurrent)->SetVolume(0.0f);
		}

		if (!m_pRadio[i].m_pList->Get(m_pRadio[i].nCurrent)->GetPlay())
		{

			m_pRadio[i].m_pList->Get(m_pRadio[i].nCurrent)->Stop();
			m_pRadio[i].nCurrent++;
			m_pRadio[i].nCurrent %= m_pRadio[i].m_pList->GetNum();
			m_pRadio[i].m_pList->Get(m_pRadio[i].nCurrent)->Play();
		}

	}

}
//==========================================================
// �{�����[���ݒ�
//==========================================================
void CRadio::VolumeChange(void)
{
	CInputKeyboard* pInputKey = CInputKeyboard::GetInstance();	// �L�[�{�[�h�̃|�C���^
	CInputPad* pInputPad = CInputPad::GetInstance();
	if (pInputKey->GetPress(DIK_UP) || pInputPad->GetPress(CInputPad::BUTTON_UP, 0))
	{
		m_fVolume += 0.01f;
	}
	else if (pInputKey->GetPress(DIK_DOWN) || pInputPad->GetPress(CInputPad::BUTTON_DOWN, 0))
	{
		m_fVolume -= 0.01f;
	}
	if (m_fVolume < 0.0f)
	{
		m_fVolume = 0.0f;
	}
	if (m_fVolume > 1.0f)
	{
		m_fVolume = 1.0f;
	}
}
//==========================================================
// �`�����l���ݒ�
//==========================================================
void CRadio::ChannelChange(void)
{
	CInputKeyboard* pInputKey = CInputKeyboard::GetInstance();	// �L�[�{�[�h�̃|�C���^
	CInputPad* pInputPad = CInputPad::GetInstance();
	if (pInputKey->GetTrigger(DIK_RIGHT) || pInputPad->GetTrigger(CInputPad::BUTTON_RIGHT, 0))
	{
		m_nChannel += 1;
		m_pSE->Play();
		m_fFade = 0.0f;
	}
	else if (pInputKey->GetTrigger(DIK_LEFT) || pInputPad->GetTrigger(CInputPad::BUTTON_LEFT, 0))
	{
		m_nChannel -= 1;
		m_pSE->Play();
		m_fFade = 0.0f;
	}
	m_nChannel %= CHANNEL_MAX;
	if (m_nChannel < 0)
	{
		m_nChannel = CHANNEL_MAX - 1;
	}
}
//==========================================================
// ����
//==========================================================
CRadio* CRadio::Create(void)
{
	CRadio* pRadio = nullptr;

	pRadio = DEBUG_NEW CRadio;

	if (pRadio != nullptr)
	{
		// ����������
		pRadio->Init();
	}

	return pRadio;
}
