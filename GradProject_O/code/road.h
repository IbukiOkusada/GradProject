//==========================================================
//
// ������ [road.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _ROAD_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _ROAD_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "task.h"

// �O���錾
class CObject3D;

//==========================================================
// ���̃N���X��`
//==========================================================
class CRoad : public CTask
{
public:	// �N�ł��A�N�Z�X�\�Ȓ�`

	// ���A�����
	enum DIRECTION
	{
		DIC_UP = 0,	// ��
		DIC_DOWN,	// ��
		DIC_LEFT,	// ��
		DIC_RIGHT,	// �E
		DIC_MAX
	};

	// �����
	enum TYPE
	{
		TYPE_STOP = 0,		// ��~��
		TYPE_NONE,			// �܂�������
		TYPE_CURVE,			// �J�[�u
		TYPE_T_JUNCTION,	// T���H
		TYPE_CROSSING,		// �����_
		TYPE_MAX
	};

	// ��{���\����
	struct SInfo
	{
		D3DXVECTOR3 pos;	// ���W
		D3DXVECTOR3 rot;	// ����
		D3DXVECTOR2 size;	// ��

		// �R���X�g���N�^
		SInfo() : pos({ 0.0f, 0.0f, 0.0f }), rot({ 0.0f, 0.0f, 0.0f }), size({ 0.0f, 0.0f }) {}
		SInfo(const D3DXVECTOR3& _pos, const D3DXVECTOR3& _rot, const D3DXVECTOR2& _size) : pos(_pos), rot(_rot), size(_size) {}
	};

public:	// �N�ł��A�N�Z�X�\

	//CRoad();	// �R���X�g���N�^(�I�[�o�[���[�h)
	CRoad(const SInfo& info = SInfo());
	~CRoad();	// �f�X�g���N�^

	// �����o�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	static CRoad* Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, const D3DXVECTOR2& size);
	void BindTexture();

	// �����o�֐�(�擾)
	CRoad* GetNext(void) { return m_pNext; }	// ��
	CRoad* GetPrev(void) { return m_pPrev; }	// �O��
	CObject3D* GetObj(void) { return m_pObj; }	// �`��I�u�W�F
	SInfo& GetInfo(void) { return m_Info; }		// ��{���
	D3DXVECTOR3& GetPosition(void) { return m_Info.pos; }	// ���W
	D3DXVECTOR2& GetSize(void) { return m_Info.size; }		// ������
	CRoad* GetConnectRoad(const DIRECTION& dic) // �A�����H
	{ return m_apConnectRoad[dic]; }
	float GetConnectLength(const DIRECTION& dic) // �A�����H�ւ̋���
	{ return m_apConnectLength[dic]; }
	TYPE GetType(void) { return m_Type; }

	// �����o�֐�(�ݒ�)
	void Connect(CRoad* pRoad, const DIRECTION dic);
	void SetNext(CRoad* pNext) { m_pNext = pNext; }
	void SetPrev(CRoad* pPrev) { m_pPrev = pPrev; }

private:	// �����������A�N�Z�X�\

	// �����o�֐�
	void Rotation(TYPE type);

	// �����o�ϐ�
	CRoad* m_pPrev;			// �O�̃I�u�W�F�N�g�ւ̃|�C���^
	CRoad* m_pNext;			// ���̃I�u�W�F�N�g�ւ̃|�C���^
	SInfo m_Info;	// ��{���
	CObject3D* m_pObj;
	int m_nIdx;
	CRoad* m_apConnectRoad[DIRECTION::DIC_MAX];	// �A��������
	float m_apConnectLength[DIRECTION::DIC_MAX];	// �A���������ւ̋���
	TYPE m_Type;
};

#endif
