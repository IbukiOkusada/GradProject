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

	// AI�^�C�v�񋓌^
	enum TYPE
	{
		TYPE_NORMAL = 0,	// �ʏ�
		TYPE_ELITE,			// ��荞�݌^
		TYPE_GENTLE,		// �ɂ₩�^
		TYPE_MAX
	};

	CPoliceAI();	// �R���X�g���N�^
	~CPoliceAI();	// �f�X�g���N�^

	// �����o�֐�
	virtual HRESULT Init(void);
	void Uninit(void);
	void Search(void);
	void Chase(void);

	void BeginChase(CPlayer* pPlayer);
	void EndChase(void);

	static CPoliceAI* Create(CPolice* pPolice, TYPE type = TYPE_NORMAL);

	// �����o�֐�(�擾)
	CRoad::SInfoSearch* GetSearchRoad() { return m_pSearchTarget; }

	// �����o�֐�(�ݒ�)

protected:
	
	// �����o�֐�
	virtual void SelectRoad(void);
	virtual void ReachRoad(void);
	virtual void ChaseAStar(void);

	// �����o�ϐ�
	CPolice* m_pPolice;
	CRoad* m_pRoadStart;		// �ړ��J�n�n�_
	CRoad* m_pRoadTarget;		// �ڕW�n�_
	CRoad::SInfoSearch* m_pSearchTarget;
	vector<CRoad::SInfoSearch*> m_searchRoad;
	int m_nCntThread;

private:	// �����������A�N�Z�X�\

	// �����o�֐�
	void CheckSpeed(CPlayer* pPlayer);
	void CheckTurn(CPlayer* pPlayer);
	void CheckDamage(CPlayer* pPlayer);
	void CheckSmoke(CPlayer* pPlayer);
	void CheckCollision(CPlayer* pPlayer);

	// �����o�ϐ�
	float m_fSearchTimer;
	TYPE m_type;
};

//==========================================================
// �x�@AI�̃N���X��`
//==========================================================
class CPoliceAINomal : public CPoliceAI
{
	// �����o�֐�
	HRESULT Init(void) override;
	void SelectRoad(void) override;
};

//==========================================================
// �x�@AI�̃N���X��`
//==========================================================
class CPoliceAIElite : public CPoliceAI
{
	// �����o�֐�
	HRESULT Init(void) override;
	void SelectRoad(void) override;
	void ChaseAStar(void) override;

	// �����o�ϐ�
	CRoad* m_pRoadRelay;		// �ڕW�n�_
};

//==========================================================
// �x�@AI�̃N���X��`
//==========================================================
class CPoliceAIGentle : public CPoliceAI
{
	// �����o�֐�
	HRESULT Init(void) override;
	void SelectRoad(void) override;
};

#endif
