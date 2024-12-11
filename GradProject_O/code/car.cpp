//==========================================================
//
// �ԏ��� [car.cpp]
// Author : Riku Nakamura
//
//==========================================================
#include "car.h"
#include "car_manager.h"
#include "road.h"
#include "road_manager.h"
#include "manager.h"
#include "debugproc.h"
#include "collision.h"
#include "deltatime.h"
#include "slow.h"
#include "player.h"
#include "player_manager.h"

// �}�N����`

// �������O��Ԃ��`
namespace
{
	const float SPEED_STREET = (15.0f);			// ���i���̑��x
	const float SPEED_CURVE = (10.0f);			// �J�[�u���̑��x
	const float SPEED_STREET_BACK = (-10.0f);	// �o�b�N�Œ��i���̑��x
	const float SPEED_CURVE_BACK = (-10.0f);	// �o�b�N�ŃJ�[�u���̑��x
	const int TIME_BACK = (80);					// �o�b�N���鎞��
	const float SPEED_INER = (0.05f);			// ���x�̊���
	const float ROT_MULTI = (0.06f);			// �����␳�{��
	const float ROT_MULTI_BACK = (0.015f);		// �o�b�N���̌����␳�{��
	const float ROT_CURVE = (0.15f);			// �J�[�u����p�x
	const float LENGTH_POINT = (200.0f);		// ���B���苗��
	const float FRAME_RATE_SCALER = 60.0f;		// �t���[�����[�g���l���������x�̒���
	const float RECV_INER = (0.35f);			// ��M�����f�[�^�̊���
}

//==========================================================
// �R���X�g���N�^
//==========================================================
CCar::CCar(int nId)
{
	// �l�̃N���A
	m_Info = SInfo();
	m_Info.nId = nId;
	m_RecvInfo = SRecvInfo();
	m_pObj = nullptr;
	m_pTailLamp = nullptr;

	// ���X�g�ɓ����
	CCarManager::GetInstance()->ListIn(this);
}

//==========================================================
// �f�X�g���N�^
//==========================================================
CCar::~CCar()
{

}

//==========================================================
// ����������
//==========================================================
HRESULT CCar::Init(void)
{
	m_pObj = CObjectX::Create(VECTOR3_ZERO, VECTOR3_ZERO, "data\\MODEL\\car002.x");
	TailLamp();
	m_Info.fRotMulti = ROT_MULTI;
	return S_OK;
}

//==========================================================
// �I������
//==========================================================
void CCar::Uninit(void)
{
	// �`��I�u�W�F�N�g��p��
	SAFE_UNINIT(m_pObj);
	SAFE_DELETE(m_pTailLamp);
	CCarManager::GetInstance()->ListOut(this);

	Release();
}

//==========================================================
// �X�V����
//==========================================================
void CCar::Update(void)
{
	m_Info.posOld = m_Info.pos;
	m_Info.fSpeedDest = 0.0f;

	// ��M������ނ�ݒ�
	RecvTypeSet();

	// �ړ���̌���
	MoveRoad();

	// �ړ�����
	Move();

	// �����蔻�菈��
	Collision();

	if (m_Info.type == TYPE::TYPE_RECV)
	{
		RecvInerSet();
	}
	else if(m_Info.type != TYPE::TYPE_NONE)
	{
		CNetWork* pNet = CNetWork::GetInstance();

		// ���W���M
		if (pNet->GetTime()->IsOK())
		{
			SendPosition();
		}
	}

	CDebugProc::GetInstance()->Print("�Ԃ̍��W : [ %f, %f, %f ]\n", m_Info.pos.x, m_Info.pos.y, m_Info.pos.z);

	// ���W�n�ݒ�
	Set();
}

//==========================================================
// ����
//==========================================================
CCar *CCar::Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, const D3DXVECTOR3& move, int nId)
{
	CCar *pCar = nullptr;

	pCar = DEBUG_NEW CCar(nId);

	if (pCar != nullptr)
	{
		// ����������
		pCar->Init();

		// ���W�ݒ�
		pCar->SetPosition(pos);

		// �����ݒ�
		pCar->SetRotation(rot);

		// �ړ��ʐݒ�
		pCar->SetMove(move);
	}

	return pCar;
}

//==========================================================
// �e�[�������v����
//==========================================================
void CCar::TailLamp()
{
	m_pTailLamp = CEffekseer::GetInstance()->Create("data\\EFFEKSEER\\taillamp.efkefc", VECTOR3_ZERO, VECTOR3_ZERO, VECTOR3_ZERO, 45.0f, false, false);
}

