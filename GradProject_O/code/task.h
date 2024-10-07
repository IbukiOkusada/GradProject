//==========================================================
//
// タスクの処理 [task.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _TASK_H_		// このマクロが定義されていない場合
#define _TASK_H_		// 二重インクルード防止用マクロを定義

//==========================================================
// サンプルのクラス定義
//==========================================================
class CTask
{
protected:	// 誰でもアクセス可能

	// 種類列挙型の定義
	enum TYPE
	{
		TYPE_NONE = 0,	// 何もない
		TYPE_BG,		// 背景
		TYPE_PLAYER,	// プレイヤー
		TYPE_ENEMY,		// 敵
		TYPE_BULLET,	// 弾
		TYPE_ITEM,		// アイテム
		TYPE_EXPLOSION,	// 爆発
		TYPE_EFFECT,	// エフェクト
		TYPE_BLOCK,		// ブロック
		TYPE_MODEL,		// モデル
		TYPE_CAR,		// 車　
		TYPE_PAUSE,		// ポーズ画面
		TYPE_MAP,		// マップ表示
		TYPE_MAX
	};

public:

	CTask();	// コンストラクタ
	virtual ~CTask();	// デストラクタ

	// メンバ関数
	virtual HRESULT Init(void) = 0; 
	virtual void Uninit(void) = 0;
	virtual void Update(void) = 0;
	CTask *GetNext(void) { return m_pNext; }
	CTask *GetPrev(void) { return m_pPrev; }
	void SetNext(CTask *pNext) { m_pNext = pNext; }
	void SetPrev(CTask *pPrev) { m_pPrev = pPrev; }
	bool GetDeath(void) { return m_bDeath; }

protected:	// 派生クラスからもアクセス可能

	// メンバ関数
	void Release(void);

private:	// 自分だけがアクセス可能

	// メンバ関数

	// メンバ変数
	static CTask *m_pTop;	// 先頭のオブジェクトへのポインタ
	static CTask *m_pCur;	// 最後尾のオブジェクトへのポインタ
	CTask *m_pPrev;	// 前のオブジェクトへのポインタ
	CTask *m_pNext;	// 次のオブジェクトへのポインタ
	bool m_bDeath;	// 死亡フラグ
	TYPE m_type;	// 種類
};

#endif


