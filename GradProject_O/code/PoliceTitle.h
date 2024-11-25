//<=================================================
//�^�C�g���ł̌x�@�̓������� [PlayerTitle.h]
//
//Author : Kazuki Watanabe
//<=================================================
#ifndef _POLICETITLE_H_
#define _POLICETITLE_H_

#include "police.h"

//<========================================
//�v���C���[�N���X���p�����Ē�`
//<========================================
class CPoliceTitle : public CPolice
{
public:

	CPoliceTitle();
	~CPoliceTitle();

	// �����o�֐�
	HRESULT Init(const D3DXVECTOR3 pos);
	void Uninit(void);
	void Update(void);
	static CPoliceTitle* Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, const D3DXVECTOR3 move);


};

#endif