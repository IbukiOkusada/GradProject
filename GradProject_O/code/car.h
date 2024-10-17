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
		CRoad* pRoadStart;		// �ړ��J�n�n�_
		CRoad* pRoadTarget;		// �ڕW�ʒu
		float speed;
		float speedDest;
	};

public:	// �N�ł��A�N�Z�X�\

	CCar();	// �R���X�g���N�^(�I�[�o�[���[�h)
	~CCar();	// �f�X�g���N�^

	// �����o�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	static CCar*Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move);

	// �����o�֐�(�擾)
	D3DXVECTOR3 GetMove(void) { return m_Info.move; }
	D3DXVECTOR3 GetPosition(void) { return m_Info.pos; }
	D3DXVECTOR3 GetRotation(void) { return m_Info.rot; }
	D3DXVECTOR3 GetOldPosition(void) { return m_Info.posOld; }
	CCar* GetNext(void) { return m_pNext; }
	CCar* GetPrev(void) { return m_pPrev; }

	// �����o�֐�(�ݒ�)
	void SetMove(const D3DXVECTOR3 move) { m_Info.move = move; }
	void SetPosition(const D3DXVECTOR3 pos) { m_Info.pos = pos; }
	void SetRotation(const D3DXVECTOR3 rot) { m_Info.rot = rot; }
	void SetNext(CCar* pNext) { m_pNext = pNext; }
	void SetPrev(CCar* pPrev) { m_pPrev = pPrev; }

private:	// �����������A�N�Z�X�\

	// �����o�֐�
	void Move();
	void Rot();
	void MoveRoad();
	void SearchRoad();
	void ReachRoad();

	// �����o�ϐ�
	CCar* m_pPrev;			// �O�̃I�u�W�F�N�g�ւ̃|�C���^
	CCar* m_pNext;			// ���̃I�u�W�F�N�g�ւ̃|�C���^
	SInfo m_Info;				// �������g�̏��
	CObjectX* m_pObj;
};

#endif
