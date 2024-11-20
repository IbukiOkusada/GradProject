//============================================================
//
//	文字列2Dヘッダー [string2D.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _STRING2D_H_
#define _STRING2D_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "object.h"

//************************************************************
//	前方宣言
//************************************************************
class CFontChar;	// フォント文字クラス
class CChar2D;		// 文字2Dクラス

//************************************************************
//	クラス定義
//************************************************************
// 文字列2Dクラス
class CString2D : public CObject
{
public:
	// コンストラクタ
	CString2D();

	// デストラクタ
	~CString2D() override;

	// オーバーライド関数
	HRESULT Init() override;	// 初期化
	void Uninit() override;		// 終了
	void Update() ;		// 更新
	void Draw() ;		// 描画

	void SetEnableDraw(const bool bDraw) ;		// 描画状況設定
	void SetVec3Position(const D3DXVECTOR3& rPos) ;	// 位置設定
	void SetVec3Rotation(const D3DXVECTOR3& rRot) ;	// 向き設定
	inline D3DXVECTOR3 GetVec3Position() const  { return m_pos; }	// 位置取得
	inline D3DXVECTOR3 GetVec3Rotation() const  { return m_rot; }	// 向き取得

	// 静的メンバ関数
	static CString2D* Create	// 生成 (マルチバイト文字列)
	( // 引数
		const std::string& rFilePath,	// フォントパス
		const bool bItalic,				// イタリック
		const std::string& rStr,		// 指定文字列
		const D3DXVECTOR3& rPos,			// 原点位置
		const float fHeight = 100.0f,			// 文字縦幅
		const EAlignX alignX = XALIGN_CENTER,	// 横配置
		const D3DXVECTOR3& rRot = VECTOR3_ZERO,		// 原点向き
		const D3DXCOLOR& rCol = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f)		// 色
	);
	static CString2D* Create	// 生成 (ワイド文字列)
	( // 引数
		const std::string& rFilePath,	// フォントパス
		const bool bItalic,				// イタリック
		const std::wstring& rStr,		// 指定文字列
		const D3DXVECTOR3& rPos,			// 原点位置
		const float fHeight = 100.0f,			// 文字縦幅
		const EAlignX alignX = XALIGN_CENTER,	// 横配置
		const D3DXVECTOR3& rRot = VECTOR3_ZERO,		// 原点向き
		const D3DXCOLOR& rCol = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f)			// 色
	);

	// 仮想関数
	virtual HRESULT SetString(const std::string& rStr);		// 文字列の設定 (マルチバイト文字列)
	virtual HRESULT SetString(const std::wstring& rStr);	// 文字列の設定 (ワイド文字列)

	// メンバ関数
	void SetFont	// フォントの設定
	( // 引数
		const std::string& rFilePath,	// フォントパス
		const bool bItalic = false		// イタリック
	);
	void SetAlpha(const float fAlpha);				// 透明度設定
	void SetColor(const D3DXCOLOR& rCol);				// 色設定
	void SetCharHeight(const float fHeight);		// 文字の縦幅設定
	void SetAlignX(const EAlignX align);			// 横配置設定
	float GetStrWidth() const;						// 文字列の横幅取得
	CChar2D* GetChar2D(const int nCharIdx) const;	// 文字の取得
	std::string GetStr() const;						// 文字列取得 (マルチバイト文字列)
	inline std::wstring GetWideStr() const	{ return m_wsStr; }				// 文字列取得 (ワイド文字列)
	inline float GetAlpha() const			{ return m_col.a; }				// 透明度取得
	inline D3DXCOLOR GetColor() const			{ return m_col; }				// 色取得
	inline float GetCharHeight() const		{ return m_fCharHeight; }		// 文字の縦幅取得
	inline EAlignX GetAlignX() const		{ return m_alignX; }			// 横配置取得
	inline int GetNumChar() const			{ return (int)m_wsStr.size(); }	// 文字数取得

private:
	// オーバーライド関数
	inline void Release()  { CObject::Release(); }	// 破棄

	// メンバ関数
	void SetPositionRelative();	// 相対位置設定

	// メンバ変数
	CChar2D** m_ppChar;		// 文字ポリゴンの情報
	CFontChar* m_pFontChar;	// フォント文字
	std::wstring m_wsStr;	// 指定文字列
	D3DXVECTOR3 m_pos;			// 位置
	D3DXVECTOR3 m_rot;			// 向き
	D3DXCOLOR m_col;			// 色
	EAlignX m_alignX;		// 横配置
	float m_fCharHeight;	// 文字の縦幅
};

#endif	// _STRING2D_H_
