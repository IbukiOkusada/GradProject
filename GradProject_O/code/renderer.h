//==========================================================
//
// DirectX�̃����_���[���� [renderer.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _RENDERER_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _RENDERER_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "main.h"

// �����_���[�N���X�̒�`
class CRenderer
{
private:

	// �}���`�^�[�Q�b�g�����_�����O�p���
	struct MultiTargetInfo
	{
		float fStartColAlpha;	// �J�n�����x
		float fStartMulti;		// �J�n�T�C�Y�{��
		float fAddTimer;		// �^�C�}�[������
		float fColAlpha;		// �����x
		float fTimer;			// �J�ڃ^�C�}�[
		float fMulti;			// �T�C�Y�{��
		bool bActive;			// �ғ���

		// �R���X�g���N�^
		MultiTargetInfo() : fStartColAlpha(0.0f), fStartMulti(0.0f), fAddTimer(0.0f), 
			fColAlpha(0.0f), fMulti(0.0f), fTimer(0.0f), bActive(false){ }
	};

	// �}���`�^�[�Q�b�g�����_�����O�p�����f�[�^
	struct MultiTarget
	{
		LPDIRECT3DTEXTURE9 pTextureMT[2];	// �����_�����O�^�[�Q�b�g�p�e�N�X�`��
		LPDIRECT3DSURFACE9 pRenderMT[2];	// �e�N�X�`�������_�����O�p�C���^�[�t�F�[�X
		LPDIRECT3DSURFACE9 pZBuffMT;		// �e�N�X�`�������_�����O�pZ�o�b�t�@
		D3DVIEWPORT9 viewportMT;			// �e�N�X�`�������_�����O�p�r���[�|�[�g
		LPDIRECT3DVERTEXBUFFER9 pVtxBuff;	// ���_�o�b�t�@
	};

public:	// �N�ł��A�N�Z�X�\

	CRenderer();	// �R���X�g���N�^
	~CRenderer();	// �f�X�g���N�^

	// �����o�֐�
	HRESULT Init(HWND hWnd, BOOL bWindow);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	LPDIRECT3DDEVICE9 GetDevice(void);

	// �}���`�^�[�Q�b�g�֘A
	void GetDefaultRenderTarget(LPDIRECT3DSURFACE9* pRender, LPDIRECT3DSURFACE9* pZBuff, D3DXMATRIX* viewport, D3DXMATRIX* projection);	// �f�t�H���g�̃����_�[�^�[�Q�b�g�擾
	void ChangeRendertarget(LPDIRECT3DSURFACE9 pRender, LPDIRECT3DSURFACE9 pZBuff, D3DXMATRIX viewport, D3DXMATRIX projection);	// �^�[�Q�b�g�ؖ�
	void ChangeTarget(const D3DXVECTOR3& posV, const D3DXVECTOR3& posR, const D3DXVECTOR3& vecU);
	LPDIRECT3DTEXTURE9 GetTextureMT(int idx) { return m_Multitarget.pTextureMT[idx]; }	// �����_�����O�^�[�Q�b�g�p�e�N�X�`���擾
	float GetGoalAlpha() { return m_MultitargetInfo.fColAlpha; }
	/**
	@brief	�}���`�^�[�Q�b�g��ʂ̕`�攻��
	@param	fGoalAlpha	[in]	�ڕW�����F
	@param	fGoalMulti	[in]	�ڕW��ʔ{��
	@param	fTimer		[in]	�ڕW�܂ł̎���
	@return	void
	*/
	void SetEnableDrawMultiScreen(float fGoalAlpha, float fGoalMulti, float fTimer);

private:	// �����������A�N�Z�X�\

	// �����o�֐�
	void SetWire(bool bWire);

	// �}���`�^�[�Q�b�g�����_���[�̏�����
	void InitMTRender();
	void SetMultiTarget();
	void DrawMultiTargetScreen(int texIdx, const D3DXCOLOR& col, const D3DXVECTOR2& size);	// �}���`�^�[�Q�b�g��ʂ̕`��

	// �����o�ϐ�
	LPDIRECT3D9 m_pD3D;				//Direct3D�I�u�W�F�N�g�̃|�C���^
	LPDIRECT3DDEVICE9 m_pD3DDevice;	//Direct3D�f�o�C�X�ւ̃|�C���^
	bool m_bWire;					// ���C���[�t���[���ɂ��邩�ۂ�
	LPDIRECT3DTEXTURE9 pTexture;
	LPDIRECT3DSURFACE9 m_pRenderTextureSurface;
	LPDIRECT3DSURFACE9 m_pZSurface;
	LPDIRECT3DSURFACE9 m_pOrgSurface;
	LPDIRECT3DSURFACE9 m_pOrgZBuffer;
	MultiTargetInfo m_MultitargetInfo;
	MultiTarget m_Multitarget;	// �}���`�^�[�Q�b�g�p
};

#endif