//==========================================================
//
// �p�g�J�[���� [police.h]
// Author : Riku Nakamura
//
//==========================================================
#ifndef _POLICE_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _POLICE_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "main.h"
#include "object.h"
#include "objectX.h"
#include "task.h"
#include "car.h"

// �O���錾

//==========================================================
// �T���v���̃N���X��`
//==========================================================
class CPolice : public CCar
{
private:	// �����������A�N�Z�X�\

	// ���\����
	struct SInfo
	{
		
	};

public:	// �N�ł��A�N�Z�X�\

	CPolice();	// �R���X�g���N�^(�I�[�o�[���[�h)
	~CPolice();	// �f�X�g���N�^

	// �����o�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	static CPolice*Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move);

	// �����o�֐�(�擾)

	// �����o�֐�(�ݒ�)

private:	// �����������A�N�Z�X�\

	// �����o�֐�
	void Move();
	void Rot();
	void MoveRoad();
	void SearchRoad();
	void ReachRoad();
	void Collision();

	// �����o�ϐ�
	SInfo m_Info;				// �������g�̏��
};

#endif
