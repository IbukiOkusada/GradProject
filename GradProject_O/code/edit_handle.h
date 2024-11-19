//==========================================================
//
// �G�f�B�b�g�n���h����� [edit_handle.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _EDIT_HANDLE_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _EDIT_HANDLE_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

class CObjectX;

//==========================================================
// �G�f�B�b�g�n���h���̃N���X��`
//==========================================================
class CEdit_Handle
{
public:

	// ���
	enum TYPE
	{
		TYPE_MOVE = 0,	// �ړ�
		TYPE_SCALE,		// �X�P�[��
		TYPE_ROT,		// ����
		TYPE_MAX
	};

	// ����
	enum VEC
	{
		VEC_ALL = 0,	// �S������
		VEC_X,			// X���W
		VEC_Y,			// Y���W
		VEC_Z,			// Z���W
		VEC_MAX
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
		VEC type;
	};

public:	// �N�ł��A�N�Z�X�\

	CEdit_Handle();	// �R���X�g���N�^(�I�[�o�[���[�h)
	virtual ~CEdit_Handle();	// �f�X�g���N�^

	// �����o�֐�
	virtual HRESULT Init(void) = 0;
	virtual void Uninit(void);
	virtual void Update(void);
	static CEdit_Handle* Create(const D3DXVECTOR3& pos, const TYPE type);

	// �����o�֐�(�擾)
	D3DXVECTOR3& GetPosition() { return m_pos; }
	D3DXVECTOR3& GetDiffPosition() { return m_pos - m_Info.startpos; }
	SObj* GetHold() { return m_pHold; }
	SObj* GetOldHold() { return m_pOldHold; }
	// �����o�֐�(�ݒ�)
	void SetPosition(const D3DXVECTOR3& pos);
	TYPE GetType() { return m_type; }

protected:

	// �����o�֐�
	virtual void Move(){}
	virtual void Scale(){}
	virtual void Rotation(){}
	SObj* GetHoldObj() { return m_pHold; }

	// �����o�ϐ�
	D3DXVECTOR3 m_pos;	// ���W
	SInfo m_Info;
	SObj m_aObj[VEC_MAX];

private:	// �����������A�N�Z�X�\

	// �����o�ϐ�
	SObj* m_pHold;	// �I��
	SObj* m_pOldHold;
	TYPE m_type;
	void Select();
	void Release();
};

#endif

