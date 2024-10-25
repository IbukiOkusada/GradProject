//==========================================================
//
// �G�f�B�b�g�� [edit_road.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _EDIT_ROAD_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _EDIT_ROAD_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "edit.h"

// �O���錾
class CRoad;
class CEdit_Arrow;

//==========================================================
// �T���v���̃N���X��`
//==========================================================
class CEdit_Road : public CEdit
{
public:	// �N�ł��A�N�Z�X�\

	CEdit_Road();	// �R���X�g���N�^(�I�[�o�[���[�h)
	~CEdit_Road();	// �f�X�g���N�^

	// �����o�֐�
	virtual HRESULT Init(void) override;
	virtual void Uninit(void);
	virtual void Update(void) override;

	// �����o�֐�(�擾)

	// �����o�֐�(�ݒ�)

private:	// �����������A�N�Z�X�\

	// �����o�֐�
	void ClickCheck();	// �I���m�F
	void ReConnect();	// �ĘA��
	void Delete();		// �폜
	void Move();		// �ړ�
	void Save();		// �ۑ�
	void Create();		// ����
	bool CursorCollision(CRoad* pRoad);
	bool TriangleCollision(const D3DXVECTOR3& rayOrigin, const D3DXVECTOR3& rayDir,
		const D3DXVECTOR3& pos0, const D3DXVECTOR3& pos1, const D3DXVECTOR3& pos2);

	// �����o�ϐ�
	CRoad* m_pSelectRoad;	// �I��������
	CEdit_Arrow* m_pArrow;	// ���
	float m_fMoveLength;	// �ړ�����
};

#endif


