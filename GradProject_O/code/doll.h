//==========================================================
//
// �l�`�̏��� [doll.h]
// Author : Riku Nakamura
//
//==========================================================
#ifndef _DOLL_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _DOLL_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "task.h"
#include "objectX.h"
#include "player.h"
#include <list>
#include "task.h"
#include "effekseerControl.h"

// �O���錾
class CCharacter;

//==========================================================
// ���{�b�g�̃N���X��`
//==========================================================
class CDoll : public CTask
{
public:	// �N�ł��A�N�Z�X�\

	// ��ԗ񋓌^
	enum STATE
	{
		STATE_NONE = 0, // �����Ȃ�
		STATE_NIGHTOFFIRE, 	// �������
		STATE_ENOCH,	// ������
		STATE_DROPS_FIRST,	// ������
		STATE_DROPS_SECOND,	// ������
		STATE_DROPS_POSE,	// ������
		STATE_MAX
	};

	// ���\����
	struct SInfo
	{
		D3DXVECTOR3 pos;		// �ʒu
		D3DXVECTOR3 rot;		// ����
		D3DXVECTOR3 move;		// �ړ���
		STATE state;			// ���
		int nId;
		int nCntLoop;			// ���[�v��

		// �R���X�g���N�^
		SInfo() : pos(VECTOR3_ZERO), rot(VECTOR3_ZERO),
			move(VECTOR3_ZERO), nId(-1) {}
	};

private:

	

public:	// �N�ł��A�N�Z�X�\
	
	// ���[�V�����񋓌^
	enum MOTION
	{
		MOTION_NIGHTOFFIRE = 0,	// night of fire���[�V����
		MOTION_ENOCH,			// enoch���[�V����
		MOTION_DROPS_FIRST,		// �����~�D���h���b�v�X�O�����[�V����
		MOTION_DROPS_SECOND,	// �����~�D���h���b�v�X�㔼���[�V����
		MOTION_DROPS_POSE1,		// �����~�D���h���b�v�X�|�[�Y���[�V����
		MOTION_DROPS_POSE2,		// �����~�D���h���b�v�X�|�[�Y���[�V����
		MOTION_DROPS_POSE3,		// �����~�D���h���b�v�X�|�[�Y���[�V����
		MOTION_DROPS_POSE4,		// �����~�D���h���b�v�X�|�[�Y���[�V����
		MOTION_DROPS_POSE5,		// �����~�D���h���b�v�X�|�[�Y���[�V����
		MOTION_DROPS_POSE6,		// �����~�D���h���b�v�X�|�[�Y���[�V����
		MOTION_DROPS_POSE7,		// �����~�D���h���b�v�X�|�[�Y���[�V����
		MOTION_MAX
	};

	CDoll();	// �R���X�g���N�^(�I�[�o�[���[�h)
	~CDoll();	// �f�X�g���N�^

	// �����o�֐�
	HRESULT Init();
	HRESULT Init(const D3DXVECTOR3& rot);
	void Uninit(void);
	void Update(void);
	static CDoll* Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, int nID);

	// �����o�֐�(�擾)
	D3DXVECTOR3 GetPosition(void) { return m_Info.pos; }
	D3DXVECTOR3 GetRotation(void) { return m_Info.rot; }
	CDoll::SInfo GetInfo(void) { return m_Info; }

	// �����o�֐�(�ݒ�)
	void SetMove(const D3DXVECTOR3& move) { m_Info.move = move; }
	void SetPosition(const D3DXVECTOR3& pos);
	void SetRotation(const D3DXVECTOR3& rot);
	void SetState(STATE state) { m_Info.state = state; }
	void SetID(int nID) { m_Info.nId = nID; }

protected:	// �h���N���X������A�N�Z�X�\

	// �����o�֐�
	void Set();

	// �����o�ϐ�
	CCharacter* m_pCharacter;	// �L�����N�^�[

private:	// �����������A�N�Z�X�\

	// �����o�֐�
	void Dance(void);

	// �����o�ϐ�
	SInfo m_Info;				// �������g�̏��
};

#endif

