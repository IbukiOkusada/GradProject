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

public:	// �N�ł��A�N�Z�X�\

	CGimmick();	// �R���X�g���N�^(�I�[�o�[���[�h)
	~CGimmick();	// �f�X�g���N�^

	// �����o�֐�
	virtual HRESULT Init(void) = 0;
	virtual void Uninit(void)= 0;
	virtual void Update(void)= 0;
	// �����o�֐�
	D3DXVECTOR3 GetPos(){ return m_pos; }
	D3DXVECTOR3 GetRot() { return m_rot; }
	D3DXVECTOR3 GetScale() { return m_scale; }
	void SetPos(D3DXVECTOR3 pos) { m_pos = pos; }
	void SetRot(D3DXVECTOR3 rot) { m_rot = rot; }
	void SetScale(D3DXVECTOR3 scale) { m_scale = scale; }

private:	// �����������A�N�Z�X�\

	// �����o�֐�

	// �����o�ϐ�
	D3DXVECTOR3 m_pos;
	D3DXVECTOR3 m_rot;
	D3DXVECTOR3 m_scale;

};

#endif

