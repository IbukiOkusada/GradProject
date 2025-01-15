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
		TYPE_NONE = 0,		// �^�C�v����
		TYPE_NORMAL,		// �ʏ�
		TYPE_ELITE,			// ��荞�݌^
		TYPE_GENTLE,		// �ɂ₩�^
		TYPE_MAX
	};

	// AI�^�C�v�񋓌^
	enum STATE
	{
		STATE_NORMAL = 0,		// �ʏ���
		STATE_PREP,				// �U���������
		STATE_ATTACK,			// �U�����
		STATE_FINISH,			// �U���I�����
		STATE_MAX
	};

	CPoliceAI();	// �R���X�g���N�^
	~CPoliceAI();	// �f�X�g���N�^

	// �����o�֐�
	virtual HRESULT Init(void);
	void Uninit(void);
	void Search(void);
	void Chase(void);
	void CallBackup(void);
	void Attack(void);
	void StopAttack(void);

	void BeginChase(CPlayer* pPlayer);
	void EndChase(void);

	static CPoliceAI* Create(CPolice* pPolice, TYPE type = TYPE_NORMAL);

	virtual void ChaseAStar(void);

	// �����o�֐�(�擾)
	CRoad::SInfoSearch* GetSearchRoad() { return m_pSearchTarget; }
	CPolice* GetPolice() { return m_pPolice; }
	STATE GetState() { return m_state; }
	float GetChaseSpeed() { return m_fChaseSpeed; }
	bool GetCall() { return m_bCall; }

	// �����o�֐�(�ݒ�)
	void SetCall(bool bCall) { m_bCall = bCall; }

protected:
	
	// �����o�֐�
	virtual void SelectRoad(void);
	virtual void ReachRoad(void);

	// �����o�ϐ�
	CPolice* m_pPolice;
	CRoad* m_pRoadStart;		// �ړ��J�n�n�_
	CRoad* m_pRoadTarget;		// �ڕW�n�_
	CRoad::SInfoSearch* m_pSearchTarget;
	vector<CRoad::SInfoSearch*> m_searchRoad;
	int m_nCntThread;
	int m_nCntCall;
	float m_fChaseSpeed;
	float m_fSearchInterval;
	bool m_bCross;
	bool m_bCall;

private:	// �����������A�N�Z�X�\

	// �����o�֐�
	void CheckSpeed(CPlayer* pPlayer);
	void CheckTurn(CPlayer* pPlayer);
	void CheckDamage(CPlayer* pPlayer);
	void CheckSmoke(CPlayer* pPlayer);
	void CheckCollision(CPlayer* pPlayer);
	void CheckLevel(CPlayer* pPlayer);

	// �����o�ϐ�
	float m_fSearchTimer;
	float m_fLevelSearch;
	int nAttackTime;
	TYPE m_type;
	STATE m_state;
	CPolice* m_pPoliceBackUp;
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
	void ReachRoad(void) override;
	void ChaseAStar(void) override;

	// �����o�ϐ�
	CRoad* m_pRoadRelay;		// �ڕW�n�_
	bool m_bRelay;				// �I�񂵂����ǂ���
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
