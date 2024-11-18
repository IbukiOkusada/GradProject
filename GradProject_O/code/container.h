//==========================================================
//
// ���[�^�[ [Container.h]
// Author : �O�엳�V��
//
//==========================================================
#ifndef _Container_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _Container_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "task.h"
#include "number.h"
#include "object2D.h"
#include "player.h"

namespace
{
	const int ICE_NUM = (4);
	const int LOVE_NUM = (2);
	const D3DXVECTOR3 CONTAINER_POS = (D3DXVECTOR3(1100.0f, 580.0f, 0.0f));
	const D3DXVECTOR3 ICE_POS = (D3DXVECTOR3(-60.0f, -35.0f, 0.0f));
	const D3DXVECTOR3 OFFSET_ICE = (D3DXVECTOR3(120.0f, 70.0f, 0.0f));
	const D3DXVECTOR3 OFFSET_LOVE = (D3DXVECTOR3(0.0f, 80.0f, 0.0f));
}
//==========================================================
// �T���v���̃N���X��`
//==========================================================
class CContainer : public CTask
{

public:	// �N�ł��A�N�Z�X�\

	CContainer();	// �R���X�g���N�^(�I�[�o�[���[�h)
	~CContainer();	// �f�X�g���N�^

	// �����o�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	static CContainer* Create(void);
	void BootAnimation();

	// �����o�֐�(�擾)

	// �����o�֐�(�ݒ�)

	struct SIce
	{
		D3DXVECTOR3 pos;
		float fTime;
		CObject2D* pIce;
	};
private:	// �����������A�N�Z�X�\

	// �����o�֐�
	enum STATE
	{
		STATE_NONE = 0,
		STATE_BOOT,
		STATE_NORMAL,
		STATE_MAX
	};
	// �����o�ϐ�
	D3DXVECTOR3 m_pos;


	CObject2D* m_pContainer;
	CObject2D* m_pLove[2];
	SIce* m_pIce[ICE_NUM];
	
	STATE m_state;
	int m_nStateCount;
	float m_fScroll;
};

#endif