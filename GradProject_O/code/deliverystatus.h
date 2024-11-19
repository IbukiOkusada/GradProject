//==========================================================
//
// 配達状況の処理 [deliverystatus.h]
// Author : Ryosuke Ohara
//
//==========================================================
#ifndef _DELIVERYSTATUS_H_		// このマクロが定義されていない場合
#define _DELIVERYSTATUS_H_		// 二重インクルード防止用マクロを定義

#include "task.h"
#include <vector>

// 前方宣言
class CObject2D;
class CNumber;

//**********************************************************
// 配達状況クラスの定義
//**********************************************************
class CDeliveryStatus : CTask
{
public:		// 誰でもアクセス可能

	CDeliveryStatus();	// コンストラクタ
	CDeliveryStatus(D3DXVECTOR3 pos, int nNumPoint = 2);	// コンストラクタ
	~CDeliveryStatus();	// デストラクタ

	// メンバ関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	static CDeliveryStatus* Create(D3DXVECTOR3 pos, int nNumPoint = 2);

private:	// 自分だけがアクセス可能

	// メンバ関数
	void Number(void);
	void Gage(void);

	// メンバ変数
	std::vector<CObject2D*> m_pGageList;
	CObject2D **m_pGage;                    // ゲージのポインタ
	CObject2D* m_pLine;                     // 数字の間のスラッシュ
	CNumber* m_pNumDeliveryPointNumber;     // 配達する総数を表す数字のポインタ
	CNumber*m_pCountDeliveryPointNumber;    // 現在配達した数を表す数字のポインタ

	D3DXVECTOR3 m_pos;
	int m_nNumDeliveryPoint;  // 配達する数
};

#endif