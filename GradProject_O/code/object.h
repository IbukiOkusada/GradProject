//===============================================
//
// オブジェクトの処理 [object.h]
// Author : Ibuki Okusada
//
//===============================================
#ifndef _OBJECT_H_		// このマクロが定義されていない場合
#define _OBJECT_H_		// 二重インクルード防止用マクロを定義

#include "main.h"

// 前方宣言
class CObject2D;
class CMultiBg;
class CEnemy;

//===============================================
// オブジェクトクラスの定義(抽象クラス)
//===============================================
class CObject
{
public:	// 誰でもアクセス可能

	// 種類列挙型の定義
	enum TYPE
	{
		TYPE_NONE = 0,	// 何もない
		TYPE_BG,		// 背景
		TYPE_PLAYER,	// プレイヤー
		TYPE_ENEMY,	// 敵
		TYPE_BULLET,	// 弾
		TYPE_ITEM,	// アイテム
		TYPE_EXPLOSION,	// 爆発
		TYPE_EFFECT,	// エフェクト
		TYPE_BLOCK,	// ブロック
		TYPE_MODEL,	// 3Dモデル
		TYPE_MAX
	};

	//CObject();			// コンストラクタ
	CObject(int nPriority = 3);	// コンストラクタ(オーバーロード)デフォルト引数
	virtual ~CObject();	// デストラクタ

	// 純粋仮想関数関数
	virtual HRESULT Init(void) = 0;
	virtual void Uninit(void) = 0;
	virtual void Draw(void) = 0;

	// メンバ関数(設定)
	virtual void SetPosition(const D3DXVECTOR3) {}
	virtual void SetRotation(const D3DXVECTOR3) {}

	// メンバ関数(取得)
	virtual D3DXVECTOR3 GetPosition(void) { return D3DXVECTOR3(0.0f, 0.0f, 0.0f); }
	virtual D3DXVECTOR3 GetRotation(void) { return D3DXVECTOR3(0.0f, 0.0f, 0.0f); }
	virtual CObject2D *GetObject2D(void) { return NULL; }
	virtual int GetModelType(void) { return -1; }
	virtual D3DXMATRIX *GetMtx(void) { return NULL; }
	virtual CEnemy *GetEnemy(void) { return NULL; }
	virtual bool GetDeath(void) { return m_bDeath; }

	// メンバ関数
	virtual void Hit(float){ }
	void SetDraw(bool bUse = true) { m_bDraw = bUse; }

	// メンバ関数(取得
	TYPE GetType(void) { return m_type; }
	int GetPri(void) { return m_nPriority; }
	CObject *GetNext(void) { return m_pNext; }
	CObject *GetPrev(void) { return m_pPrev; }
	bool GetDraw(void) { return m_bDraw; }

	// メンバ関数(設定)
	void SetType(const TYPE type);
	void SetPriOrity(int nPriority) { m_nPriority = nPriority; }
	void SetNext(CObject *pNext) { m_pNext = pNext; }
	void SetPrev(CObject *pPrev) { m_pPrev = pPrev; }

protected:	// 派生クラスからもアクセス可能

	// メンバ関数
	void Release(void);

private:	// 自分だけがアクセス可能

	// メンバ変数
	CObject *m_pPrev;	// 前のオブジェクトへのポインタ
	CObject *m_pNext;	// 次のオブジェクトへのポインタ
	bool m_bDeath;	// 死亡フラグ
	bool m_bDraw;		// 描画フラグ
	int m_nPriority;	// 優先順位の位置
	TYPE m_type;		// 種類
};

#endif
