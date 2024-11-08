//==========================================================
//
// ���ˋ� [Bridge.h]
// Author : �O�엳�V��
//
//==========================================================
#ifndef _Bridge_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _Bridge_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "gimmick.h"
#include "objectX.h"
//�萔
namespace
{
	const int BRIDGE_NUM = (2);//���̃p�[�c��(��������Ȃ̂œ��R2)
};
//==========================================================
// ���ˋ��̃N���X��`
//==========================================================

class CBridge : public CGimmick
{

public:	// �N�ł��A�N�Z�X�\

	CBridge();	// �R���X�g���N�^(�I�[�o�[���[�h)
	~CBridge();	// �f�X�g���N�^

	// �����o�֐�
	HRESULT Init(void)		override;
	void	Uninit(void)	override;
	void	Update(void)	override;
	void MoveBridge();
	static CBridge* Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale, float fdown,float fup);
private:	// �����������A�N�Z�X�\

	// �����o�֐�
	float m_fUpTime;//���Ă鎞��
	float m_fDownTime;//�󂢂Ă鎞��
	float m_fTimeCount;//�o�ߎ���
	bool m_bPass;//�ʍs�\��
	// �����o�ϐ�
	CObjectX* m_pBridge[BRIDGE_NUM];
};

#endif
