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
		float speed;
		float speedDest;
		bool bBreak;
	};

public:	// �N�ł��A�N�Z�X�\

	CCar();	// �R���X�g���N�^(�I�[�o�[���[�h)
	virtual ~CCar();	// �f�X�g���N�^

	// �����o�֐�
	virtual HRESULT Init(void);
	virtual void Uninit(void);
	virtual void Update(void);
	static CCar*Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move);
	void TailLamp();
	// �����o�֐�(�擾)
	D3DXVECTOR3 GetMove(void) { return m_Info.move; }
	D3DXVECTOR3 GetPosition(void) { return m_Info.pos; }
	D3DXVECTOR3 GetRotation(void) { return m_Info.rot; }
	D3DXVECTOR3 GetOldPosition(void) { return m_Info.posOld; }
	float GetSpeed(void) { return m_Info.speed; }
	float GetSpeedDest(void) { return m_Info.speedDest; }
	CRoad* GetRoadStart(void) { return m_Info.pRoadStart; }
	CRoad* GetRoadTarget(void) { return m_Info.pRoadTarget; }
	CCar* GetNext(void) { return m_pNext; }
	CCar* GetPrev(void) { return m_pPrev; }
	int GetModelIndex(void) { return m_pObj->GetIdx(); }

	// �����o�֐�(�ݒ�)
	void SetMove(const D3DXVECTOR3 move) { m_Info.move = move; }
	void SetPosition(const D3DXVECTOR3 pos) { m_Info.pos = pos; }
	void SetRotation(const D3DXVECTOR3 rot) { m_Info.rot = rot; }
	void SetPosTarget(const D3DXVECTOR3 pos) { m_Info.posTarget = pos; }
	void SetSpeed(const float speed) { m_Info.speed = speed; }
	void SetSpeedDest(const float speedDest) { m_Info.speedDest = speedDest; }
	void SetRoadStart(CRoad* RoadStart) { m_Info.pRoadStart = RoadStart; }
	void SetRoadTarget(CRoad* RoadTarget) { m_Info.pRoadTarget = RoadTarget; }
	void SetNext(CCar* pNext) { m_pNext = pNext; }
	void SetPrev(CCar* pPrev) { m_pPrev = pPrev; }

protected:	// �h���N���X������A�N�Z�X�\

	// �����o�֐�
	virtual void MoveRoad();
	void SearchRoad();
	virtual void ReachRoad();

	// �����o�ϐ�
	CObjectX* m_pObj;

private:	// �����������A�N�Z�X�\

	// �����o�֐�
	void Move();
	void Rot();
	bool Collision();

	// �����o�ϐ�
	CCar* m_pPrev;			// �O�̃I�u�W�F�N�g�ւ̃|�C���^
	CCar* m_pNext;			// ���̃I�u�W�F�N�g�ւ̃|�C���^
	SInfo m_Info;				// �������g�̏��
	CEffekseer::CEffectData* m_pTailLamp;
};

#endif
