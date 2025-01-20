//==========================================================
//
// �G�f�B�b�g�G�t�F�N�g [edit_effect.h]
// Author : Riku Nakamura
//
//==========================================================
#ifndef _EDIT_EFFECT_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _EDIT_EFFECT_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "edit.h"

class CEffectEffekseer;
class CEdit_Handle;

//==========================================================
// �T���v���̃N���X��`
//==========================================================
class CEdit_Effect : public CEdit
{
public:	// �N�ł��A�N�Z�X�\

	CEdit_Effect();	// �R���X�g���N�^(�I�[�o�[���[�h)
	~CEdit_Effect();	// �f�X�g���N�^

	// �����o�֐�
	virtual HRESULT Init(void) override;
	virtual void Uninit(void);
	virtual void Update(void) override;

	// �����o�֐�(�擾)

	// �����o�֐�(�ݒ�)

private:	// �����������A�N�Z�X�\

	// �����o�֐�
	void ClickCheck();	// �I���m�F
	void Delete();		// �폜
	void Move();		// �ړ�
	void Scale();		// �X�P�[��
	void Rotate();		// ��]
	void RotateReset();	// ��]���Z�b�g
	void Save();		// �ۑ�
	void Create();		// ����
	void ModeChange();	// �ύX
	bool CursorCollision(CEffectEffekseer* pEffect);
	void ModelChange();	// ���f���ύX
	void ModelSelect();	// ���f���ύX

	// �����o�ϐ�
	D3DXVECTOR3 m_startScale;
	D3DXVECTOR3 m_startRotate;
	CEdit_Handle* m_pHandle;	// ���
	float m_fMouseWheel;	// �}�E�X�z�C�[���̈ړ���
	CEffectEffekseer* m_pSelect;	// �I�𒆃I�u�W�F�N�g
	int m_nIdxType;
};

#endif
