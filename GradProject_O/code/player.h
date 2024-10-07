//==========================================================
//
// プレイヤーの処理 [player.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _PLAYER_H_		// このマクロが定義されていない場合
#define _PLAYER_H_		// 二重インクルード防止用マクロを定義

#include "main.h"
#include "object.h"
#include "objectX.h"
#include "task.h"

// 前方宣言
class CWaist;
class CCharacter;

// マクロ定義
#define MAX_ITEM  (1280)  // 所持できるアイテムの最大数

//==========================================================
// プレイヤーのクラス定義(派生クラス)
//==========================================================
class CPlayer : public CTask
{
public:

	// 操作種類列挙型
	enum TYPE
	{
		TYPE_NONE,	// 操作不可能
		TYPE_SEND,	// データ送信
		TYPE_ACTIVE,	// 操作可能
		TYPE_MAX
	};


	// アクション列挙型
	enum ACTION
	{
		ACTION_NEUTRAL = 0,		// 待機
		ACTION_WALK,			// 歩行
		ACTION_JUMP,			// ジャンプ
		ACTION_SLIDING,			// スライディング
		ACTION_WALLSTAND,		// 壁ずり
		ACTION_WALLKICK,		// 壁キック
		ACTION_SLIDEJUMP,		// しゃがみジャンプ
		ACTION_NORMALATK,		// 攻撃
		ACTION_WALLDUSH,		// 壁走り
		ACTION_CEILINGDUSH,		// 天井走り
		ACTION_KICKUP,			// 蹴りあがり
		ACTION_AXEKICK,			// かかと落とし
		ACTION_RIDERKICK,		// ライダーキック
		ACTION_TITLENEUTRAL,	// タイトル待機
		ACTION_DAMAGE,			// ダメージ
		ACTION_DEATH,			// ダウン
		ACTION_MAX
	};

	// 攻撃種類
	enum ATK
	{
		ATK_NORMAL = 0,	// 通常
		ATK_AXEKICK,	// かかと落とし
		ATK_RIDERKICK,	// ライダーキック
		ATK_SLOWKICK,	// スローライダーキック
		ATK_MAX
	};

private:	// 自分だけがアクセス可能な定義

	// 状態列挙型
	enum STATE
	{
		STATE_APPEAR = 0,	// 出現状態
		STATE_NORMAL,		// 通常状態
		STATE_DAMAGE,		// ダメージ状態
		STATE_DEATH,		// 死亡状態
		STATE_SPAWN,		// 復活中状態
		STATE_MAX
	};

	// 情報構造体
	struct SInfo
	{
		D3DXVECTOR3 pos;		// 位置
		D3DXVECTOR3 rot;		// 向き
		D3DXVECTOR3 move;		// 移動量
		float fSlideMove;		// スライディング開始移動量
		D3DXVECTOR3 posOld;		// 設定位置
		D3DXMATRIX mtxWorld;	// ワールドマトリックス
		STATE state;			// 状態
		float fStateCounter;	// 状態管理カウンター
	};

public:	// 誰でもアクセス可能

	CPlayer();	// コンストラクタ
	~CPlayer();	// デストラクタ

	// メンバ関数
	HRESULT Init(void);
	HRESULT Init(const char *pBodyName, const char *pLegName);	// オーバーロード
	void Uninit(void);
	void Update(void);
	static CPlayer *Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, const D3DXVECTOR3 move,
		const char *pBodyName, const char *pLegName);

	// メンバ関数(設定)
	void SetMove(const D3DXVECTOR3 move) { m_Info.move = move; }
	void SetPosition(const D3DXVECTOR3 pos) { m_Info.pos = pos; }
	void SetRotation(const D3DXVECTOR3 rot) { m_Info.rot = rot; }
	void SetType(TYPE type);
	void BindId(int nId) { m_nId = nId; }
	void SetMotion(int nMotion);
	void SetDraw(bool bDraw);
	void SetNext(CPlayer* pNext) { m_pNext = pNext; }
	void SetPrev(CPlayer* pPrev) { m_pPrev = pPrev; }
	void SetRotDiff(float fDiff) { m_fRotDest = fDiff; }

	// メンバ関数(取得)
	D3DXVECTOR3 GetMove(void) { return m_Info.move; }
	D3DXVECTOR3 GetPosition(void) { return m_Info.pos; }
	D3DXVECTOR3 GetRotation(void) { return m_Info.rot; }
	D3DXVECTOR3 GetOldPosition(void) { return m_Info.posOld; }
	CPlayer* GetNext(void) { return m_pNext; }
	CPlayer* GetPrev(void) { return m_pPrev; }

private:	// 自分だけがアクセス可能

	// メンバ関数
	void SetMatrix(void);
	void StateSet(void);
	void Controller(void);
	void Move(void);

	// メンバ変数
	CPlayer *m_pPrev;			// 前のオブジェクトへのポインタ
	CPlayer *m_pNext;			// 次のオブジェクトへのポインタ
	SInfo m_Info;				// 自分自身の情報
	float m_fRotMove;			// 現在の角度
	float m_fRotDiff;			// 目的の角度
	float m_fRotDest;			// 角度計算
	int m_nId;					// ID
	TYPE m_type;
};

#endif
