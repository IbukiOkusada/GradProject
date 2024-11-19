//==========================================================
//
// �z�B�󋵂̏��� [deliverystatus.h]
// Author : Ryosuke Ohara
//
//==========================================================
#ifndef _DELIVERYSTATUS_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _DELIVERYSTATUS_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "task.h"
#include <vector>

// �O���錾
class CObject2D;
class CNumber;

//**********************************************************
// �z�B�󋵃N���X�̒�`
//**********************************************************
class CDeliveryStatus : CTask
{
public:		// �N�ł��A�N�Z�X�\

	CDeliveryStatus();	// �R���X�g���N�^
	CDeliveryStatus(D3DXVECTOR3 pos, int nNumPoint = 2);	// �R���X�g���N�^
	~CDeliveryStatus();	// �f�X�g���N�^

	// �����o�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	static CDeliveryStatus* Create(D3DXVECTOR3 pos, int nNumPoint = 2);

private:	// �����������A�N�Z�X�\

	// �����o�֐�
	void Number(void);
	void Gage(void);

	// �����o�ϐ�
	std::vector<CObject2D*> m_pGageList;
	CObject2D **m_pGage;                    // �Q�[�W�̃|�C���^
	CObject2D* m_pLine;                     // �����̊Ԃ̃X���b�V��
	CNumber* m_pNumDeliveryPointNumber;     // �z�B���鑍����\�������̃|�C���^
	CNumber*m_pCountDeliveryPointNumber;    // ���ݔz�B��������\�������̃|�C���^

	D3DXVECTOR3 m_pos;
	int m_nNumDeliveryPoint;  // �z�B���鐔
};

#endif