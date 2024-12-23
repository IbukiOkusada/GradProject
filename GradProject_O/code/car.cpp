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
#include "bridge.h"
#include "meshfield.h"
#include "magic_enum/magic_enum.hpp"

// �}�N����`

// �������O��Ԃ��`
namespace
{
	const float SPEED_STREET = (15.0f);			// ���i���̑��x
	const float SPEED_CURVE_RIGHT = (9.0f);		// ���܎��̑��x
	const float SPEED_CURVE_LEFT = (3.0f);		// �E�܎��̑��x
	const float SPEED_CURVE_UTURN = (7.0f);		// U�^�[�����̑��x
	const float SPEED_STREET_BACK = (-10.0f);	// �o�b�N�Œ��i���̑��x
	const float SPEED_CURVE_BACK = (-10.0f);	// �o�b�N�ŃJ�[�u���̑��x
	const int TIME_BACK = (80);					// �o�b�N���鎞��
	const float SPEED_INER = (0.05f);			// ���x�̊���
	const float ROT_MULTI = (0.015f);			// �����␳�{��
	const float ROT_MULTI_BACK = (0.015f);		// �o�b�N���̌����␳�{��
	const float ROT_CURVE = (0.3f);				// �J�[�u����p�x
	const float LENGTH_POINT = (900.0f);		// ���B���苗��
	const float LENGTH_LANE = (-400.0f);		// �Ԑ��̕�
	const float FRAME_RATE_SCALER = 60.0f;		// �t���[�����[�g���l���������x�̒���
	const float RECV_INER = (0.35f);			// ��M�����f�[�^�̊���
	const float GRAVITY = (-24.0f);		//�v���C���[�d��

	const int SET_COL_MAX = (100);				// �ݒ肷��F�̍ő�l�i100 = 1.0�j
	const float SET_FLOAT_COL = (100.0f);		// �F��int�̒l��float�ɒ����p
}

//==========================================================
// �R���X�g���N�^
//==========================================================
CCar::CCar(int nId, CAR_TYPE type) : 
m_Info(SInfo()),
m_RecvInfo(SRecvInfo()),
m_pObj(nullptr),
m_pTailLamp(nullptr),
m_type(type)
{
	// �l�̃N���A
	m_Info.nId = nId;

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
	// �ԑ̂̐F�������_����
	float fColR = (float)(rand() % SET_COL_MAX) / SET_FLOAT_COL;
	float fColG = (float)(rand() % SET_COL_MAX) / SET_FLOAT_COL;
	float fColB = (float)(rand() % SET_COL_MAX) / SET_FLOAT_COL;

	m_pObj = CObjectX::Create(VECTOR3_ZERO, VECTOR3_ZERO, "data\\MODEL\\car002.x");
	m_pObj->SetColMulti(D3DXCOLOR(fColR, fColG, fColB, 1.0f));

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
	if (m_pObj == nullptr) { return; }
	if (m_Info.nId < 0) { 
		Uninit();
		return; 
	}

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

		Uninit();
		return;
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

	// ���W�n�ݒ�
	Set();
}

