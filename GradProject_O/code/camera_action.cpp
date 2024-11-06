//==========================================================
//
// �J�����A�N�V�����̏��� [camera_action.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "camera_action.h"
#include "camera.h"
#include "deltatime.h"

//==========================================================
// �R���X�g���N�^
//==========================================================
CCameraAction::CCameraAction()
{
	// �l�̃N���A
	m_bNext = false;
	m_bFinish = false;
	m_startInfo = SStartInfo();
	m_targetInfo = STargetInfo();
	m_time = STime();
}

//==========================================================
// �f�X�g���N�^
//==========================================================
CCameraAction::~CCameraAction()
{

}


//==========================================================
// ����������
//==========================================================
HRESULT CCameraAction::Init(void)
{
	return S_OK;
}



//==========================================================
// �I������
//==========================================================
void CCameraAction::Uninit(void)
{
	delete this;
}

//==========================================================
// �X�V����
//==========================================================
void CCameraAction::Update(CCamera* pCamera)
{
	if (pCamera == nullptr) { return; }
	if (m_bFinish) { return; }

	// �i�߂�
	if (!m_bPause && m_time.fNow < m_time.fEnd)
	{
		m_time.fNow += CDeltaTime::GetInstance()->GetDeltaTime();
	}

	D3DXVECTOR3 rot = m_startInfo.rot;
	float length = m_startInfo.fLength;

	// �^�C�����i��ł���
	if (m_time.fNow > 0)
	{
		D3DXVECTOR3 rotDiff = m_targetInfo.rot - rot;
		float lenDiff = m_targetInfo.fLength - length;
		float multi = m_time.fNow / m_time.fEnd;
		rot += rotDiff * multi;
		length += lenDiff * multi;
	}

	// ��ނ��Ƃɐݒ�
	pCamera->SetRotation(rot);
	pCamera->SetLength(length);

	switch (m_move)
	{
	case MOVE_POSV:
		pCamera->SetV();
		break;

	case MOVE_POSR:
		pCamera->SetV();
		break;
	default:
		break;
	}

	// ���ԏI��
	if (m_time.fNow >= m_time.fEnd)
	{
		m_time.fStopNow += CDeltaTime::GetInstance()->GetDeltaTime();

		if (m_time.fStopNow >= m_time.fStop)
		{
			m_bFinish = true;
		}
	}
}

//==========================================================
// �A�N�V�����ݒ�
//==========================================================
void CCameraAction::Set(CCamera* pCamera, const D3DXVECTOR3& rot, const float& fLength,
	const float fTime, const float fStopTime, const MOVE& move, const bool& bNext)
{
	if (pCamera == nullptr) { return; }

	// �����l�_�̐ݒ�
	m_startInfo.posV = pCamera->GetPositionV();
	m_startInfo.posR = pCamera->GetPositionR();
	m_startInfo.fLength = pCamera->GetLength();
	m_startInfo.rot = pCamera->GetRotation();
	
	// �ڕW�n�_�̒l�̐ݒ�
	m_targetInfo.rot = rot;
	m_targetInfo.fLength = fLength;

	// ���Ԃ̐ݒ�
	m_time.fEnd = fTime;
	m_time.fStop = fStopTime;
	m_time.fNow = 0.0f;
	m_time.fStopNow = 0.0f;

	// �t���O�̐ݒ�
	m_bFinish = false;
	m_bPause = false;
	m_bNext = bNext;
}