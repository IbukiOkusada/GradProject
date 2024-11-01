//==========================================================
//
// エディットハンドル基底 [edit_handle.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _EDIT_HANDLE_H_		// このマクロが定義されていない場合
#define _EDIT_HANDLE_H_		// 二重インクルード防止用マクロを定義

class CObjectX;

//==========================================================
// エディットハンドルのクラス定義
//==========================================================
class CEdit_Handle
{
public:

	// 種類
	enum TYPE
	{
		TYPE_MOVE = 0,	// 移動
		TYPE_SCALE,		// スケール
		TYPE_ROT,		// 向き
		TYPE_MAX
	};

	// 方向
	enum VEC
	{
		VEC_ALL = 0,	// 全部動く
		VEC_X,			// X座標
		VEC_Y,			// Y座標
		VEC_Z,			// Z座標
		VEC_MAX
	};

private:

	// 移動情報
	struct SInfo
	{
		D3DXVECTOR3 startpos;		// 座標
		D3DXVECTOR3 touchpos;		// 前回の座標
		D3DXVECTOR3 touchworldpos;	// 触れたスクリーン座標

		SInfo() : startpos(VECTOR3_ZERO), touchpos(VECTOR3_ZERO), touchworldpos(VECTOR3_ZERO) {}
	};

	struct SObj
	{
		CObjectX* pObj;
		VEC type;
	};

public:	// 誰でもアクセス可能

	CEdit_Handle();	// コンストラクタ(オーバーロード)
	virtual ~CEdit_Handle();	// デストラクタ

	// メンバ関数
	virtual HRESULT Init(void) = 0;
	virtual void Uninit(void);
	virtual void Update(void);
	static CEdit_Handle* Create(const D3DXVECTOR3& pos, const TYPE type);

	// メンバ関数(取得)
	D3DXVECTOR3& GetPosition() { return m_pos; }
	D3DXVECTOR3& GetDiffPosition() { return m_pos - m_Info.startpos; }
	SObj* GetHold() { return m_pHold; }
	SObj* GetOldHold() { return m_pOldHold; }
	// メンバ関数(設定)
	void SetPosition(const D3DXVECTOR3& pos);
	TYPE GetType() { return m_type; }

protected:

	// メンバ関数
	virtual void Move(){}
	virtual void Scale(){}
	virtual void Rotation(){}
	SObj* GetHoldObj() { return m_pHold; }

	// メンバ変数
	D3DXVECTOR3 m_pos;	// 座標
	SInfo m_Info;
	SObj m_aObj[VEC_MAX];

private:	// 自分だけがアクセス可能

	// メンバ変数
	SObj* m_pHold;	// 選択
	SObj* m_pOldHold;
	TYPE m_type;
	void Select();
	void Release();
};

#endif

