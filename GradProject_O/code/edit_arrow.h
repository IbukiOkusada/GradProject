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

	// メンバ関数(設定)
	void SetPosition(const D3DXVECTOR3& pos);

private:	// 自分だけがアクセス可能

	// メンバ関数

	// メンバ変数
	CObjectX* m_apObj[TYPE_MAX];
	D3DXVECTOR3 m_pos;	// 座標
};

#endif

