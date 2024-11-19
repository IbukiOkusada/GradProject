//============================================================
//
//	テキスト2Dヘッダー [text2D.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _TEXT2D_H_
#define _TEXT2D_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "object.h"
#include "string2D.h"

//************************************************************
//	インクルードファイル
//************************************************************
class CFontChar;	// フォント文字クラス

//************************************************************
//	クラス定義
//************************************************************
// テキスト2Dクラス
class CText2D : public CObject
{
public:
	// コンストラクタ
	CText2D();

	// デストラクタ
	~CText2D() override;

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
	static CText2D* Create	// 生成
	( // 引数
		const std::string& rFilePath,			// フォントパス
		const bool bItalic,						// イタリック
		const VECTOR3& rPos,					// 原点位置
		const float fCharHeight = 100.0f,		// 文字縦幅
		const float fLineHeight = 100.0f,		// 行間縦幅
		const EAlignX alignX = XALIGN_CENTER,	// 横配置
		const EAlignY alignY = YALIGN_CENTER,	// 縦配置
		const VECTOR3& rRot = VEC3_ZERO,		// 原点向き
		const COLOR& rCol = color::White()		// 色
	);

	// 仮想関数
	virtual HRESULT PushFrontString(const std::string& rStr);	// 文字列の先頭追加 (マルチバイト文字列)
	virtual HRESULT PushFrontString(const std::wstring& rStr);	// 文字列の先頭追加 (ワイド文字列)
	virtual HRESULT PushBackString(const std::string& rStr);	// 文字列の最後尾追加 (マルチバイト文字列)
	virtual HRESULT PushBackString(const std::wstring& rStr);	// 文字列の最後尾追加 (ワイド文字列)
	virtual void DeleteString(const int nStrIdx);	// 文字列削除
	virtual void DeleteStringAll();					// 文字列全削除

	// メンバ関数
	void SetFont	// フォントの設定
	( // 引数
		const std::string& rFilePath,	// フォントパス
		const bool bItalic = false		// イタリック
	);
	void SetAlpha(const float fAlpha);					// 透明度設定
	void SetColor(const COLOR& rCol);					// 色設定
	void SetCharHeight(const float fHeight);			// 文字の縦幅設定
	void SetLineHeight(const float fHeight);			// 行間の縦幅設定
	void SetAlignX(const EAlignX align);				// 横配置設定
	void SetAlignY(const EAlignY align);				// 縦配置設定
	float GetTextHeight() const;						// テキストの縦幅取得
	CString2D* GetString2D(const int nStrIdx) const;	// 文字列の取得
	inline float GetAlpha() const		{ return m_col.a; }			// 透明度取得
	inline COLOR GetColor() const		{ return m_col; }			// 色取得
	inline float GetCharHeight() const	{ return m_fCharHeight; }	// 文字の縦幅取得
	inline float GetLineHeight() const	{ return m_fLineHeight; }	// 行間の縦幅取得
	inline EAlignX GetAlignX() const	{ return m_alignX; }		// 横配置取得
	inline EAlignY GetAlignY() const	{ return m_alignY; }		// 縦配置取得
	inline int GetNumString() const		{ return (int)m_listString.size(); }	// 文字列数取得

private:
	// オーバーライド関数
	inline void Release() override { CObject::Release(); }	// 破棄

	// メンバ関数
	CString2D* CreateString2D(const std::wstring& rStr);	// 文字列の生成
	void SetPositionRelative();	// 相対位置設定

	// メンバ変数
	std::list<CString2D*> m_listString;	// 文字列リスト
	CFontChar* m_pFontChar;	// フォント文字
	VECTOR3 m_pos;			// 位置
	VECTOR3 m_rot;			// 向き
	COLOR m_col;			// 色
	EAlignX m_alignX;		// 横配置
	EAlignY m_alignY;		// 縦配置
	float m_fCharHeight;	// 文字の縦幅
	float m_fLineHeight;	// 文字の行間
};

#endif	// _TEXT2D_H_
