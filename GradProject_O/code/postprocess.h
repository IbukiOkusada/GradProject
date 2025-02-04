//============================================================
//
//	�V�F�[�_�[�Ǘ�[Postprocess.h]
//	Author�F�O�엳�V��
//
//============================================================
//************************************************************
//	��d�C���N���[�h�h�~
//************************************************************
#ifndef _postprocess_H_
#define _postprocess_H_

//************************************************************
//	�C���N���[�h�t�@�C��
//************************************************************
#include"texture.h"
#include "shaderlight.h"
#include "object2D.h"
//************************************************************
//	�N���X��`
//************************************************************
// �V�F�[�_�[�N���X
class CPostprocess
{
public:

	// �R���X�g���N�^
	CPostprocess();

	// �f�X�g���N�^
	~CPostprocess();

	// �I�[�o�[���C�h�֐�
	HRESULT Init(void);	// ������
	void Uninit(void);		// �I��

	void Draw();
	void Rendering(LPDIRECT3DTEXTURE9 pTex, D3DXCOLOR col);

	// �ÓI�����o�֐�
	static CPostprocess* Create(void);		// ����
	static CPostprocess* GetInstance(void);	// �擾
	static void Release(void);				// �j��
	HRESULT Loadshader(char* path, LPD3DXEFFECT* pEffect);
private:
	// �ÓI�����o�ϐ�
	static CPostprocess* m_pShader;	// �V�F�[�_�[���

	// �����o�ϐ�
	LPD3DXEFFECT m_pBright;						// �G�t�F�N�g
	LPD3DXEFFECT m_pGauss;						// �G�t�F�N�g
	LPD3DXEFFECT m_pAberration;						// �G�t�F�N�g

	LPDIRECT3DTEXTURE9 m_pGaussTex;
	LPDIRECT3DSURFACE9 m_pGaussSurface;
	ID3DXSprite* m_pSprite;
	float time;
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff;	// ���_�o�b�t�@
};

#endif	// _postprocess_H_
