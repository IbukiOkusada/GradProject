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
class CObjectX;

//==========================================================
// �S�[���̃N���X��`
//==========================================================
class CGoal : public CTask
{
public:

	// ��{���\����
	struct SInfo
	{
		D3DXVECTOR3 pos;				// ���W
		float fRange;					// �S�[���͈̔�
		float fLimit;					// ���x����

		// �R���X�g���N�^
		SInfo() : pos(VECTOR3_ZERO), fRange(0.0f), fLimit(0.0f){}
	};

private:

	// �l�Ǘ��p�\����
	struct SPeople
	{
		CCharacter* pChara;		// �l
		D3DXVECTOR3 setpos;		// ���΍��W

		// �R���X�g���N�^
		SPeople() : pChara(nullptr), setpos(VECTOR3_ZERO) {}
	};

public:	// �N�ł��A�N�Z�X�\

	CGoal();	// �R���X�g���N�^(�I�[�o�[���[�h)
	~CGoal();	// �f�X�g���N�^

	// �����o�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	static CGoal* Create(D3DXVECTOR3 pos,float fRange,float fLimit);

	void SetEnd(int nId);
	static Clist<CGoal*>* GetInstance() { if (pList == nullptr) { pList = pList->Create(); }return pList; }		// ���X�g�擾
	static void ListRelease() { if (pList != nullptr) { delete pList; pList = nullptr; } }					// ���X�g���
	
	// �ݒ�
	void SetPos(const D3DXVECTOR3& pos);

	// �擾
	SInfo* GetInfo() { return &m_Info; }
	CRoad* GetRoad() { return m_pRoad; }
	D3DXVECTOR3 GetPos() { return m_Info.pos; }
	bool GetEnd() { return m_bEnd; }
	CEffekseer::CEffectData* GetEffect() { return pEffect; }
	float GetRange() { return m_Info.fRange; }

private:	// �����������A�N�Z�X�\

	// �����o�֐�
	bool CheckRange(int nId);
	bool CheckSpeed(int nId);
	void ScreenEffect();

	// �����o�ϐ�
	int m_nId;
	bool m_bEnd;					// �I���n�_
	SInfo m_Info;
	CRoad * m_pRoad;
	CBaggage* m_pBaggage;
	CEffekseer::CEffectData* pEffect;
	SPeople m_People;				// �l�̏��
	static Clist<CGoal*>* pList;	// �����̃��X�g*GetInstance()�o�R�ŃA�N�Z�X���鎖*
};

#endif