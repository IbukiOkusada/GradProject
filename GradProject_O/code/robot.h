//==========================================================
//
// ���{�b�g�̏��� [robot.h]
// Author : Kenta Hashimoto
//
//==========================================================
#ifndef _ROBOT_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _ROBOT_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "task.h"
#include "objectX.h"
#include "player.h"
#include <list>
#include "task.h"

// �O���錾
class CCharacter;

//==========================================================
// ���{�b�g�̃N���X��`
//==========================================================
class CRobot : public CTask
{

public:	// �N�ł��A�N�Z�X�\
		// ��ԗ񋓌^
	enum STATE
	{
		STATE_WALK = 0,	// �������
		STATE_AVOID,	// ������
		STATE_MAX
	};

	// ���[�V�����񋓌^
	enum MOTION
	{
		MOTION_WALK = 0,	// �������[�V����
		MOTION_AVOID,		// ������[�V����
		MOTION_MAX
	};

	CRobot();	// �R���X�g���N�^(�I�[�o�[���[�h)
	~CRobot();	// �f�X�g���N�^

	// �����o�֐�
	HRESULT Init();
	HRESULT Init(const D3DXVECTOR3& rot);
	void Uninit(void);
	void Update(void);
	static CRobot* Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, const float& Distance);

	// �����o�֐�(�擾)
	CRobot* GetNext(void) { return m_pNext; }
	CRobot* GetPrev(void) { return m_pPrev; }

	// �����o�֐�(�ݒ�)
	void SetMove(const D3DXVECTOR3& move) { m_Info.move = move; }
	void SetPosition(const D3DXVECTOR3& pos);
	void SetRotation(const D3DXVECTOR3& rot);

	void SetNext(CRobot* pNext) { m_pNext = pNext; }
	void SetPrev(CRobot* pPrev) { m_pPrev = pPrev; }

protected:	// �h���N���X������A�N�Z�X�\

	// �����o�֐�
	void Set();

	// �����o�ϐ�
	CCharacter* m_pCharacter;	// �L�����N�^�[

private:	// �����������A�N�Z�X�\

	// �����o�֐�
	void SetPosTerget(const float& Distance);
	bool TergetReach();

	void Walk();
	void Collision(D3DXVECTOR3 pos);

	// ���\����
	struct SInfo
	{
		D3DXVECTOR3 pos;		// �ʒu
		D3DXVECTOR3 rot;		// ����
		D3DXVECTOR3 rotDest;	// �ڕW����
		D3DXVECTOR3 move;		// �ړ���
		D3DXVECTOR3 posOld;		// �ݒ�ʒu
		STATE state;			// ���
		CPlayer* pPlayer;
		int nTargetID;
		int nId;

		// �R���X�g���N�^
		SInfo() : pos(VECTOR3_ZERO), rot(VECTOR3_ZERO), rotDest(VECTOR3_ZERO),
			move(VECTOR3_ZERO), posOld(VECTOR3_ZERO),
			nTargetID(0), nId(-1) {}
	};

	// �����o�ϐ�
	SInfo m_Info;				// �������g�̏��
	CRobot* m_pPrev;			// �O�̃I�u�W�F�N�g�ւ̃|�C���^
	CRobot* m_pNext;			// ���̃I�u�W�F�N�g�ւ̃|�C���^

	D3DXVECTOR3 m_PosTarget[2];

};

#endif

