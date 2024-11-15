//<=================================================
//�^�C�g���ł̃v���C���\�̓������� [PlayerTitle.cpp]
//
//Author : Kazuki Watanabe
//<=================================================
#include "PlayerTitle.h"
#include "camera.h"
#include "camera_manager.h"
#include "debugproc.h"

//<************************************************
//���O�錾
//<************************************************
namespace
{
	const D3DXVECTOR3 DEST_POS = D3DXVECTOR3(-4734.0f, 0.0f, 1054.0f);	//�ړI�n�̈ʒu
	const float DEST_DIFF = 5.0f;										//�����̍�
}

//<================================================
//�R���X�g���N�^
//<================================================
CPlayerTitle::CPlayerTitle()
{
	m_eState = STATE_NONE;
	m_bReached = false;
}
//<================================================
//�f�X�g���N�^
//<================================================
CPlayerTitle::~CPlayerTitle()
{

}
//<================================================
//����������
//<================================================
HRESULT CPlayerTitle::Init(void)
{
	CPlayer::Init();

	return S_OK;
}
//<===============================================
//����������(�I�[�o�[���[�h)
//<===============================================
HRESULT CPlayerTitle::Init(const char* pBodyName, const char* pLegName)
{
	CPlayer::Init(pBodyName, pLegName);

	return S_OK;
}
//<================================================
//�I������
//<================================================
void CPlayerTitle::Uninit(void)
{
	CPlayer::Uninit();
}
//<================================================
//�X�V����
//<================================================
void CPlayerTitle::Update(void)
{
	//�f�o�b�O�ȊO��������
	if (m_eState == STATE_NONE)
	{
		DEBUGKEY();
		// �O��̍��W���擾
		m_Info.posOld = GetPosition();

		StateSet();
		pRadio->Update();

		// �}�g���b�N�X
		SetMatrix();

		// �����蔻��
		Collision();

		if (m_pObj != nullptr)
		{
			D3DXVECTOR3 rot = GetRotation();
			D3DXVECTOR3 pos = GetPosition();
			SetPosition(pos);
			SetRotation(rot);
			m_pObj->SetPosition(pos);
			m_pObj->SetRotation(rot);
			m_pObj->SetShadowHeight(GetPosition().y);

			Moving();

			// �G�t�F�N�g
			{
				m_pTailLamp->m_pos = pos;
				m_pTailLamp->m_rot = rot;
				m_pBackdust->m_pos = GetPosition();
				m_pBackdust->m_rot = m_pObj->GetRotation();
				m_pBackdust->m_Scale = VECTOR3_ONE * m_fEngine * 300.0f;

				m_pAfterburner->m_pos = GetPosition();;

				m_pAfterburner->m_Scale = VECTOR3_ONE * m_fEngine * m_fBrake * 150.0f;

			}
			if (m_pDamageEffect != nullptr)
			{
				m_pDamageEffect->m_pos = pos;
				m_pDamageEffect->m_rot = rot;
			}
		}

		// �ו�������
		if (m_pBaggage != nullptr)
		{
			D3DXVECTOR3 rot = GetRotation();
			rot.y -= D3DX_PI * 0.5f;
			CCamera* pCamera = CCameraManager::GetInstance()->GetTop();
		}
	}
	//�f�o�b�O��������
	else if (m_eState == STATE_DEBUG)
	{
		//�s�����[�h
		SetType(CPlayer::TYPE::TYPE_ACTIVE);
		CPlayer::Update();
	}

	//// �f�o�b�O�\��
	//CDebugProc::GetInstance()->Print("�v���C���[ :");
	//CDebugProc::GetInstance()->Print("���W: [ %f, %f, %f ]", m_Info.pos.x, m_Info.pos.y, m_Info.pos.z);
	//CDebugProc::GetInstance()->Print(" : ����: [ %f, %f, %f ]\n", m_pObj->GetRotation().x, m_pObj->GetRotation().y, m_pObj->GetRotation().z);
}
//<================================================
//�����Ɋւ��鏈��
//<================================================
void CPlayerTitle::Moving(void)
{
	//�ړI�n�܂ňړ�
	m_Info.pos.x += (DEST_POS.x - m_Info.pos.x - m_Info.move.x) * 0.075f;//X��
	m_Info.pos.z += (DEST_POS.z - m_Info.pos.z - m_Info.move.z) * 0.075f;//Z��

	if (Function::MoveToDest(m_Info.pos, DEST_POS, DEST_DIFF)) { m_bReached = true; }

}
//<================================================
//��������
//<================================================
CPlayerTitle* CPlayerTitle::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, const D3DXVECTOR3 move
, const char* pBodyName, const char* pLegName)
{
	//�����̂��߂̃I�u�W�F�N�g
	CPlayerTitle* pPlayertitle = DEBUG_NEW CPlayerTitle;

	//���g����������
	if (pPlayertitle)
	{
		// ����������
		pPlayertitle->Init(pBodyName, pLegName);

		// ���W�ݒ�
		pPlayertitle->SetPosition(pos);

		// �����ݒ�
		pPlayertitle->SetRotation(rot);

		pPlayertitle->m_fRotDest = rot.y;

		// �ړ��ʐݒ�
		pPlayertitle->SetMove(move);

		//�����Ȃ���Ԃɂ���
		pPlayertitle->SetType(CPlayer::TYPE::TYPE_NONE);
	}
	//���������ꍇ�A���g�Ȃ���Ԃ�
	else
	{
		return nullptr;
	}

	//���̏���Ԃ�
	return pPlayertitle;
}