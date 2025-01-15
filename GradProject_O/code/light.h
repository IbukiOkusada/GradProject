//==========================================================
//
// ���C�g�̏����S�� [light.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _LIGHT_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _LIGHT_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "main.h"

//==========================================================
// �}�N����`
//==========================================================
#define NUM_LIGHT	(3)	// ���C�g�̑���

//**********************************************************
// ���C�g�N���X�̒�`
//**********************************************************
class CLight
{
public:	// �N�ł��A�N�Z�X�\
	CLight();	// �R���X�g���N�^
	~CLight();	// �f�X�g���N�^

	// �����o�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	static void AddNumLight() { m_nNumLightCnt++; }
	static void MinusNumLight() { m_nNumLightCnt--; }
	static int GetNumLight() { return m_nNumLightCnt; }

private:	// �����������A�N�Z�X�\
	D3DLIGHT9 m_aLight[NUM_LIGHT];		// ���C�g�̏��
	static int m_nNumLightCnt;			// ���C�g�̑���
};

#endif
