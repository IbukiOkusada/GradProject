//<=================================================
//タイトルでの警察の動き処理 [PlayerTitle.h]
//
//Author : Kazuki Watanabe
//<=================================================
#ifndef _POLICETITLE_H_
#define _POLICETITLE_H_

#include "police.h"

//<========================================
//プレイヤークラスを継承して定義
//<========================================
class CPoliceTitle : public CPolice
{
public:

	CPoliceTitle();
	~CPoliceTitle();

	// メンバ関数
	HRESULT Init(const D3DXVECTOR3 pos);
	void Uninit(void);
	void Update(void);
	static CPoliceTitle* Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, const D3DXVECTOR3 move);

	//パトランプ生成
	void SettingPatLamp(void)
	{
		//<*******************************************
		//パトランプ生成
		if (!m_pPatrolLamp)
		{
			m_pPatrolLamp = CEffekseer::GetInstance()->Create("data\\EFFEKSEER\\patrollamp.efkefc", VECTOR3_ZERO, VECTOR3_ZERO, VECTOR3_ZERO, 45.0f, false, false);
		}
		//ているらんぷ生成
		if (!m_pTailLamp)
		{
			m_pTailLamp = CEffekseer::GetInstance()->Create("data\\EFFEKSEER\\taillamp.efkefc", VECTOR3_ZERO, VECTOR3_ZERO, VECTOR3_ZERO, 45.0f, false, false);
		}

		//パトランプ
		m_pPatrolLamp->m_pos = this->GetPosition();
		m_pPatrolLamp->m_rot = this->GetRotation();

		//ているらんぷ
		m_pTailLamp->m_pos = this->GetPosition();
		m_pTailLamp->m_rot = this->GetRotation();
		//
		//<*******************************************
	}

	//追跡処理
	void Chasing(const float fMoveZ);

private:

	CEffekseer::CEffectData* m_pPatrolLamp;		//パトランプ
	CEffekseer::CEffectData* m_pTailLamp;		//ているらんぷ

};

#endif