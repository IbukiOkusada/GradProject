//==========================================================
//
// �M�~�b�N [Gimmick.h]
// Author : �O�엳�V��
//
//==========================================================
#ifndef _Gimmick_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _Gimmick_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "task.h"
//#include "task.h"	// ����Ńt�@�C���C���N���[�h�ł��܂�

//==========================================================
// �M�~�b�N�̃N���X��`
//==========================================================
class CGimmick : public CTask
{
private:

	// ��{���
	struct SInfo
	{
		D3DXVECTOR3 pos;	// �ʒu
		D3DXVECTOR3 rot;	// ����
		D3DXVECTOR3 scale;	// �X�P�[��

		// �R���X�g���N�^
		SInfo() : pos(VECTOR3_ZERO), rot(VECTOR3_ZERO), scale(VECTOR3_ONE){}
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
	void SetPos(D3DXVECTOR3 pos) { m_Info.pos = pos; }
	void SetRot(D3DXVECTOR3 rot) { m_Info.rot = rot; }
	void SetScale(D3DXVECTOR3 scale) { m_Info.scale = scale; }

private:	// �����������A�N�Z�X�\

	// �����o�֐�

	// �����o�ϐ�
	SInfo m_Info;
};

#endif

