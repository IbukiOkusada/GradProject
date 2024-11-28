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
		TYPE_MAX
	};

	CPoliceAI();	// �R���X�g���N�^
	~CPoliceAI();	// �f�X�g���N�^

	// �����o�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Search(void);
	void Chase(void);

	static CPoliceAI* Create(CPolice* pPolice, TYPE type = TYPE_NORMAL);

	// �����o�֐�(�擾)
	CRoad::SSearch* GetSearchRoad() { return m_pSearchTarget; }

	// �����o�֐�(�ݒ�)

protected:
	
	// �����o�֐�
	virtual void SelectRoad(void);
	virtual void ReachRoad(void);

	// �����o�ϐ�
	CPolice* m_pPolice;
	CRoad* m_pRoadStart;		// �ړ��J�n�n�_
	CRoad* m_pRoadTarget;		// �ڕW�n�_

private:	// �����������A�N�Z�X�\

	// �����o�֐�
	void CheckSpeed(CPlayer* pPlayer);
	void CheckTurn(CPlayer* pPlayer);
	void CheckDamage(CPlayer* pPlayer);
	void CheckSmoke(CPlayer* pPlayer);
	void CheckCollision(CPlayer* pPlayer);

	// �����o�ϐ�
	CRoad::SSearch* m_pSearchTarget;
	float m_fSearchTimer;
	vector<CRoad::SSearch*> m_searchRoad;
	TYPE m_type;
};

class CPoliceAINomal : public CPoliceAI
{
	void SelectRoad(void) override;
};

class CPoliceAIElite : public CPoliceAI
{
	void SelectRoad(void) override;
};

#endif
