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

private:

	// �ړ����
	struct SInfo
	{
		D3DXVECTOR3 startpos;		// ���W
		D3DXVECTOR3 touchpos;		// �O��̍��W
		D3DXVECTOR3 touchworldpos;	// �G�ꂽ�X�N���[�����W

		SInfo() : startpos(VECTOR3_ZERO), touchpos(VECTOR3_ZERO), touchworldpos(VECTOR3_ZERO) {}
	};

	struct SObj
	{
		CObjectX* pObj;
		TYPE type;
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
	SObj* GetHold() { return m_pHold; }
	// �����o�֐�(�ݒ�)
	void SetPosition(const D3DXVECTOR3& pos);

private:	// �����������A�N�Z�X�\

	// �����o�֐�
	void Move();
	void Select();
	void Release();

	// �����o�ϐ�
	SObj m_aObj[TYPE_MAX];
	D3DXVECTOR3 m_pos;	// ���W
	SInfo m_Info;
	SObj* m_pHold;	// �I��
};

#endif

