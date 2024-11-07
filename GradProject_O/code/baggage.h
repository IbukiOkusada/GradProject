//==========================================================
//
// �ו��w�b�_�[ [baggage.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _BAGGAGE_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _BAGGAGE_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "task.h"
#include "list.h"

//�O���錾
class CObjectX;
class CPlayer;

//==========================================================
// �ו��̃N���X��`
//==========================================================
class CBaggage : public CTask
{
private:

	// ��ԗ񋓌^
	enum STATE
	{
		STATE_NONE = 0,
		STATE_THROW,
		STATE_MAX
	};

	// ���ԊǗ��\����
	struct STime
	{
		float fNow;		// ����
		float fEnd;		// �I��

		// ����
		STime() : fNow(0.0f), fEnd(0.0f) {}
	};

public:	// �N�ł��A�N�Z�X�\

	CBaggage();	// �R���X�g���N�^(�I�[�o�[���[�h)
	~CBaggage();	// �f�X�g���N�^

	// �����o�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	static CBaggage* Create(const D3DXVECTOR3& pos);
	static Clist<CBaggage*>* GetList() { return &m_List; }
	void Set(const D3DXVECTOR3& pos, D3DXVECTOR3* pTarget, float fTime);
	void SetPosition(const D3DXVECTOR3& pos) { m_pos = pos; }

private:	// �����������A�N�Z�X�\

	// �����o�֐�
	void Throw();
	void SetCamera();

	// �����o�ϐ�
	bool m_bFinish;		// �I��
	bool m_bFirst;		// ���񂾂�
	D3DXVECTOR3 m_pos;	// ���W
	D3DXVECTOR3 m_rot;	// ����
	CObjectX* m_pObj;	// �I�u�W�F�N�g
	D3DXVECTOR3* m_pTarget;	// �ڕW
	STime m_time;
	static Clist<CBaggage*> m_List;	// ���X�g
	STATE m_state;		// ���
};

#endif