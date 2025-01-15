//==========================================================
//
// �X�|�b�g���C�g�̏��� [spot_light.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _SPOT_LIGHT_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _SPOT_LIGHT_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

//**********************************************************
// ���C�g�N���X�̒�`
//**********************************************************
class CSpotLight
{
public:	// �N�ł��A�N�Z�X�\
	CSpotLight();	// �R���X�g���N�^
	~CSpotLight();	// �f�X�g���N�^

	// �����o�֐�
	HRESULT Init(void);
	static CSpotLight* Create();
	void Uninit();
	void Update();

	//==========================
	// �ݒ�֐�
	//==========================
	// ���W�ݒ�
	void SetPositon(const D3DXVECTOR3& pos);
	// �����ݒ�
	void SetDirection(const D3DXVECTOR3& dic);
	// �g�U���ݒ�
	void SetDiffuse(const D3DXCOLOR& dif);
	// �͈͐ݒ�
	void SetRange(const float fRange);

	/**
	@brief	�X�|�b�g���C�g�̃p�����[�^�ݒ�
	@param	fOutRadian	[in]	�O���̃R�[���̊p�x
	@param	fInRadian	[in]	�����̃R�[���̊p�x
	@param	fFallOff	[in]	�����ƊO���̃R�[���̌�����
	@return	void
	*/
	void SetParameter(float fOutRadian, float fInRadian, float fFallOff = 1.0f);
	
	/**
	@brief	�X�|�b�g���C�g�̌����ݒ�
	@param	fOutRadian	[in]	�萔����
	@param	fInRadian	[in]	���`����
	@param	fFallOff	[in]	�񎟌���
	@return	void
	*/
	void SetAttenuation(float fConstant, float fLinear, float fSecondary);

private:	// �����������A�N�Z�X�\

	D3DLIGHT9 m_Light;	// ���C�g
	int m_nId;
};

#endif

