//==========================================================
//
// �G�f�B�b�g��� [edit_arrow.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _EDIT_ARROW_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _EDIT_ARROW_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

class CObjectX;

//==========================================================
// �T���v���̃N���X��`
//==========================================================
class CEdit_Arrow
{
public:

	enum TYPE
	{
		TYPE_ALL = 0,	// �S������
		TYPE_X,			// X���W
		TYPE_Y,			// Y���W
		TYPE_Z,			// Z���W
		TYPE_MAX
	};

public:	// �N�ł��A�N�Z�X�\

	CEdit_Arrow();	// �R���X�g���N�^(�I�[�o�[���[�h)
	~CEdit_Arrow();	// �f�X�g���N�^

	// �����o�֐�
	virtual HRESULT Init(void);
	virtual void Uninit(void);
	virtual void Update(void);
	static CEdit_Arrow* Create(const D3DXVECTOR3& pos);

	// �����o�֐�(�擾)
	D3DXVECTOR3& GetPosition() { return m_pos; }

	// �����o�֐�(�ݒ�)
	void SetPosition(const D3DXVECTOR3& pos);

private:	// �����������A�N�Z�X�\

	// �����o�֐�

	// �����o�ϐ�
	CObjectX* m_apObj[TYPE_MAX];
	D3DXVECTOR3 m_pos;	// ���W
};

#endif

