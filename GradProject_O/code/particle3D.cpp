//===============================================
//
// �p�[�e�B�N���̏��� [particle.cpp]
// Author : Ibuki Okusada
//
//===============================================
#include "particle3D.h"
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
CParticle3D::CParticle3D()
{

}

//===============================================
// �f�X�g���N�^
//===============================================
CParticle3D::~CParticle3D()
{

}

//===============================================
// ����
//===============================================
void CParticle3D::Create(const D3DXVECTOR3& pos, CEffect3D::TYPE type)
{
	// �I�u�W�F�N�g�̎�ނ̐ݒ�
	Set(pos, D3DXVECTOR3(0.0f, 0.0f, 0.0f), type);
}

//===============================================
// ���_���ݒ�
//===============================================
void CParticle3D::Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& move, CEffect3D::TYPE type)
{
	// �I�u�W�F�N�g�̎�ނ̐ݒ�
	Set(pos, move, type);
}

//===============================================
// �ݒ�
//===============================================
void CParticle3D::Set(const D3DXVECTOR3& Defpos, const D3DXVECTOR3& Defmove, CEffect3D::TYPE type)
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
	case CEffect3D::TYPE_NONE:	// �����Ȃ�

		break;

	case CEffect3D::TYPE_SMAKE:	// ��

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

			CEffect3D::Create(Defpos + move, move, col, fRadius, fLife, type);
		}

		break;

	case CEffect3D::TYPE_SPLASH:	// �G�t�F�N�g

		for (int nCnt = 0; nCnt < 1; nCnt++)
		{
			// ���W�̐ݒ�
			pos = Defpos;

			float angle = static_cast<float>(rand() % 629 - 314) * 0.01f;
			float speed = static_cast<float>(rand() % 100) * 0.11f;

			//�ړ��ʂ̐ݒ�
			move.x = sinf(angle) * speed;
			move.y = static_cast<float>(rand() % 20) + 25.0f;
			move.z = cosf(angle) * speed;

			float colrand = static_cast<float>(rand() % 40) * 0.01f;

			//�F�̐ݒ�
			col = D3DXCOLOR(colrand, colrand, 0.7f, 1.0f);

			//���a�̐ݒ�
			fRadius = 30.0f;

			//�����̐ݒ�
			fLife = 90.0f;

			CEffect3D::Create(Defpos + move, move, col, fRadius, fLife, type);
		}

		break;

	case CEffect3D::TYPE_LASER:	// �G�t�F�N�g

		for (int nCnt = 0; nCnt < 1; nCnt++)
		{
			// ���W�̐ݒ�
			pos = Defpos;

			float speed = 5.0f;

			//�ړ��ʂ̐ݒ�
			move.x = nor.x * speed;
			move.y = 0.0f;
			move.z = nor.z * speed;

			float colrand = 0.4f;

			//�F�̐ݒ�
			col = D3DXCOLOR(colrand + 0.6f, colrand, colrand, 1.0f);

			//���a�̐ݒ�
			fRadius = 30.0f;

			//�����̐ݒ�
			fLife = 1.0f;

			CEffect3D::Create(Defpos + move, move, col, fRadius, fLife, type);
		}

		break;
	}
}