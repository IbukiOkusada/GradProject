//==========================================================
//
// �ԏ��� [car.h]
// Author : Riku Nakamura
//
//==========================================================
#ifndef _CAR_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _CAR_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "main.h"
#include "object.h"
#include "objectX.h"
#include "task.h"
#include "effekseerControl.h"
// �O���錾
class CWaist;
class CCharacter;
class CObjectX;
class CRoad;

//==========================================================
// �T���v���̃N���X��`
//==========================================================
class CCar : public CTask
{
public:

	// ��ޗ�
	enum TYPE
	{
		TYPE_RECV = 0,	// ��M�̂�
		TYPE_SEND,		// ���M�̂�
		TYPE_ACTIVE,	// ����
		TYPE_MAX
	};

private:	// �����������A�N�Z�X�\

	// ���\����
	struct SInfo
	{
		D3DXVECTOR3 pos;		// �ʒu
		D3DXVECTOR3 rot;		// ����
		D3DXVECTOR3 rotDest;	// �ڕW����
		D3DXVECTOR3 move;		// �ړ���
		D3DXVECTOR3 posOld;		// �ݒ�ʒu
		D3DXVECTOR3 posTarget;	// �ڕW�ʒu
		CRoad* pRoadStart;		// �ړ��J�n�n�_
		CRoad* pRoadTarget;		// �ڕW�n�_
		TYPE type;				// ���
		TYPE typeNext;			// ���̏��
		int nBackTime;
		float fSpeed;
		float fSpeedDest;
		float fRotMulti;
		bool bBreak;
		bool bBack;
		int nId;

		// �R���X�g���N�^
		SInfo() : pos(VECTOR3_ZERO), rot(VECTOR3_ZERO), rotDest(VECTOR3_ZERO),
			move(VECTOR3_ZERO), posOld(VECTOR3_ZERO), posTarget(VECTOR3_ZERO), pRoadStart(nullptr), pRoadTarget(nullptr),
			type(TYPE::TYPE_SEND), typeNext(TYPE::TYPE_MAX), nBackTime(0), fSpeed(0.0f), fSpeedDest(0.0f), fRotMulti(0.0f), bBreak(false), bBack(0.0f),
			nId(-1) {}
	};

	// ��M�p���\����
	struct SRecvInfo
	{
		D3DXVECTOR3 pos;		// �ʒu
		D3DXVECTOR3 rot;		// ����

		// �R���X�g���N�^
		SRecvInfo() : pos(VECTOR3_ZERO), rot(VECTOR3_ZERO) {}
	};

public:	// �N�ł��A�N�Z�X�\

	CCar(int nId);	// �R���X�g���N�^(�I�[�o�[���[�h)
	virtual ~CCar();	// �f�X�g���N�^

	// �����o�֐�
	virtual HRESULT Init(void);
	virtual void Uninit(void);
	virtual void Update(void);
	static CCar*Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, const D3DXVECTOR3& move, int nID);

	// �����o�֐�(�擾)
	D3DXVECTOR3& GetMove(void) { return m_Info.move; }
	D3DXVECTOR3& GetPosition(void) { return m_Info.pos; }
	D3DXVECTOR3& GetRotation(void) { return m_Info.rot; }
	D3DXVECTOR3& GetOldPosition(void) { return m_Info.posOld; }
	float GetSpeed(void) { return m_Info.fSpeed; }
	float GetSpeedDest(void) { return m_Info.fSpeedDest; }
	CRoad* GetRoadStart(void) { return m_Info.pRoadStart; }
	CRoad* GetRoadTarget(void) { return m_Info.pRoadTarget; }
	int GetModelIndex(void) { return m_pObj->GetIdx(); }
	bool GetBack(void) { return m_Info.bBack; }
	TYPE GetType() { return m_Info.type; }
	TYPE GetTypeNext() { return m_Info.typeNext; }
	virtual int GetId() { return m_Info.nId; }

	// �����œ����Ƃ���true �����Ȃ��Ƃ���false
	bool IsActive() {
		if (m_Info.type == TYPE::TYPE_RECV) return false;
		return true;
	}

	// �����o�֐�(�ݒ�)
	void SetMove(const D3DXVECTOR3& move) { m_Info.move = move; }
	void SetPosition(const D3DXVECTOR3& pos);
	void SetRotation(const D3DXVECTOR3& rot);
	void SetPosTarget(const D3DXVECTOR3& pos) { m_Info.posTarget = pos; }
	void SetSpeed(const float speed) { m_Info.fSpeed = speed; }
	void SetSpeedDest(const float speedDest) { m_Info.fSpeedDest = speedDest; }
	void SetRotMulti(const float rotMulti) { m_Info.fRotMulti = rotMulti; }
	void SetRoadStart(CRoad* RoadStart) { m_Info.pRoadStart = RoadStart; }
	void SetRoadTarget(CRoad* RoadTarget) { m_Info.pRoadTarget = RoadTarget; }
	void SetBack(bool bBack) { m_Info.bBack = bBack; }
	void SetBackTime(int nBackTime) { m_Info.nBackTime = nBackTime; }
	void SetType(TYPE type) { m_Info.type = type; }
	void SetTypeNext(TYPE type) { m_Info.typeNext = type; }
	void BinfId(int nId) { m_Info.nId = nId; }

	// ��M�p���
	void SetRecvPosition(const D3DXVECTOR3& pos) { m_RecvInfo.pos = pos; }
	void SetRecvRotation(const D3DXVECTOR3& rot) { m_RecvInfo.rot = rot; }

protected:	// �h���N���X������A�N�Z�X�\

	// �����o�֐�
	virtual void MoveRoad();
	virtual void SearchRoad();
	virtual void ReachRoad();
	virtual void Hit();
	void TailLamp();
	virtual void Break();
	void Set();
	virtual void SendPosition();
	virtual void RecvTypeSet() {
		if (m_Info.typeNext != TYPE::TYPE_MAX)
		{
			m_Info.type = m_Info.typeNext;
			m_Info.typeNext = TYPE::TYPE_MAX;
		}
	}

	// �����o�ϐ�
	CObjectX* m_pObj;

private:	// �����������A�N�Z�X�\

	// �����o�֐�
	virtual void Move();
	void Rot();
	bool Collision();

	// ��M�p�����o�֐�
	void RecvInerSet();

	// �����o�ϐ�
	SInfo m_Info;				// �������g�̏��
	CEffekseer::CEffectData* m_pTailLamp;

	// �I�����C���֘A
	SRecvInfo m_RecvInfo;		// ��M���
};

#endif
