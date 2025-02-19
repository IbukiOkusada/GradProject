//==========================================================
//
// 荷物ヘッダー [baggage.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _BAGGAGE_H_		// このマクロが定義されていない場合
#define _BAGGAGE_H_		// 二重インクルード防止用マクロを定義

#include "task.h"
#include "list.h"

//前方宣言
class CModel;
class CPlayer;

//==========================================================
// 荷物のクラス定義
//==========================================================
class CBaggage : public CTask
{
public:

	// 状態列挙型
	enum STATE
	{
		STATE_NONE = 0,
		STATE_THROW,
		STATE_MAX
	};

private:

	// 時間管理構造体
	struct STime
	{
		float fNow;		// 現在
		float fEnd;		// 終了

		// 時間
		STime() : fNow(0.0f), fEnd(0.0f) {}
	};

public:	// 誰でもアクセス可能

	CBaggage();	// コンストラクタ(オーバーロード)
	~CBaggage();	// デストラクタ

	// メンバ関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	static CBaggage* Create(const D3DXVECTOR3& pos);
	static Clist<CBaggage*>* GetList() { return &m_List; }
	static Clist<CBaggage*>* GetThrowList() { return &m_ThrowList; }
	void Set(const D3DXVECTOR3& pos, D3DXVECTOR3* pTarget, float fTime, bool bActive = false);
	void SetPosition(const D3DXVECTOR3& pos) { m_pos = pos; }
	STATE GetState(void) { return m_state; }
	CModel* GetObj() { return m_pObj; }
	void SetThrowScale(const D3DXVECTOR3& scale);

private:	// 自分だけがアクセス可能

	// メンバ関数
	void Throw();
	void SetCamera();

	// メンバ変数
	bool m_bFinish;		// 終了
	bool m_bFirst;		// 初回だけ
	D3DXVECTOR3 m_pos;	// 座標
	D3DXVECTOR3 m_rot;	// 向き
	CModel* m_pObj;	// オブジェクト
	D3DXVECTOR3* m_pTarget;	// 目標
	STime m_time;
	static Clist<CBaggage*> m_List;			// リスト
	static Clist<CBaggage*> m_ThrowList;	// リスト
	STATE m_state;		// 状態
	bool m_bActive;		// 本人かどうか
};

#endif