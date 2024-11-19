//===================================================
//
//�p�[�e�B�N������
//Author ����ɐ�
//
//===================================================
#ifndef _PARTICLE_H_
#define _PARTICLE_H_

#include "main.h"
#include "effect3D.h"

#define MAX_PARTICLE	(256)	// �p�[�e�B�N���̍ő吔

// �N���X�̒�`
class CParticle3D
{
public:	// �N�ł��A�N�Z�X�\

	CParticle3D();	// �R���X�g���N�^
	~CParticle3D();	// �f�X�g���N�^

	// �����o�֐�
	static void Create(const D3DXVECTOR3& pos, CEffect3D::TYPE type);
	static void Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& move, CEffect3D::TYPE type);

private:	// �����������A�N�Z�X�\

	// �����o�֐�
	static void Set(const D3DXVECTOR3& Defpos, const D3DXVECTOR3& Defmove, CEffect3D::TYPE type);
};

#endif
