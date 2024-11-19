//===================================================
//
//�p�[�e�B�N������
//Author ����ɐ�
//
//===================================================
#ifndef _PARTICLE_H_
#define _PARTICLE_H_

#include "main.h"
#include "effect2D.h"

#define MAX_PARTICLE	(256)	// �p�[�e�B�N���̍ő吔

// �N���X�̒�`
class CParticle2D
{
public:	// �N�ł��A�N�Z�X�\

	CParticle2D();	// �R���X�g���N�^
	~CParticle2D();	// �f�X�g���N�^

	// �����o�֐�
	static void Create(const D3DXVECTOR3& pos, CEffect2D::TYPE type);
	static void Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& move, CEffect2D::TYPE type);

private:	// �����������A�N�Z�X�\

	// �����o�֐�
	static void Set(const D3DXVECTOR3& Defpos, const D3DXVECTOR3& Defmove, CEffect2D::TYPE type);
};

#endif

