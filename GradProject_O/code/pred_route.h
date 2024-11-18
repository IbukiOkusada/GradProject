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
private:

	// ����
	enum VEC
	{
		VEC_STRAIGHT = 0,	// ���i
		VEC_TURNLEFT,		// ���Ȃ���
		VEC_TURN_RIGHT,		// �E�Ȃ���
		VEC_TURNING,		// �]��
		VEC_OVER,			// �����o��
		VEC_MAX
	};

public:	// �N�ł��A�N�Z�X�\

	CPredRoute();	// �R���X�g���N�^(�I�[�o�[���[�h)
	~CPredRoute() override;	// �f�X�g���N�^

	// �����o�֐�
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	static CPredRoute* Create(CPlayer* pPlayer);

	// �����o�֐�(�擾)

	// �����o�֐�(�ݒ�)

private:	// �����������A�N�Z�X�\

	// �����o�֐�
	void Interp();
	void Pred();
	void TurnCheck();

	// �����o�ϐ�
	CRoad* m_pOldRoad;				// �O��̓�
	std::vector<CRoad*> m_PassRoad;	// �ʉ߂�����
	CPlayer* m_pPlayer;				// �m�F����v���C���[
};

#endif