//==========================================================
// �ړ�����
//==========================================================
void CCar::Move()
{
	if (!IsActive()) { return; }
	if (!m_Info.bBreak)
	{
		// �p�x����
		Rot();

		// �o�b�N���̏���
		if (m_Info.bBack)
		{
			m_Info.nBackTime--;

			if (m_Info.nBackTime < 0)
			{
				m_Info.bBack = false;
			}
		}

		// ���x��ǉ�
		m_Info.fSpeed += (m_Info.fSpeedDest - m_Info.fSpeed) * SPEED_INER;
		CManager::GetInstance()->GetDebugProc()->Print("��ʎԂ̑��x [ %f ]\n", m_Info.fSpeed);

		// �ړ��ʐݒ�
		m_Info.move.x = m_Info.fSpeed * sinf(m_Info.rot.y);
		m_Info.move.y = 0.0f;
		m_Info.move.z = m_Info.fSpeed * cosf(m_Info.rot.y);

		// �f���^�^�C���擾
		float DeltaTime = CDeltaTime::GetInstance()->GetDeltaTime();

		// �ʒu�ɉ��Z
		m_Info.pos += m_Info.move * FRAME_RATE_SCALER * DeltaTime;
	}
}

//==========================================================
// �p�x���菈��
//==========================================================
void CCar::Rot()
{
	float fRotMove, fRotDest, fRotDiff;				//�p�x�����p�ϐ�

	D3DXVECTOR3 vecTarget = m_Info.posTarget - m_Info.pos;

	// �v�Z�p�ϐ�
	fRotMove = m_Info.rot.y;
	fRotDest = atan2f(vecTarget.x, vecTarget.z);
	fRotDiff = fRotDest - fRotMove;

	// �p�x�␳
	if (fRotDiff > D3DX_PI)
	{
		fRotDiff -= D3DX_PI * 2.0f;
	}
	else if (fRotDiff < -D3DX_PI)
	{
		fRotDiff += D3DX_PI * 2.0f;
	}

	//�p�x��v����
	if (m_Info.bBack)
	{
		// �␳�̒l��ύX
		m_Info.fRotMulti = ROT_MULTI_BACK;

		//�����ǉ�
		if (m_Info.fSpeed < -5.0f)
		{
			fRotMove += fRotDiff * m_Info.fRotMulti;
		}

		// �󋵂ɂ���đ��x��ύX����
		if (fabsf(fRotDiff) >= ROT_CURVE)
		{
			m_Info.fSpeedDest += SPEED_CURVE_BACK;
		}
		else
		{
			m_Info.fSpeedDest += SPEED_STREET_BACK;
		}
	}
	else
	{
		//�����ǉ�
		fRotMove += fRotDiff * m_Info.fRotMulti;

		// �󋵂ɂ���đ��x��ύX����
		if (fabsf(fRotDiff) >= ROT_CURVE)
		{
			m_Info.fSpeedDest += SPEED_CURVE;
		}
		else
		{
			m_Info.fSpeedDest += SPEED_STREET;
		}
	}

	// �p�x�␳
	if (fRotMove > D3DX_PI)
	{
		fRotMove -= D3DX_PI * 2.0f;
	}
	else if (fRotMove <= -D3DX_PI)
	{
		fRotMove += D3DX_PI * 2.0f;
	}

	// �p�x�����Z
	m_Info.rot.y = fRotMove;

	// �p�x�␳
	if (m_Info.rot.y > D3DX_PI)
	{
		m_Info.rot.y -= D3DX_PI * 2.0f;
	}
	else if (m_Info.rot.y <= -D3DX_PI)
	{
		m_Info.rot.y += D3DX_PI * 2.0f;
	}
}

//==========================================================
// ���ړ�����
//==========================================================
void CCar::MoveRoad()
{
	if (!IsActive()) { return; }

	// �ړI�n�����݂��Ȃ���΍Ŋ��̓���ݒ�
	if (m_Info.pRoadTarget == nullptr)
		SearchRoad();

	// �Ŋ��̓���������Ȃ������ꍇ������
	if (m_Info.pRoadTarget == nullptr) { return; }

	// �ړI�n�ւ̓������菈��
	float length = D3DXVec3Length(&(m_Info.pRoadTarget->GetPosition() - m_Info.pos));
	if (length < LENGTH_POINT)
		ReachRoad();

	// �ڕW�n�_�̍��W���擾
	m_Info.posTarget = m_Info.pRoadTarget->GetPosition();
}

//==========================================================
// ���T������
//==========================================================
void CCar::SearchRoad()
{
	if (!IsActive()) { return; }

	// ���̃��X�g�擾
	CRoadManager* pRoadManager = CRoadManager::GetInstance();
	auto list = pRoadManager->GetList();

	CRoad* pRoad = pRoadManager->GetList()->Get(0);
	CRoad* pRoadClose = pRoadManager->GetList()->Get(0);

	// �������݂��Ȃ���Δ�����
	if (pRoad == nullptr) { return; }

	// �����v�Z�p�ϐ�
	float length = D3DXVec3Length(&(pRoadClose->GetPosition() - m_Info.pos));
	float lengthClose = 0.0f;

	// �Ŋ��̓���������
	for (int i = 0; i < pRoadManager->GetList()->GetNum() - 1; i++)
	{
		pRoad = list->Get(i);

		// �������菈��
		lengthClose = D3DXVec3Length(&(pRoad->GetPosition() - m_Info.pos));

		if (length > lengthClose)
		{
			length = lengthClose;
			pRoadClose = pRoad;
		}
	}
	
	// �ړI�n���Ŋ��̓��ɐݒ肷��
	m_Info.pRoadTarget = pRoadClose;
}

