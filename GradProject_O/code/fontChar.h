//============================================================
//
//	フォント文字ヘッダー [fontChar.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _FONT_CHAR_H_
#define _FONT_CHAR_H_
#include "convenience.h"
//************************************************************
//	クラス定義
//************************************************************
// フォント文字クラス
class CFontChar
{
public:
	// コンストラクタ
	CFontChar(const HFONT hFont, const std::string& rFilePath, const bool bItalic);

	// デストラクタ
	~CFontChar();

	// 矩形の座標情報構造体
	struct SRectangle
	{
		// デフォルトコンストラクタ
		SRectangle() :
			lu	(GRID2_ZERO),	// 左上座標
			rd	(GRID2_ZERO)	// 右下座標
		{}

		// デストラクタ
		~SRectangle() {}

		// メンバ変数
		POSGRID2 lu;	// 左上座標
		POSGRID2 rd;	// 右下座標
	};

	// フォント文字構造体
	struct SChar
	{
		// デフォルトコンストラクタ
		SChar() :
			glyph			({}),	// フォントグリフの情報
			outline			({}),	// フォントアウトラインの情報
			text			({}),	// フォントテキストの情報
			offsetBlackBox	({}),	// ブラックボックスの中心からのオフセット
			nTexIdx			(0),	// テクスチャインデックス
			bEmpty			(false)	// テクスチャ透明フラグ
		{}

		// デストラクタ
		~SChar() {}

		// メンバ変数
		GLYPHMETRICS glyph;			// フォントグリフの情報
		OUTLINETEXTMETRIC outline;	// フォントアウトラインの情報
		TEXTMETRIC text;			// フォントテキストの情報
		SRectangle offsetBlackBox;	// ブラックボックスの中心からのオフセット
		int nTexIdx;				// テクスチャインデックス
		bool bEmpty;				// テクスチャ透明フラグ
	};

	// メンバ関数
	HRESULT Init();	// フォント文字初期化
	void Uninit();	// フォント文字終了
	SChar Regist(const wchar_t wcChar);		// フォント文字登録
	inline const HFONT GetFont()			{ return m_hFont; }		// フォントへのポインタ
	inline const std::string GetFilePath()	{ return m_sFilePath; }	// フォントパス
	inline const bool GetItalic()			{ return m_bItalic; }	// イタリック

	// 静的メンバ関数
	static CFontChar* Create(const HFONT hFont, const std::string& rFilePath, const bool bItalic);	// 生成
	static void Release(CFontChar*& prFontChar);	// 破棄

private:
	// メンバ関数
	BYTE* CreateBitMap(SChar* pChar, HDC hDC, const wchar_t wcChar);	// ビットマップ生成・文字情報保存
	HRESULT CreateTexture(SChar* pChar, BYTE* pBitMap);					// テクスチャ生成・作成

	// メンバ変数
	std::map<wchar_t, SChar> m_mapChar;	// フォント文字連想配列
	const HFONT m_hFont;				// フォントへのポインタ
	const std::string m_sFilePath;		// フォントパス
	const bool m_bItalic;				// イタリック
};

#endif	// _FONT_CHAR_H_
