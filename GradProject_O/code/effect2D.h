//===============================================
//
// �G�t�F�N�g�̏��� [effect.h]
// Author : Ibuki Okusada
//
//===============================================
#ifndef _EFFECT_H_
#define _EFFECT_H_

#include "task.h"
#include "texture.h"

// �O���錾
class CObject2D;

//===============================================
//�N���X�̒�`
//===============================================
class CEffect2D : public CTask
{
public:	// �N�ł��A�N�Z�X�\�Ȓ�`

	// ��ނ̗񋓌^��`
	typedef enum
	{
		TYPE_NONE = 0,			// �ʏ�
		TYPE_SMAKE,				// ��
		TYPE_TARGET,			// �S�[��
		TYPE_MAX
	}TYPE;

private:	// ���������A�N�Z�X�\�Ȓ�`

	// ���\���̂̒�`
	struct INFO
	{
		float fLife;	// ����
		D3DXVECTOR3 pos;	// �ʒu
		TYPE Type;	// ���
		D3DXCOLOR col;	//�F
		D3DXVECTOR3 move;	// �ړ���
		float fRadius;	//���a

		// �R���X�g���N�^
		INFO() : fLife(0.0f), pos(VECTOR3_ZERO), Type(TYPE_NONE), col({0.0f, 0.0f, 0.0f, 0.0f}), move(VECTOR3_ZERO), fRadius(0.0f) {}
	};

public:	// �N�ł��A�N�Z�X�\

	CEffect2D();	// �R���X�g���N�^
	~CEffect2D();	// �f�X�g���N�^

	// �����o�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	static CEffect2D* Create(D3DXVECTOR3 pos, TYPE type);
	static CEffect2D* Create(D3DXVECTOR3 pos, D3DXVECTOR3 move, D3DXCOLOR col, float fRadius, float fLife, TYPE type);
	void SetMove(D3DXVECTOR3 move);
	D3DXVECTOR3 GetPosition(void) { return m_Info.pos; }
	void SetPosition(const D3DXVECTOR3& pos) { m_Info.pos = pos; }
	D3DXVECTOR3 GetMove(void) { return m_Info.move; }
	void SetType(TYPE type) { m_Info.Type = type; }
	CObject2D* GetObj(void) { return m_pObj; }
	float GetRange(void) const;
	D3DXCOLOR GetCol(void) const;
	float GetLife(void) const { return m_Info.fLife; }

private:	// �����������A�N�Z�X�\

	// �����o�֐�
	void RadiusSet(void);
	void ColorSet(void);
	void InfoSet(void);
	void DrawSet(void);
	CTexture::TYPE SetTex(TYPE type);

	// �����o�ϐ�
	INFO m_Info;	// ���̍\����
	CObject2D* m_pObj;
};

#endif 

