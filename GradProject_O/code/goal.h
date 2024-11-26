//==========================================================
//
// �S�[�� [goal.h]
// Author : �O�엳�V��
//
//==========================================================
#ifndef _GOAL_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _GOAL_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "task.h"
#include "list.h"
#include "effekseerControl.h"
#include "road.h"

//#include "task.h"	// ����Ńt�@�C���C���N���[�h�ł��܂�
//�O���錾
class CMeshCylinder;
class CCharacter;
class CBaggage;

//==========================================================
// �S�[���̃N���X��`
//==========================================================
class CGoal : public CTask
{

public:	// �N�ł��A�N�Z�X�\

	CGoal();	// �R���X�g���N�^(�I�[�o�[���[�h)
	~CGoal();	// �f�X�g���N�^

	// �����o�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	static CGoal* Create(D3DXVECTOR3 pos,float fRange,float fLimit);

	CRoad* GetRoad() { return m_pRoad; }
	D3DXVECTOR3 GetPos() { return m_pos; }
	void SetEnd(int nId);
	static Clist<CGoal*>* GetInstance() { if (pList == nullptr) { pList = pList->Create(); }return pList; }		// ���X�g�擾
	static void ListRelease() { if (pList != nullptr) { delete pList; pList = nullptr; } }					// ���X�g���
	
	bool GetEnd() { return m_bEnd; }
private:	// �����������A�N�Z�X�\

	// �����o�֐�
	bool CheckRange(int nId);
	bool CheckSpeed(int nId);
	void ScreenEffect();

	// �����o�ϐ�
	int m_nId;
	D3DXVECTOR3 m_pos;				// ���W
	float m_fRange;					// �S�[���͈̔�
	float m_fLimit;					// ���x����
	bool m_bEnd;					// �I���n�_
	CCharacter* m_pPeople;			// �l
	CRoad * m_pRoad;
	CBaggage* m_pBaggage;
	CEffekseer::CEffectData* pEffect;
	static Clist<CGoal*>* pList;	// �����̃��X�g*GetInstance()�o�R�ŃA�N�Z�X���鎖*
};

#endif