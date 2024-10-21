//==========================================================
//
// エディット基底 [edit.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _EDIT_H_		// このマクロが定義されていない場合
#define _EDIT_H_		// 二重インクルード防止用マクロを定義

//==========================================================
// サンプルのクラス定義
//==========================================================
class CEdit
{
public:	// 誰でもアクセス可能な定義

	enum TYPE
	{
		TYPE_OBJ = 0,	// 障害物
		TYPE_ROAD,		// 道
		TYPE_MAX
	};

public:	// 誰でもアクセス可能

	CEdit();	// コンストラクタ(オーバーロード)
	~CEdit();	// デストラクタ

	// メンバ関数
	virtual HRESULT Init(void) = 0;
	virtual void Uninit(void);
	virtual void Update(void) = 0;
	static CEdit* Create(const TYPE& type);

	// メンバ関数(取得)
	TYPE GetType() { return m_type; }

	// メンバ関数(設定)

protected:

	void SetType(const TYPE& type) { m_type = type; }

private:	// 自分だけがアクセス可能

	// メンバ関数

	// メンバ変数
	TYPE m_type;
};

#endif
