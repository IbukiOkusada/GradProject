//===============================================
//
// エフェクトの処理 [effect.h]
// Author : Riku Nakamura
//
//===============================================
#ifndef _EFFECT_EFFEKSEER_H_
#define _EFFECT_EFFEKSEER_H_

#include "task.h"
#include "effekseerControl.h"
#include "list.h"

// 前方宣言

//===============================================
//クラスの定義
//===============================================
class CEffectEffekseer : public CTask
{
public:	// 誰でもアクセス可能な定義

	// 種類の列挙型定義
	typedef enum
	{
		TYPE_SIGNAGE = 0,		// 看板
		TYPE_STOP,				// 停止表示
		TYPE_MAX
	}TYPE;

	// 情報構造体の定義
	struct SInfo
	{
		D3DXVECTOR3 pos;	// 位置
		D3DXVECTOR3 rot;	// 位置
		D3DXVECTOR3 move;	// 移動量
		float fScale;	//半径
		TYPE Type;	// 種類

		// コンストラクタ
		SInfo() : pos(VECTOR3_ZERO), rot(VECTOR3_ZERO), move(VECTOR3_ZERO), fScale(1.0f), Type(TYPE_SIGNAGE) {}
	};

private:	// 自分だけアクセス可能な定義

public:	// 誰でもアクセス可能

	CEffectEffekseer();	// コンストラクタ
	~CEffectEffekseer();	// デストラクタ

	// メンバ関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	static CEffectEffekseer*Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move, float fScale, TYPE type);

	SInfo GetInfo(void) { return m_Info; }

	D3DXVECTOR3 GetPosition(void) { return m_Info.pos; }
	void SetPosition(const D3DXVECTOR3& pos) { m_Info.pos = pos; }

	D3DXVECTOR3& GetRotation(void) { return m_Info.rot; }
	void SetRotation(const D3DXVECTOR3& rot) { m_Info.rot = rot; }

	D3DXVECTOR3& GetMove(void) { return m_Info.move; }
	void SetMove(const D3DXVECTOR3& move) { m_Info.move = move; }

	float GetScale(void) { return m_Info.fScale; }
	void SetScale(const float& fScale) { m_Info.fScale = fScale; }

	TYPE GetType(void) { return m_Info.Type; }

	static Clist<CEffectEffekseer*>* GetList() { return &m_List; }

private:	// 自分だけがアクセス可能

	// メンバ関数
	void SetType(TYPE type) { m_Info.Type = type; }

	// メンバ変数
	SInfo m_Info;	// 情報の構造体
	int m_nId;
	CEffekseer::CEffectData* m_pEffekseer;

	// 静的メンバ変数
	static Clist<CEffectEffekseer*> m_List;
};

#endif 
