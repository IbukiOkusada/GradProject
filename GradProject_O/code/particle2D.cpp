//===============================================
//
// �p�[�e�B�N���̏��� [particle.cpp]
// Author : Ibuki Okusada
//
//===============================================
#include "particle2D.h"
#include "manager.h"
#include "renderer.h"
#include "input.h"
#include "debugproc.h"
#include "texture.h"
#include "billboard.h"

//===============================================
// �}�N����`
//===============================================
#define LIFE	(30)		// ����

//===============================================
// �R���X�g���N�^
//===============================================
CParticle2D::CParticle2D()
{

}

//===============================================
// �f�X�g���N�^
//===============================================
CParticle2D::~CParticle2D()
{

}

//===============================================
// ����
//===============================================
void CParticle2D::Create(const D3DXVECTOR3& pos, CEffect2D::TYPE type)
{
	// �I�u�W�F�N�g�̎�ނ̐ݒ�
	Set(pos, D3DXVECTOR3(0.0f, 0.0f, 0.0f), type);
}

//===============================================
// ���_���ݒ�
//===============================================
void CParticle2D::Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& move, CEffect2D::TYPE type)
{
	// �I�u�W�F�N�g�̎�ނ̐ݒ�
	Set(pos, move, type);
}

//===============================================
// �ݒ�
//===============================================
void CParticle2D::Set(const D3DXVECTOR3& Defpos, const D3DXVECTOR3& Defmove, CEffect2D::TYPE type)
{
	D3DXVECTOR3 pos = {};
	D3DXVECTOR3 move = {};	// �ړ���
	D3DXCOLOR col = {};	// �F
	float fRadius = 0.0f;
	float fLife = 0;
	D3DXVECTOR3 nor;

	// �ړ��x�N�g�������߂�
	D3DXVec3Normalize(&nor, &Defmove);	// �x�N�g���𐳋K������

	switch (type)
	{
	case CEffect2D::TYPE_NONE:	// �����Ȃ�

		break;

	case CEffect2D::TYPE_SMAKE:	// ��

		for (int nCnt = 0; nCnt < 2; nCnt++)
		{
			// ���W�̐ݒ�
			pos = Defpos;

			//�ړ��ʂ̐ݒ�
			move.x = sinf((float)(rand() % 629 - 314) * 0.01f) * ((float)(rand() % 100)) * 0.09f;
			move.y = ((float)(rand() % 50)) * 0.15f;
			move.z = cosf((float)(rand() % 629 - 314) * 0.01f) * ((float)(rand() % 100)) * 0.09f;

			//�F�̐ݒ�
			col = D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.0f);

			//���a�̐ݒ�
			fRadius = 50.0f;

			//�����̐ݒ�
			fLife = 100.0f;

			CEffect2D::Create(Defpos + move, move, col, fRadius, fLife, type);
		}

		break;

	case CEffect2D::TYPE_TARGET:	// ��


		// ���W�̐ݒ�
		pos = Defpos;

		//�ړ��ʂ̐ݒ�
		move = VECTOR3_ZERO;

		//�F�̐ݒ�
		col = D3DXCOLOR(0.1f, 0.1f, 0.5f, 0.7f);

		//���a�̐ݒ�
		fRadius = 100.0f;

		//�����̐ݒ�
		fLife = 10.0f;

		CEffect2D::Create(Defpos + move, move, col, fRadius, fLife, type);

		break;
	}
}