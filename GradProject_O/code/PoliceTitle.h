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
	HRESULT Init(void);
	void Uninit(void);
	void Update(void) {}
	static CPoliceTitle* Create(const D3DXVECTOR3 rot, const D3DXVECTOR3 move);

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
		
		//<*******************************************
	}

	//サイレン情報設定
	inline static void SetSiren(const bool bStart)
	{
		//開始なら再生、していないなら止める
		if (bStart) { m_pSound->Start(); }
		else{ m_pSound->Stop(); }
	}

	//追跡処理
	void Chasing(const float fMoveZ);

	void TitleMove(void);
private:

	void Move(void);

	static int m_nNumAll;

	CEffekseer::CEffectData* m_pPatrolLamp;		//パトランプ
	CEffekseer::CEffectData* m_pTailLamp;		//ているらんぷ

	static CMasterSound::CObjectSound* m_pSound;		//パトロール(追跡時の音)

	int m_nNumDest;						//目的地の番号

};

#endif