//==========================================================
// �����B������
//==========================================================
void CCar::ReachRoad()
{
	if (!IsActive()) { return; }
	CRoad* pRoadNext = nullptr;

	while (1)
	{// �n�_������܂�
		int roadPoint = rand() % CRoad::DIC_MAX;

		pRoadNext = m_Info.pRoadTarget->GetConnectRoad((CRoad::DIRECTION)roadPoint);

		// �����̓��͂��̂܂ܐi
		if (m_Info.pRoadTarget->GetType() == CRoad::TYPE_STOP)
		{
			
		}
		else
		{
			// U�^�[������ꍇ����T���Ȃ���
			if (pRoadNext == m_Info.pRoadStart) { continue; }
		}

		if (pRoadNext != nullptr) { break; }
	}

	// �ڕW�n�_�Əo���n�_�����炷
	m_Info.pRoadStart = m_Info.pRoadTarget;
	m_Info.pRoadTarget = pRoadNext;
}

//==========================================================
// �����蔻�菈��
//==========================================================
bool CCar::Collision()
{
	if (!IsActive()) { return false; }

	auto mgr = CObjectX::GetList();
	for(int i = 0; i < mgr->GetNum(); i++)
	{// �g�p����Ă��Ȃ���Ԃ܂�

		CObjectX* pObjectX = mgr->Get(i);	// �擪���擾

		if (!pObjectX->GetEnableCollision()) { continue; }

		// �I�u�W�F�N�g�̏��擾
		D3DXVECTOR3 posObjectX = pObjectX->GetPosition();
		D3DXVECTOR3 rotObjectX = pObjectX->GetRotation();
		D3DXVECTOR3 sizeMax = pObjectX->GetVtxMax();
		D3DXVECTOR3 sizeMin = pObjectX->GetVtxMin();

		// OBB�Ƃ̓����蔻������s
		bool bCollision = collision::CollidePointToOBB(&m_Info.pos, m_Info.posOld, posObjectX, rotObjectX, (sizeMax - sizeMin) * 0.5f);
		
		// �Փ˂��Ă��Ȃ��ꍇ�J��Ԃ�
		if (!bCollision) { continue; }

		if (pObjectX->GetType() == TYPE_PLAYER)
		{
			Break();
		}

		return true;
	}

	return false;
}

//==========================================================
// �ڐG������
//==========================================================
void CCar::Hit()
{
	if (!m_Info.bBack) { return; }

	CRoad* pRoadNext = m_Info.pRoadTarget;
	m_Info.pRoadTarget = m_Info.pRoadStart;
	m_Info.pRoadStart = pRoadNext;
	m_Info.bBack = true;
	m_Info.nBackTime = TIME_BACK;
}

//==========================================================
// �j��
//==========================================================
void CCar::Break()
{
	m_Info.bBreak = true;
}

//==========================================================
// �ݒ�
//==========================================================
void CCar::Set()
{
	if (m_pObj != nullptr)
	{
		m_Info.rot.y += D3DX_PI;
		m_pObj->SetPosition(m_Info.pos);
		m_pObj->SetRotation(m_Info.rot);
		m_Info.rot.y -= D3DX_PI;
	}

	if (m_pTailLamp != nullptr)
	{
		m_pTailLamp->m_pos = m_Info.pos;
		m_pTailLamp->m_rot = m_Info.rot;
	}
}

//==========================================================
// ���W�ݒ�
//==========================================================
void CCar::SetPosition(const D3DXVECTOR3& pos)
{ 
	m_Info.pos = pos; 

	if (m_pObj != nullptr)
	{
		m_pObj->SetPosition(m_Info.pos);
	}
}

//==========================================================
// �����ݒ�
//==========================================================
void CCar::SetRotation(const D3DXVECTOR3& rot)
{ 
	m_Info.rot = rot;

	if (m_pObj != nullptr)
	{
		m_pObj->SetRotation(m_Info.rot);
	}
}

//===============================================
// ��M�����f�[�^�Ɋ��������ĕ␳
//===============================================
void CCar::RecvInerSet()
{
	// �ʒu
	{
		D3DXVECTOR3 diff = m_RecvInfo.pos - m_Info.pos;
		D3DXVECTOR3 pos = m_Info.pos + diff * RECV_INER;
		m_Info.pos = pos;
	}

	// ����
	{
		D3DXVECTOR3 diff = m_RecvInfo.rot - m_Info.rot;
		Adjust(diff);

		D3DXVECTOR3 rot = m_Info.rot + diff;
		Adjust(rot);
		m_Info.rot = rot;
		Adjust(m_Info.rot);
	}
}

//===============================================
// �Ԃ̍��W�̍X�V
//===============================================
void CCar::SendPosition()
{
	CNetWork* pNet = CNetWork::GetInstance();

	pNet->SendCarPos(GetId(), GetPosition(), GetRotation());
}