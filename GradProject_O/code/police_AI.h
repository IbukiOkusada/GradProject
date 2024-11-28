//==========================================================
//
// �p�g�J�[AI���� [police_AI.h]
// Author : Riku Nakamura
//
//==========================================================
#ifndef _POLICE_AI_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _POLICE_AI_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "main.h"
#include "road.h"
#include "road_manager.h"
#include <list>

// �O���錾
class CRoad;
class CPolice;
class CPlayer;

//==========================================================
// �x�@AI�̃N���X��`
//==========================================================
class CPoliceAI
{
public:	// �N�ł��A�N�Z�X�\

	CPoliceAI();	// �R���X�g���N�^
	virtual ~CPoliceAI();	// �f�X�g���N�^

	// �����o�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Search(void);
	void Chase(void);

	static CPoliceAI* Create(CPolice* pPolice);

	// �����o�֐�(�擾)
	CRoad::SSearch* GetSearchRoad() { return m_pSearchTarget; }

	// �����o�֐�(�ݒ�)

protected:
	
	// �����o�֐�

	// �����o�ϐ�

private:	// �����������A�N�Z�X�\

	// �����o�֐�
	void CheckSpeed(CPlayer* pPlayer);
	void CheckTurn(CPlayer* pPlayer);
	void CheckDamage(CPlayer* pPlayer);
	void CheckSmoke(CPlayer* pPlayer);
	void CheckCollision(CPlayer* pPlayer);
	void SelectRoad(void);
	void ReachRoad(void);

	// �����o�ϐ�
	CPolice* m_pPolice;
	CRoad* m_pRoadStart;		// �ړ��J�n�n�_
	CRoad* m_pRoadTarget;		// �ڕW�n�_
	CRoad::SSearch* m_pSearchTarget;
	float m_fSearchTimer;
	vector<CRoad::SSearch*> m_searchRoad;

};

#endif