//==========================================================
// ����
//==========================================================
CCar *CCar::Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, const D3DXVECTOR3& move, int nId)
{
	CCar *pCar = nullptr;

	pCar = DEBUG_NEW CCar(nId, CAR_TYPE::CAR_TYPE_CAR);

	if (pCar != nullptr)
	{
		// ���W�ݒ�
		pCar->SetPosition(pos);
		pCar->SetRecvPosition(pos);

		// ����������
		pCar->Init();

		// �����ݒ�
		pCar->SetRotation(rot);
		pCar->SetRecvRotation(rot);

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
	if (m_pTailLamp == nullptr)
	{
		m_pTailLamp = CEffekseer::GetInstance()->Create("data\\EFFEKSEER\\taillamp.efkefc", m_Info.pos, VECTOR3_ZERO, VECTOR3_ZERO, 45.0f, false, false);
	}
}

//==========================================================
// �e�[�������v�폜
//==========================================================
void CCar::DeleteTailLamp()
{
	SAFE_DELETE(m_pTailLamp);
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
		//CManager::GetInstance()->GetDebugProc()->Print("��ʎԂ̑��x [ %f ]\n", m_Info.fSpeed);

		// �f���^�^�C���擾
		float DeltaTime = CDeltaTime::GetInstance()->GetDeltaTime();

		// �ړ��ʐݒ�
		m_Info.move.x = m_Info.fSpeed * sinf(m_Info.rot.y);
		m_Info.move.y += GRAVITY * DeltaTime;
		m_Info.move.z = m_Info.fSpeed * cosf(m_Info.rot.y);

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

	// �p�x�␳
	if (fRotDest > D3DX_PI)
	{
		fRotDest -= D3DX_PI * 2.0f;
	}
	else if (fRotDest < -D3DX_PI)
	{
		fRotDest += D3DX_PI * 2.0f;
	}

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

	// �f���^�^�C���擾
	float DeltaTime = CDeltaTime::GetInstance()->GetDeltaTime();

	//�p�x��v����
	if (m_Info.bBack)
	{
		//�����ǉ�
		if (m_Info.fSpeed < -5.0f)
		{
			fRotMove += fRotDiff * m_Info.fRotMulti * FRAME_RATE_SCALER * DeltaTime;
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
		fRotMove += fRotDiff * m_Info.fRotMulti * FRAME_RATE_SCALER * DeltaTime;

		// �󋵂ɂ���đ��x��ύX����
		if (fabsf(fRotDiff) >= ROT_CURVE)
		{
			m_Info.fSpeedDest += m_Info.fSpeedCurve;
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
	m_Info.posTarget = m_Info.pRoadTarget->GetPosition() + m_Info.offsetLane;
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

	CRoad* pRoad = list->Get(0);
	CRoad* pRoadClose = list->Get(0);

	// �������݂��Ȃ���Δ�����
	if (pRoad == nullptr) { return; }

	// �����v�Z�p�ϐ�
	float length = D3DXVec3Length(&(pRoadClose->GetPosition() - m_Info.pos));
	float lengthClose = 0.0f;

	// �Ŋ��̓���������
	for (int i = 0; i < list->GetNum() - 1; i++)
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
	Clist<int> listStation;

	// ���̐������o��
	for (int i = 0; i < CRoad::DIC_MAX; i++)
	{
		if (m_Info.pRoadTarget->GetConnectRoad((CRoad::DIRECTION)i) == nullptr) { continue; }

		if (m_Info.pRoadTarget->GetType() != CRoad::TYPE_STOP)
		{
			if (m_Info.pRoadTarget->GetConnectRoad((CRoad::DIRECTION)i) == m_Info.pRoadStart) { continue; }
		}

		listStation.Regist(i);
	}

	// �i�ޕ����������_���Ō���
	int roadPoint = listStation.Get(rand() % listStation.GetNum());
	pRoadNext = m_Info.pRoadTarget->GetConnectRoad((CRoad::DIRECTION)roadPoint);

	// �i�s�����ɂ���ĎԐ��̕����ړI�n�����炷
	if ((CRoad::DIRECTION)roadPoint == CRoad::DIC_UP)
	{// ��
		m_Info.offsetLane = D3DXVECTOR3(-LENGTH_LANE, 0.0f, 0.0f);
	}
	else if ((CRoad::DIRECTION)roadPoint == CRoad::DIC_DOWN)
	{// ��
		m_Info.offsetLane = D3DXVECTOR3(LENGTH_LANE, 0.0f, 0.0f);
	}
	else if ((CRoad::DIRECTION)roadPoint == CRoad::DIC_LEFT)
	{// ��
		m_Info.offsetLane = D3DXVECTOR3(0.0f, 0.0f, -LENGTH_LANE);
	}
	else if ((CRoad::DIRECTION)roadPoint == CRoad::DIC_RIGHT)
	{// �E
		m_Info.offsetLane = D3DXVECTOR3(0.0f, 0.0f, LENGTH_LANE);
	}

	// �ړI�n�Əo���n�_�����݂��鎞
	if (m_Info.pRoadStart != nullptr && m_Info.pRoadTarget != nullptr)
	{
		D3DXVECTOR3 vecTarget, vecNext, vecTemp;
		vecTarget = m_Info.pRoadTarget->GetPosition() - m_Info.pRoadStart->GetPosition();
		vecNext = pRoadNext->GetPosition() - m_Info.pRoadStart->GetPosition();

		// �Ȃ�������ɑ΂��đ��x��ݒ�
		if (m_Info.pRoadTarget->GetType() == CRoad::TYPE_STOP)
		{// U�^�[��
			m_Info.fSpeedCurve = SPEED_CURVE_UTURN;
		}
		else if (D3DXVec3Cross(&vecTemp, &vecTarget, &vecNext)->y > 0.0f)
		{// ����
			m_Info.fSpeedCurve = SPEED_CURVE_LEFT;
		}
		else if (D3DXVec3Cross(&vecTemp, &vecTarget, &vecNext)->y < 0.0f)
		{// �E��
			m_Info.fSpeedCurve = SPEED_CURVE_RIGHT;
		}
		else
		{// ���i
			m_Info.fSpeedCurve = SPEED_CURVE_LEFT;
		}
	}
	else
	{
		m_Info.fSpeedCurve = SPEED_CURVE_LEFT;
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

	bool bCollision = false;

	// �I�u�W�F�N�g�Ƃ̓����蔻��
	if (CollisionObjX())
		bCollision = true;

	// ���Ƃ̓����蔻��
	if (CollisionRoad())
		bCollision = true;

	// �n�ʂƂ̓����蔻��
	if (CollisionField())
		bCollision = true;

	// �M�~�b�N�Ƃ̓����蔻��
	if (CollisionGimick())
		bCollision = true;

	return bCollision;
}

//==========================================================
// �I�u�W�F�N�g�Ƃ̓����蔻�菈��
//==========================================================
bool CCar::CollisionObjX(void)
{
	auto mgr = CObjectX::GetList();
	for (int i = 0; i < mgr->GetNum(); i++)
	{// �g�p����Ă��Ȃ���Ԃ܂�

		CObjectX* pObjectX = mgr->Get(i);	// �擾

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
// ���Ƃ̓����蔻�菈��
//==========================================================
bool CCar::CollisionRoad(void)
{
	// ���Ƃ̔���
	auto listRoad = CRoadManager::GetInstance()->GetList();
	for (int i = 0; i < listRoad->GetNum(); i++)
	{// �g�p����Ă��Ȃ���Ԃ܂�

		// ���m�F
		CRoad* pRoad = listRoad->Get(i);	// �擪���擾
		if (pRoad == nullptr) { continue; }

		D3DXVECTOR3* pVtx = pRoad->GetVtxPos();
		D3DXVECTOR3 pos = pRoad->GetPosition();

		float height = m_Info.pos.y - 0.1f;
		D3DXVECTOR3 vec1 = pVtx[1] - pVtx[0], vec2 = pVtx[2] - pVtx[0];
		D3DXVECTOR3 nor0, nor1;

		D3DXVec3Cross(&nor0, &vec1, &vec2);
		D3DXVec3Normalize(&nor0, &nor0);	// �x�N�g���𐳋K������

		vec1 = pVtx[2] - pVtx[3];
		vec2 = pVtx[1] - pVtx[3];
		D3DXVec3Cross(&nor1, &vec1, &vec2);
		D3DXVec3Normalize(&nor1, &nor1);	// �x�N�g���𐳋K������

		// ����
		collision::IsOnSquarePolygon(pos + pVtx[0], pos + pVtx[1], pos + pVtx[2], pos + pVtx[3],
			nor0, nor1, m_Info.pos, m_Info.posOld, height);

		if (height >= m_Info.pos.y)
		{
			m_Info.pos.y = height;
			m_Info.move.y = 0.0f;
		}
	}

	return false;
}

//==========================================================
// �M�~�b�N�Ƃ̓����蔻�菈��
//==========================================================
bool CCar::CollisionGimick(void)
{
	auto listGimmick = CGimmick::GetList();
	for (int i = 0; i < listGimmick->GetNum(); i++)
	{// �g�p����Ă��Ȃ���Ԃ܂�

		// ���m�F
		CGimmick* pGimmick = listGimmick->Get(i);	// �擪���擾
		if (pGimmick == nullptr) { continue; }

		if (pGimmick->GetType() != CGimmick::TYPE_BRIDGE) { continue; }

		CBridge* pBridge = dynamic_cast <CBridge*> (pGimmick);

		for (int bridge = 0; bridge < BRIDGE_NUM; bridge++)
		{
			// �|���S���̑傫�����v�Z
			CObjectX* pObjectX = pBridge->GetObjectX(bridge);
			D3DXVECTOR3 posGimmick = pObjectX->GetPosition();
			D3DXVECTOR3 rotGimmick = pObjectX->GetRotation();
			D3DXVECTOR3 sizeMax = pObjectX->GetVtxMax();
			D3DXVECTOR3 sizeMin = pObjectX->GetVtxMin();
			sizeMin.y = 0.0f;

			sizeMax = collision::PosRelativeMtx(VECTOR3_ZERO, rotGimmick, sizeMax);
			sizeMin = collision::PosRelativeMtx(VECTOR3_ZERO, rotGimmick, sizeMin);

			float height = m_Info.pos.y - 0.1f;
			D3DXVECTOR3 pVtx[4];
			pVtx[0] = D3DXVECTOR3(sizeMax.x, sizeMax.y, sizeMax.z);
			pVtx[1] = D3DXVECTOR3(sizeMin.x, sizeMin.y, sizeMax.z);
			pVtx[2] = D3DXVECTOR3(sizeMax.x, sizeMax.y, sizeMin.z);
			pVtx[3] = D3DXVECTOR3(sizeMin.x, sizeMin.y, sizeMin.z);

			// �@���x�N�g�����v�Z����
			D3DXVECTOR3 vec1, vec2;
			D3DXVECTOR3 nor0, nor1;

			vec1 = pVtx[1] - pVtx[0];
			vec2 = pVtx[2] - pVtx[0];
			D3DXVec3Cross(&nor0, &vec1, &vec2);
			D3DXVec3Normalize(&nor0, &nor0);	// �x�N�g���𐳋K������

			vec1 = pVtx[2] - pVtx[3];
			vec2 = pVtx[1] - pVtx[3];
			D3DXVec3Cross(&nor1, &vec1, &vec2);
			D3DXVec3Normalize(&nor1, &nor1);	// �x�N�g���𐳋K������

			// ����
			collision::IsOnSquarePolygon(posGimmick + pVtx[0], posGimmick + pVtx[1], posGimmick + pVtx[2], posGimmick + pVtx[3],
				nor0, nor1, m_Info.pos, m_Info.posOld, height);

			if (height >= m_Info.pos.y)
			{
				m_Info.pos.y = height;
				m_Info.move.y = 0.0f;
			}
		}
	}

	return false;
}

//==========================================================
// �n�ʂƂ̓����蔻�菈��
//==========================================================
bool CCar::CollisionField(void)
{
	// ���Ƃ̔���
	auto listRoad = CMeshField::GetList();
	for (int i = 0; i < listRoad->GetNum(); i++)
	{// �g�p����Ă��Ȃ���Ԃ܂�

		// ���m�F
		CMeshField* pRoad = listRoad->Get(i);	// �擪���擾
		if (pRoad == nullptr) { continue; }

		D3DXVECTOR3 pVtx[4] = {};
		D3DXVECTOR3 pos = pRoad->GetPosition();

		pVtx[0] = D3DXVECTOR3(-(pRoad->GetWidth() * pRoad->GetNumWidth()), 0.0f, (pRoad->GetHeight() * pRoad->GetNumHeight()));
		pVtx[1] = D3DXVECTOR3((pRoad->GetWidth() * pRoad->GetNumWidth()), 0.0f, (pRoad->GetHeight() * pRoad->GetNumHeight()));
		pVtx[2] = D3DXVECTOR3(-(pRoad->GetWidth() * pRoad->GetNumWidth()), 0.0f, -(pRoad->GetHeight() * pRoad->GetNumHeight()));
		pVtx[3] = D3DXVECTOR3((pRoad->GetWidth() * pRoad->GetNumWidth()), 0.0f, -(pRoad->GetHeight() * pRoad->GetNumHeight()));

		float height = m_Info.pos.y - 0.1f;
		D3DXVECTOR3 vec1 = pVtx[1] - pVtx[0], vec2 = pVtx[2] - pVtx[0];
		D3DXVECTOR3 nor0, nor1;

		D3DXVec3Cross(&nor0, &vec1, &vec2);
		D3DXVec3Normalize(&nor0, &nor0);	// �x�N�g���𐳋K������

		vec1 = pVtx[2] - pVtx[3];
		vec2 = pVtx[1] - pVtx[3];
		D3DXVec3Cross(&nor1, &vec1, &vec2);
		D3DXVec3Normalize(&nor1, &nor1);	// �x�N�g���𐳋K������

		// ����
		collision::IsOnSquarePolygon(pos + pVtx[0], pos + pVtx[1], pos + pVtx[2], pos + pVtx[3],
			nor0, nor1, m_Info.pos, m_Info.posOld, height);

		if (height >= m_Info.pos.y)
		{
			m_Info.pos.y = height;
			m_Info.move.y = 0.0f;
		}
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

	CDebugProc::GetInstance()->Print(" ��M���Ă��܂� %d ���W : [%f, %f, %f]\n", m_Info.nId,
		m_RecvInfo.pos.x, m_RecvInfo.pos.y, m_RecvInfo.pos.z);
}

//===============================================
// �Ԃ̍��W�̍X�V
//===============================================
void CCar::SendPosition()
{
	CNetWork* pNet = CNetWork::GetInstance();

	pNet->SendCarPos(GetId(), GetPosition(), GetRotation());
}