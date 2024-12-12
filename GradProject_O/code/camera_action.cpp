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
	m_bFinish = true;
	m_bPause = false;
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
	if (m_bPause) { return; }

	// �i�߂�(�^�C�����B���Ă��Ȃ�)
	if (m_time.fNow < m_time.fEnd) {
		m_time.fNow += CDeltaTime::GetInstance()->GetDeltaTime();
	}

	// ��{���擾
	D3DXVECTOR3 rot = m_startInfo.rot;
	float length = m_startInfo.fLength;

	// �^�C�����i��ł���
	if (m_time.fNow > 0)
	{
		// ������␳
		float multi = m_time.fNow / m_time.fEnd;
		if (multi > 1.0f) { multi = 1.0f; }

		// ����
		D3DXVECTOR3 rotDiff = m_targetInfo.rot - rot;
		Adjust(rotDiff);
		rot += rotDiff * multi;
		Adjust(rot);

		// ����
		float lenDiff = m_targetInfo.fLength - length;
		length += lenDiff * multi;
	}

	// ��ނ��Ƃɐݒ�
	pCamera->SetRotation(rot);
	pCamera->SetLength(length);

	// ��ނƕʂ̍��W�ړ�
	if (m_time.fNow > 0)
	{
		// ������␳
		float multi = m_time.fNow / m_time.fEnd;
		if (multi > 1.0f) { multi = 1.0f; }

		switch (m_move)
		{
		case MOVE_POSV:
		{
			// ���W
			D3DXVECTOR3 pos = m_startInfo.posR;
			D3DXVECTOR3 posDiff = m_targetInfo.pos - pos;
			pos += posDiff * multi;
			pCamera->SetPositionR(pos);
		}
			break;

		case MOVE_POSR:
		{
			// ���W
			D3DXVECTOR3 pos = m_startInfo.posV;
			D3DXVECTOR3 posDiff = m_targetInfo.pos - pos;
			pos += posDiff * multi;
			pCamera->SetPositionV(pos);
		}
			break;

		default:
			break;
		}
	}

	// ��ނ��ƂɕύX
	switch (m_move)
	{
	case MOVE_POSV:
		pCamera->SetV();
		break;

	case MOVE_POSR:
		pCamera->SetR();
		break;

	default:
		break;
	}

	// ���ԏI��
	if (m_time.fNow >= m_time.fEnd) {

		// ��~���ԉ��Z
		m_time.fStopNow += CDeltaTime::GetInstance()->GetDeltaTime();

		// ���ԏI��
		if (m_time.fStopNow >= m_time.fStop) {

			if (m_bNext) m_bPause = true;
			else m_bFinish = true;

		}
	}
}

//==========================================================
// �A�N�V�����ݒ�
//==========================================================
void CCameraAction::Set(CCamera* pCamera, const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, const float& fLength,
	const float fTime, const float fStopTime, const MOVE& move, const bool& bNext)
{
	if (pCamera == nullptr) { return; }

	// �����l�_�̐ݒ�
	m_startInfo.posV = pCamera->GetPositionV();
	m_startInfo.posR = pCamera->GetPositionR();
	m_startInfo.fLength = pCamera->GetLength();
	m_startInfo.rot = pCamera->GetRotation();
	Adjust(m_startInfo.rot);
	
	// �ڕW�n�_�̒l�̐ݒ�
	m_targetInfo.pos = pos;
	m_targetInfo.rot = rot;
	m_targetInfo.fLength = fLength;
	Adjust(m_targetInfo.rot);

	// ���Ԃ̐ݒ�
	m_time.fEnd = fTime;
	m_time.fStop = fStopTime;
	m_time.fNow = 0.0f;
	m_time.fStopNow = 0.0f;

	// �t���O�̐ݒ�
	m_bFinish = false;
	m_bPause = false;
	m_bNext = bNext;
	m_move = move;
}

//==========================================================
// �����␳
//==========================================================
void CCameraAction::Adjust(D3DXVECTOR3& rot)
{
	while (1)
	{
		if (rot.x > D3DX_PI || rot.x < -D3DX_PI)
		{//-3.14�`3.14�͈̔͊O�̏ꍇ
			if (rot.x > D3DX_PI)
			{
				rot.x += (-D3DX_PI * 2);
			}
			else if (rot.x < -D3DX_PI)
			{
				rot.x += (D3DX_PI * 2);
			}
		}
		else
		{
			break;
		}
	}

	while (1)
	{
		if (rot.y > D3DX_PI || rot.y < -D3DX_PI)
		{//-3.14�`3.14�͈̔͊O�̏ꍇ
			if (rot.y > D3DX_PI)
			{
				rot.y += (-D3DX_PI * 2);
			}
			else if (rot.y < -D3DX_PI)
			{
				rot.y += (D3DX_PI * 2);
			}
		}
		else
		{
			break;
		}
	}

	while (1)
	{
		if (rot.z > D3DX_PI || rot.z < -D3DX_PI)
		{//-3.14�`3.14�͈̔͊O�̏ꍇ
			if (rot.z > D3DX_PI)
			{
				rot.z += (-D3DX_PI * 2);
			}
			else if (rot.z < -D3DX_PI)
			{
				rot.z += (D3DX_PI * 2);
			}
		}
		else
		{
			break;
		}
	}
}