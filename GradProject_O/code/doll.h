//==========================================================
//
// 人形の処理 [doll.h]
// Author : Riku Nakamura
//
//==========================================================
#ifndef _DOLL_H_		// このマクロが定義されていない場合
#define _DOLL_H_		// 二重インクルード防止用マクロを定義

#include "task.h"
#include "objectX.h"
#include "player.h"
#include <list>
#include "task.h"
#include "effekseerControl.h"

// 前方宣言
class CCharacter;

//==========================================================
// ロボットのクラス定義
//==========================================================
class CDoll : public CTask
{
public:	// 誰でもアクセス可能

	// 状態列挙型
	enum STATE
	{
		STATE_NONE = 0, // 何もない
		STATE_WALK, 	// 歩き状態
		STATE_AVOID,	// 回避状態
		STATE_MAX
	};

	// 情報構造体
	struct SInfo
	{
		D3DXVECTOR3 pos;		// 位置
		D3DXVECTOR3 rot;		// 向き
		D3DXVECTOR3 move;		// 移動量
		STATE state;			// 状態
		int nId;

		// コンストラクタ
		SInfo() : pos(VECTOR3_ZERO), rot(VECTOR3_ZERO),
			move(VECTOR3_ZERO), nId(-1) {}
	};

private:

	

public:	// 誰でもアクセス可能
	
	// モーション列挙型
	enum MOTION
	{
		MOTION_NIGHTOFFIRE = 0,	// night of fireモーション
		MOTION_ENOCH,	// enochモーション
		MOTION_MAX
	};

	CDoll();	// コンストラクタ(オーバーロード)
	~CDoll();	// デストラクタ

	// メンバ関数
	HRESULT Init();
	HRESULT Init(const D3DXVECTOR3& rot);
	void Uninit(void);
	void Update(void);
	static CDoll* Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot);

	// メンバ関数(取得)
	CDoll* GetNext(void) { return m_pNext; }
	CDoll* GetPrev(void) { return m_pPrev; }
	D3DXVECTOR3 GetPosition(void) { return m_Info.pos; }
	D3DXVECTOR3 GetRotation(void) { return m_Info.rot; }
	CDoll::SInfo GetInfo(void) { return m_Info; }

	// メンバ関数(設定)
	void SetMove(const D3DXVECTOR3& move) { m_Info.move = move; }
	void SetPosition(const D3DXVECTOR3& pos);
	void SetRotation(const D3DXVECTOR3& rot);
	void SetState(STATE state) { m_Info.state = state; }

	void SetNext(CDoll* pNext) { m_pNext = pNext; }
	void SetPrev(CDoll* pPrev) { m_pPrev = pPrev; }

protected:	// 派生クラスからもアクセス可能

	// メンバ関数
	void Set();

	// メンバ変数
	CCharacter* m_pCharacter;	// キャラクター

private:	// 自分だけがアクセス可能

	// メンバ関数

	// メンバ変数
	SInfo m_Info;				// 自分自身の情報
	CDoll* m_pPrev;			// 前のオブジェクトへのポインタ
	CDoll* m_pNext;			// 次のオブジェクトへのポインタ
};

#endif

