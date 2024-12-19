//==========================================================
//
// �ו��̏��� [baggage.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "baggage.h"
#include "model.h"
#include "deltatime.h"
#include "camera.h"
#include "camera_manager.h"
#include "camera_action.h"
#include "player.h"
#include "manager.h"
#include "renderer.h"

namespace
{
	const float ROTATE = (D3DX_PI * 2.0f) * 5.0f;	// ��]��
	const D3DXVECTOR3 THROW_SCALE = D3DXVECTOR3(3.0f, 3.0f, 3.0f);
	const float HEIGHT = 1200.0f;
}

// �ÓI�����o�ϐ�
Clist<CBaggage*> CBaggage::m_List = {};
Clist<CBaggage*> CBaggage::m_ThrowList = {};

//==========================================================
// �R���X�g���N�^
//==========================================================
CBaggage::CBaggage()
{
	// �l�̃N���A
	m_time = STime();
	m_pos = VECTOR3_ZERO;
	m_rot = VECTOR3_ZERO;
	m_pObj = nullptr;
	m_bFinish = false;
	m_bFirst = false;
	m_List.Regist(this);
	m_state = STATE_NONE;
}

//==========================================================
// �f�X�g���N�^
//==========================================================
CBaggage::~CBaggage()
{

}

//==========================================================
// ����
//==========================================================
CBaggage* CBaggage::Create(const D3DXVECTOR3& pos)
{
	CBaggage* pBaggage = DEBUG_NEW CBaggage;

	if (pBaggage != nullptr)
	{
		pBaggage->Init();
		pBaggage->m_pos = pos;
	}

	return pBaggage;
}

//==========================================================
// �l�ݒ�
//==========================================================
void CBaggage::Set(const D3DXVECTOR3& pos, D3DXVECTOR3* pTarget, float fTime)
{
	// �l�ݒ�
	m_pos = pos;
	m_pTarget = pTarget;
	m_time.fEnd = fTime;
	m_rot.y = atan2f(m_pTarget->x - pos.x, m_pTarget->z - pos.z);
	m_state = STATE_THROW;

	// �������X�g�ɓ����
	m_ThrowList.Regist(this);

	if (m_pObj == nullptr) { return; }

	m_pObj->SetPosition(pos);
}

//==========================================================
// ����������
//==========================================================
HRESULT CBaggage::Init(void)
{
	m_pObj = CModel::Create("data\\MODEL\\ice\\softcream.x");
	m_pObj->SetPosOrigin(VECTOR3_ZERO);
	m_pObj->SetCurrentPosition(VECTOR3_ZERO);
	m_pObj->SetRotation(VECTOR3_ZERO);
	m_pObj->SetCurrentRotation(VECTOR3_ZERO);
	m_pObj->SetScale(D3DXVECTOR3(5.0f, 5.0f, 5.0f));
	return S_OK;
}

//==========================================================
// �I������
//==========================================================
void CBaggage::Uninit(void)
{
	// �p��
	if (m_pObj != nullptr)
	{
		m_pObj->Uninit();
		m_pObj = nullptr;
	}

	if (m_List.Find(this) != nullptr)
	{
		m_List.Delete(this);
	}
	
	if (m_state == STATE::STATE_THROW)
	{
		if (m_ThrowList.Find(this) != nullptr)
		{
			m_ThrowList.Delete(this);
		}
	}

	Release();
}

//==========================================================
// �X�V����
//==========================================================
void CBaggage::Update(void)
{
	if (m_pObj == nullptr) { return; }

	// �����Ă��Ȃ�
	if (m_state != STATE_THROW) { 
		m_pObj->SetPosition(m_pos); 
		return; 
	}

	// ����������
	Throw();
}

//==========================================================
// ��������
//==========================================================
void CBaggage::Throw()
{
	if (m_pObj == nullptr) { return; }
	if (m_bFinish) { return; }

	// �i�߂�(�^�C�����B���Ă��Ȃ�)
	if (m_time.fNow < m_time.fEnd) {
		m_time.fNow += CDeltaTime::GetInstance()->GetDeltaTime();
	}

	// ��{���擾
	D3DXVECTOR3 pos = m_pos;
	D3DXVECTOR3 rot = m_rot;

	// �^�C�����i��ł���
	if (m_time.fNow > 0)
	{
		// ������␳
		float multi = m_time.fNow / m_time.fEnd;

		if (multi > 1.0f) { multi = 1.0f; }

		rot.x += ROTATE * multi;

		// �ʒu
		if (m_pTarget != nullptr)
		{
			D3DXVECTOR3 posDiff = *m_pTarget - pos;
			pos += posDiff * multi;

			// y���W�͌ʂ�`��
			float ymulti = multi;

			if (ymulti > 0.5f)
			{
				ymulti = 0.5f - (ymulti - 0.5f);
			}

			pos.y += sinf(ymulti) * HEIGHT;
		}

		if (multi >= 0.4f)
		{
			if(!m_bFirst){ CManager::GetInstance()->GetRenderer()->SetEnableDrawMultiScreen(0.0f, 1.0f, 120.0f); }
			m_bFirst = true;
			CDeltaTime::GetInstance()->SetSlow(0.2f);
		}
	}

	// �J�����ݒ�
	SetCamera();

	// �ݒ�
	m_pObj->SetPosition(pos);
	m_pObj->SetRotation(rot);
	m_pObj->SetShadowHeight(m_pTarget->y);

	// ���ԏI��
	if (m_time.fNow >= m_time.fEnd) {
		
		if (m_ThrowList.Find(this) != nullptr)
		{
			m_ThrowList.Delete(this);
		}

		m_pos = VECTOR3_ZERO;
		CDeltaTime::GetInstance()->SetSlow(1.0f);
		CCamera* pCamera = CCameraManager::GetInstance()->GetTop();
		pCamera->GetAction()->SetFinish(true);
		m_bFinish = true;
		m_state = STATE_NONE;
		//Uninit();
	}
}

//==========================================================
// �J�����ݒ菈��
//==========================================================
void CBaggage::SetCamera()
{
	if (m_pTarget == nullptr) { return; }

	CCamera* pCamera = CCameraManager::GetInstance()->GetTop();
	float rotY = atan2f(m_pos.z - m_pTarget->z, m_pos.x - m_pTarget->x);
	pCamera->GetAction()->Set(pCamera, pCamera->GetPositionR(), D3DXVECTOR3(0.0f, rotY, D3DX_PI * 0.35f), 600.0f, 0.5f, 1.0f, CCameraAction::MOVE_POSV, true);
	D3DXVECTOR3 pos = *m_pTarget;
	pos.y += 200.0f;
	pCamera->Pursue(pos, VECTOR3_ZERO, 0.0f);
}

//==========================================================
// �X�P�[���ݒ�
//==========================================================
void CBaggage::SetThrowScale(const D3DXVECTOR3& scale)
{
	D3DXVECTOR3 setscale = scale;
	setscale.x *= THROW_SCALE.x;
	setscale.y *= THROW_SCALE.y;
	setscale.z *= THROW_SCALE.z;
	m_pObj->SetScale(setscale);
}