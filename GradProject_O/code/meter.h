//==========================================================
//
// ���[�^�[ [meter.h]
// Author : �O�엳�V��
//
//==========================================================
#ifndef _Meter_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _Meter_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "task.h"
#include "number.h"
#include "object2D.h"
#include "player.h"
#include "objectsound.h"

namespace
{
	const int NUMBER_NUM = (5);
	const int METER_NUM = (29);
	const int ANIM_COUNT = (180);
	const float ONE_METRE = (160.0f);
	D3DXVECTOR3 OFFSET_NUMBER = (D3DXVECTOR3(-60.0f, -14.0f, 0.0f));
	D3DXVECTOR3 INTERVAL_NUMBER = (D3DXVECTOR3(30.0f, 0.0f, 0.0f));
	D3DXVECTOR3 OFFSET_METER = (D3DXVECTOR3(0.0f, -120.0f, 0.0f));
	
}
//==========================================================
// �T���v���̃N���X��`
//==========================================================
class CMeter : public CTask
{

public:	// �N�ł��A�N�Z�X�\

	CMeter();	// �R���X�g���N�^(�I�[�o�[���[�h)
	~CMeter();	// �f�X�g���N�^

	// �����o�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	static CMeter* Create(void);
	void BootAnimation();
	void Measure();;
	void Gage();
	// �����o�֐�(�擾)

	// �����o�֐�(�ݒ�)

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

	CNumber*m_pNumber[NUMBER_NUM];
	CObject2D* m_pMeter[METER_NUM];
	CObject2D* m_pFrame;

	CObject2D* m_pNitroGage;
	CObject2D* m_pCircle;
	CObject2D* m_pInnerCircle;
	CMasterSound::CObjectSound * m_pSound;
	STATE m_state;
	int m_nStateCount;
};

#endif