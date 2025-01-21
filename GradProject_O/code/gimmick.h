//==========================================================
//
// �M�~�b�N [Gimmick.h]
// Author : �O�엳�V��
//
//==========================================================
#ifndef _Gimmick_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _Gimmick_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "task.h"
#include "list.h"
//#include "task.h"	// ����Ńt�@�C���C���N���[�h�ł��܂�

class CBridge;

//==========================================================
// �M�~�b�N�̃N���X��`
//==========================================================
class CGimmick : public CTask
{
public:

	// ��ޏ��
	enum TYPE
	{
		TYPE_FIREHYDRANT = 0,	// ���ΐ�
		TYPE_GUARDRAIL,			// �K�[�h���[��
		TYPE_POLICESTATION,		// �x�@��
		TYPE_BRIDGE,			// �͂ˋ�
		TYPE_SIGNBOARD,			// �Ŕ�
		TYPE_TEASHCAN,			// �S�~��
		TYPE_MAX
	};

	// ��{���
	struct SInfo
	{
		D3DXVECTOR3 pos;	// �ʒu
		D3DXVECTOR3 rot;	// ����
		D3DXVECTOR3 scale;	// �X�P�[��
		D3DXVECTOR3 CollisionMax;	// �ő�l
		D3DXVECTOR3 CollisionMin;	// �ŏ��l
		TYPE type;

		// �R���X�g���N�^
		SInfo() : pos(VECTOR3_ZERO), rot(VECTOR3_ZERO), scale(VECTOR3_ONE), CollisionMax(VECTOR3_ZERO), CollisionMin(VECTOR3_ZERO){}
	};

public:	// �N�ł��A�N�Z�X�\

	CGimmick();	// �R���X�g���N�^(�I�[�o�[���[�h)
	virtual ~CGimmick();	// �f�X�g���N�^

	// �����o�֐�
	virtual HRESULT Init(void) = 0;
	virtual void Uninit(void);
	virtual void Update(void)= 0;
	// �����o�֐�
	D3DXVECTOR3 GetPos(){ return m_Info.pos; }
	D3DXVECTOR3 GetRot() { return m_Info.rot; }
	D3DXVECTOR3 GetScale() { return m_Info.scale; }
	D3DXVECTOR3 GetVtxMax();
	D3DXVECTOR3 GetVtxMin();
	void SetPos(D3DXVECTOR3 pos) { m_Info.pos = pos; SetObjPos(pos); }
	void SetRot(D3DXVECTOR3 rot) { m_Info.rot = rot; SetObjRot(rot); }
	void SetScale(D3DXVECTOR3 scale) { m_Info.scale = scale; SetObjScale(scale); }
	static Clist<CGimmick*>* GetList() { return &m_List; }
	SInfo& GetInfo() { return m_Info; }
	static CGimmick* Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, const D3DXVECTOR3& scale, const TYPE type);
	TYPE GetType() { return m_Info.type; }
	void SetType(const TYPE type) { m_Info.type = type; }
	int GetId() { return m_nId; }
	virtual CBridge* GetBridge() { return nullptr; }

	// �Փˎ��̔���
	virtual void SetColMulti(const D3DXCOLOR& col) = 0;

	// �I�����C���p
	virtual void Hit(const D3DXVECTOR3& HitPos, const float fSpeed) {}

protected:

	void SetVtxMax(const D3DXVECTOR3& max) { m_Info.CollisionMax = max; }
	void SetVtxMin(const D3DXVECTOR3& min) { m_Info.CollisionMin = min; }
	virtual void SetObjPos(const D3DXVECTOR3& pos) = 0;
	virtual void SetObjRot(const D3DXVECTOR3& pos) = 0;
	virtual void SetObjScale(const D3DXVECTOR3& pos) = 0;

private:	// �����������A�N�Z�X�\

	// �����o�֐�
	static Clist<CGimmick*> m_List;

	// �����o�ϐ�
	SInfo m_Info;
	int m_nId;
};

#endif

