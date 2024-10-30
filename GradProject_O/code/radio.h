//==========================================================
//
// ���W�I [Radio.h]
// Author : �O�엳�V��
//
//==========================================================
#ifndef _Radio_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _Radio_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "main.h"
#include "objectsound.h"
#include "list.h"
#include "convenience.h"
//==========================================================
// �T���v���̃N���X��`
//==========================================================
class CRadio
{

public:	// �N�ł��A�N�Z�X�\

	CRadio();	// �R���X�g���N�^(�I�[�o�[���[�h)
	~CRadio();	// �f�X�g���N�^

	// �����o�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	static CRadio* Create(void);
	void VolumeChange();
	void ChannelChange();
	enum CHANNEL
	{
		CHANNEL_RELAX = 0,
		CHANNEL_METAL,
		CHANNEL_TECHNO,
		CHANNEL_POPS,
		CHANNEL_SHINOBI,
		CHANNEL_MAX
	};
	struct radio
	{
		Clist<CMasterSound::CObjectSound*>* m_pList;
		int nCurrent;
	};
private:	// �����������A�N�Z�X�\

	// �����o�֐�

	// �����o�ϐ�
	float m_fVolume;
	int m_nChannel;
	bool m_bSwitch;
	radio m_pRadio[CHANNEL_MAX];
	CMasterSound::CObjectSound* m_pSE;
	float m_fFade;
};

#endif

