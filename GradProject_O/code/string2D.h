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
	void Update(const float fDeltaTime) override;		// 更新
	void Draw(CShader* pShader = nullptr) override;		// 描画
	void SetPriority(const int nPriority) override;		// 優先順位設定
	void SetEnableUpdate(const bool bUpdate) override;	// 更新状況設定
	void SetEnableDraw(const bool bDraw) override;		// 描画状況設定
	void SetVec3Position(const VECTOR3& rPos) override;	// 位置設定
	void SetVec3Rotation(const VECTOR3& rRot) override;	// 向き設定
	inline VECTOR3 GetVec3Position() const override { return m_pos; }	// 位置取得
	inline VECTOR3 GetVec3Rotation() const override { return m_rot; }	// 向き取得

	// 静的メンバ関数
	static CString2D* Create	// 生成 (マルチバイト文字列)
	( // 引数
		const std::string& rFilePath,	// フォントパス
		const bool bItalic,				// イタリック
		const std::string& rStr,		// 指定文字列
		const VECTOR3& rPos,			// 原点位置
		const float fHeight = 100.0f,			// 文字縦幅
		const EAlignX alignX = XALIGN_CENTER,	// 横配置
		const VECTOR3& rRot = VEC3_ZERO,		// 原点向き
		const COLOR& rCol = color::White()		// 色
	);
	static CString2D* Create	// 生成 (ワイド文字列)
	( // 引数
		const std::string& rFilePath,	// フォントパス
		const bool bItalic,				// イタリック
		const std::wstring& rStr,		// 指定文字列
		const VECTOR3& rPos,			// 原点位置
		const float fHeight = 100.0f,			// 文字縦幅
		const EAlignX alignX = XALIGN_CENTER,	// 横配置
		const VECTOR3& rRot = VEC3_ZERO,		// 原点向き
		const COLOR& rCol = color::White()		// 色
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
	void SetColor(const COLOR& rCol);				// 色設定
	void SetCharHeight(const float fHeight);		// 文字の縦幅設定
	void SetAlignX(const EAlignX align);			// 横配置設定
	float GetStrWidth() const;						// 文字列の横幅取得
	CChar2D* GetChar2D(const int nCharIdx) const;	// 文字の取得
	std::string GetStr() const;						// 文字列取得 (マルチバイト文字列)
	inline std::wstring GetWideStr() const	{ return m_wsStr; }				// 文字列取得 (ワイド文字列)
	inline float GetAlpha() const			{ return m_col.a; }				// 透明度取得
	inline COLOR GetColor() const			{ return m_col; }				// 色取得
	inline float GetCharHeight() const		{ return m_fCharHeight; }		// 文字の縦幅取得
	inline EAlignX GetAlignX() const		{ return m_alignX; }			// 横配置取得
	inline int GetNumChar() const			{ return (int)m_wsStr.size(); }	// 文字数取得

private:
	// オーバーライド関数
	inline void Release() override { CObject::Release(); }	// 破棄

	// メンバ関数
	void SetPositionRelative();	// 相対位置設定

	// メンバ変数
	CChar2D** m_ppChar;		// 文字ポリゴンの情報
	CFontChar* m_pFontChar;	// フォント文字
	std::wstring m_wsStr;	// 指定文字列
	VECTOR3 m_pos;			// 位置
	VECTOR3 m_rot;			// 向き
	COLOR m_col;			// 色
	EAlignX m_alignX;		// 横配置
	float m_fCharHeight;	// 文字の縦幅
};

#endif	// _STRING2D_H_
