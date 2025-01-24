//==========================================================
//
// �}�b�v�z�u�̃L�����N�^�[�̏��� [doll.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _MAP_CHARACTER_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _MAP_CHARACTER_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "task.h"
#include "objectX.h"
#include "player.h"
#include <list>
#include "task.h"
#include "effekseerControl.h"

// �O���錾
class CCharacter;

//==========================================================
// ���{�b�g�̃N���X��`
//==========================================================
class CMapCharacter : public CTask
{
public:	// �N�ł��A�N�Z�X�\

	// ���\����
	struct SInfo
	{
		D3DXVECTOR3 pos;		// �ʒu
		D3DXVECTOR3 rot;		// ����
		D3DXVECTOR3 scale;		// �ړ���

		// �R���X�g���N�^
		SInfo() : pos(VECTOR3_ZERO), rot(VECTOR3_ZERO),
			scale(VECTOR3_ZERO) {}
	};

private:

	

public:	// �N�ł��A�N�Z�X�\

	CMapCharacter();	// �R���X�g���N�^(�I�[�o�[���[�h)
	~CMapCharacter();	// �f�X�g���N�^

	// �����o�֐�
	HRESULT Init();
	HRESULT Init(const D3DXVECTOR3& rot, const char* pPath, int nPlayMotion);
	void Uninit(void);
	void Update(void);
	static CMapCharacter* Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, const D3DXVECTOR3& scale, const char* pPath, int nPlayMotion);

	// �����o�֐�(�擾)
	D3DXVECTOR3 GetPosition(void) { return m_Info.pos; }
	D3DXVECTOR3 GetRotation(void) { return m_Info.rot; }
	CMapCharacter::SInfo GetInfo(void) { return m_Info; }

	// �����o�֐�(�ݒ�)
	void SetScale(const D3DXVECTOR3& scale);
	void SetPosition(const D3DXVECTOR3& pos);
	void SetRotation(const D3DXVECTOR3& rot);

protected:	// �h���N���X������A�N�Z�X�\

	// �����o�֐�
	void Set();

	// �����o�ϐ�
	CCharacter* m_pCharacter;	// �L�����N�^�[

private:	// �����������A�N�Z�X�\

	// �����o�֐�

	// �����o�ϐ�
	SInfo m_Info;				// �������g�̏��
};

#endif

