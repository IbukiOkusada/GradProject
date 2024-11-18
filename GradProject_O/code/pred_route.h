//==========================================================
//
// ���̓��\������ [pred_route.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _PRED_ROUTE_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _PRED_ROUTE_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "task.h"

// �O���錾
class CPlayer;
class CRoad;

//==========================================================
// �\�������̃N���X��`
//==========================================================
class CPredRoute : public CTask
{

public:	// �N�ł��A�N�Z�X�\

	CPredRoute();	// �R���X�g���N�^(�I�[�o�[���[�h)
	~CPredRoute() override;	// �f�X�g���N�^

	// �����o�֐�
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	static CPredRoute* Create(void);

	// �����o�֐�(�擾)

	// �����o�֐�(�ݒ�)

private:	// �����������A�N�Z�X�\

	// �����o�֐�

	// �����o�ϐ�
	std::vector<CRoad*> m_PassRoad;	// �ʉ߂�����
	CPlayer* m_pPlayer;				// �m�F����v���C���[
};

#endif