//===============================================
//
// �G�t�F�N�g�̏��� [effect.h]
// Author : Riku Nakamura
//
//===============================================
#ifndef _EFFECT_EFFEKSEER_H_
#define _EFFECT_EFFEKSEER_H_

#include "task.h"
#include "effekseerControl.h"
#include "list.h"

// �O���錾

//===============================================
//�N���X�̒�`
//===============================================
class CEffectEffekseer : public CTask
{
public:	// �N�ł��A�N�Z�X�\�Ȓ�`

	// ��ނ̗񋓌^��`
	typedef enum
	{
		TYPE_SIGNAGE = 0,		// �Ŕ�
		TYPE_STOP,				// ��~�\��
		TYPE_MAX
	}TYPE;

	// ���\���̂̒�`
	struct SInfo
	{
		D3DXVECTOR3 pos;	// �ʒu
		D3DXVECTOR3 rot;	// �ʒu
		D3DXVECTOR3 move;	// �ړ���
		float fScale;	//���a
		TYPE Type;	// ���

		// �R���X�g���N�^
		SInfo() : pos(VECTOR3_ZERO), rot(VECTOR3_ZERO), move(VECTOR3_ZERO), fScale(1.0f), Type(TYPE_SIGNAGE) {}
	};

private:	// ���������A�N�Z�X�\�Ȓ�`

public:	// �N�ł��A�N�Z�X�\

	CEffectEffekseer();	// �R���X�g���N�^
	~CEffectEffekseer();	// �f�X�g���N�^

	// �����o�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	static CEffectEffekseer*Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move, float fScale, TYPE type);

	SInfo GetInfo(void) { return m_Info; }

	D3DXVECTOR3 GetPosition(void) { return m_Info.pos; }
	void SetPosition(const D3DXVECTOR3& pos) { m_Info.pos = pos; }

	D3DXVECTOR3& GetRotation(void) { return m_Info.rot; }
	void SetRotation(const D3DXVECTOR3& rot) { m_Info.rot = rot; }

	D3DXVECTOR3& GetMove(void) { return m_Info.move; }
	void SetMove(const D3DXVECTOR3& move) { m_Info.move = move; }

	float GetScale(void) { return m_Info.fScale; }
	void SetScale(const float& fScale) { m_Info.fScale = fScale; }

	TYPE GetType(void) { return m_Info.Type; }

	static Clist<CEffectEffekseer*>* GetList() { return &m_List; }

private:	// �����������A�N�Z�X�\

	// �����o�֐�
	void SetType(TYPE type) { m_Info.Type = type; }

	// �����o�ϐ�
	SInfo m_Info;	// ���̍\����
	int m_nId;
	CEffekseer::CEffectData* m_pEffekseer;

	// �ÓI�����o�ϐ�
	static Clist<CEffectEffekseer*> m_List;
};

#endif 
