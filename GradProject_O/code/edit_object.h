//==========================================================
//
// �G�f�B�b�g�I�u�W�F�N�g [edit_object.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _EDIT_OBJ_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _EDIT_OBJ_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "edit.h"

class CMapObstacle;
class CEdit_Arrow;

//==========================================================
// �T���v���̃N���X��`
//==========================================================
class CEdit_Obj : public CEdit
{
public:	// �N�ł��A�N�Z�X�\

	CEdit_Obj();	// �R���X�g���N�^(�I�[�o�[���[�h)
	~CEdit_Obj();	// �f�X�g���N�^

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
	void Save();		// �ۑ�
	void Create();		// ����
	bool CursorCollision(CMapObstacle* pObj);
	void ModelChange();	// ���f���ύX

	// �����o�ϐ�
	CEdit_Arrow* m_pArrow;	// ���
	float m_fMouseWheel;	// �}�E�X�z�C�[���̈ړ���
	CMapObstacle* m_pSelect;	// �I�𒆃I�u�W�F�N�g
};

#endif

