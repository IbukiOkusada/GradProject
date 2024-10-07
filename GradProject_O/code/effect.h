//===============================================
//
// エフェクトの処理 [effect.h]
// Author : Ibuki Okusada
//
//===============================================
#ifndef _EFFECT_H_
#define _EFFECT_H_

#include "task.h"
#include "texture.h"

// 前方宣言
class CObjectBillboard;

//===============================================
//クラスの定義
//===============================================
class CEffect : public CTask
{
public:	// 誰でもアクセス可能な定義

	// 種類の列挙型定義
	typedef enum
	{
		TYPE_NONE = 0,			// 通常
		TYPE_SMAKE,				// 煙
		TYPE_MAX
	}TYPE;

private:	// 自分だけアクセス可能な定義

	// 情報構造体の定義
	typedef struct
	{
		float fLife;	// 寿命
		D3DXVECTOR3 pos;	// 位置
		TYPE Type;	// 種類
		D3DXCOLOR col;	//色
		D3DXVECTOR3 move;	// 移動量
		float fRadius;	//半径
	}INFO;

public:	// 誰でもアクセス可能

	CEffect();	// コンストラクタ
	~CEffect();	// デストラクタ

	// メンバ関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	static CEffect *Create(D3DXVECTOR3 pos, TYPE type);
	static CEffect *Create(D3DXVECTOR3 pos, D3DXVECTOR3 move, D3DXCOLOR col, float fRadius, float fLife, TYPE type);
	void SetMove(D3DXVECTOR3 move);
	D3DXVECTOR3 GetPosition(void) { return m_Info.pos; }
	void SetPosition(const D3DXVECTOR3& pos) { m_Info.pos = pos; }
	D3DXVECTOR3 GetMove(void) { return m_Info.move; }
	void SetType(TYPE type) { m_Info.Type = type; }
	CObjectBillboard *GetObj(void) { return m_pObjectBilBoard; }
	float GetRange(void) const;
	D3DXCOLOR GetCol(void) const;
	float GetLife(void) const { return m_Info.fLife; }

private:	// 自分だけがアクセス可能

	// メンバ関数
	void RadiusSet(void);
	void ColorSet(void);
	void InfoSet(void);
	void DrawSet(void);
	CTexture::TYPE SetTex(TYPE type);

	// メンバ変数
	INFO m_Info;	// 情報の構造体
	CObjectBillboard *m_pObjectBilBoard;
};

#endif 
