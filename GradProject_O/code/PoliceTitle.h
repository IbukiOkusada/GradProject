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

	//�p�g�����v����
	void SettingPatLamp(void)
	{
		//<*******************************************
		//�p�g�����v����
		if (!m_pPatrolLamp)
		{
			m_pPatrolLamp = CEffekseer::GetInstance()->Create("data\\EFFEKSEER\\patrollamp.efkefc", VECTOR3_ZERO, VECTOR3_ZERO, VECTOR3_ZERO, 45.0f, false, false);
		}

		//�ʒu�ƌ�����ݒ�
		m_pPatrolLamp->m_pos = this->GetPosition();
		m_pPatrolLamp->m_rot = this->GetRotation();
	}

	//
	void Chasing(const float fMoveZ);

private:

	CEffekseer::CEffectData* m_pPatrolLamp;		//�e�X�g�p
	CEffekseer::CEffectData* m_pTailLamp;

};

#endif