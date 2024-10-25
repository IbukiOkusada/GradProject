//==========================================================
//
// エディット矢印 [edit_arrow.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _EDIT_ARROW_H_		// このマクロが定義されていない場合
#define _EDIT_ARROW_H_		// 二重インクルード防止用マクロを定義

class CObjectX;

//==========================================================
// サンプルのクラス定義
//==========================================================
class CEdit_Arrow
{
public:

	enum TYPE
	{
		TYPE_ALL = 0,	// 全部動く
		TYPE_X,			// X座標
		TYPE_Y,			// Y座標
		TYPE_Z,			// Z座標
		TYPE_MAX
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
		TYPE type;
	};

public:	// 誰でもアクセス可能

	CEdit_Arrow();	// コンストラクタ(オーバーロード)
	~CEdit_Arrow();	// デストラクタ

	// メンバ関数
	virtual HRESULT Init(void);
	virtual void Uninit(void);
	virtual void Update(void);
	static CEdit_Arrow* Create(const D3DXVECTOR3& pos);

	// メンバ関数(取得)
	D3DXVECTOR3& GetPosition() { return m_pos; }
	SObj* GetHold() { return m_pHold; }
	// メンバ関数(設定)
	void SetPosition(const D3DXVECTOR3& pos);

private:	// 自分だけがアクセス可能

	// メンバ関数
	void Move();
	void Select();
	void Release();

	// メンバ変数
	SObj m_aObj[TYPE_MAX];
	D3DXVECTOR3 m_pos;	// 座標
	SInfo m_Info;
	SObj* m_pHold;	// 選択
};

#endif

