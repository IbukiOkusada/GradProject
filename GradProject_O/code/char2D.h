//============================================================
//
//	文字2Dヘッダー [char2D.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _CHAR2D_H_
#define _CHAR2D_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "object2D.h"
#include "fontChar.h"

//************************************************************
//	クラス定義
//************************************************************
// 文字2Dクラス
class CChar2D : public CObject2D
{
public:
	// コンストラクタ
	CChar2D();

	// デストラクタ
	~CChar2D() override;

	// オーバーライド関数
	HRESULT Init() override;	// 初期化
	void Uninit() override;		// 終了
	void Update(const float fDeltaTime) override;		// 更新
	void Draw(CShader* pShader = nullptr) override;		// 描画
	void SetVec3Size(const VECTOR3& rSize) override;	// 大きさ設定

	// 静的メンバ関数
	static CChar2D* Create	// 生成 (マルチバイト文字)
	( // 引数
		const std::string& rFilePath,		// フォントパス
		const bool bItalic,					// イタリック
		const std::string& rChar,			// 指定文字
		const VECTOR3& rPos = VEC3_ZERO,	// 位置
		const float fHeight = 100.0f,		// 縦幅
		const VECTOR3& rRot = VEC3_ZERO,	// 向き
		const COLOR& rCol = color::White()	// 色
	);
	static CChar2D* Create	// 生成 (ワイド文字)
	( // 引数
		const std::string& rFilePath,		// フォントパス
		const bool bItalic,					// イタリック
		const wchar_t wcChar,				// 指定文字
		const VECTOR3& rPos = VEC3_ZERO,	// 位置
		const float fHeight = 100.0f,		// 縦幅
		const VECTOR3& rRot = VEC3_ZERO,	// 向き
		const COLOR& rCol = color::White()	// 色
	);

	// メンバ関数
	void SetFont	// フォントの設定
	( // 引数
		const std::string& rFilePath,	// フォントパス
		const bool bItalic = false		// イタリック
	);
	void SetChar(const std::string& rChar);		// 文字の設定 (マルチバイト文字)
	void SetChar(const wchar_t wcChar);			// 文字の設定 (ワイド文字)
	void SetCharHeight(const float fHeight);	// 文字の縦幅設定
	VECTOR2 GetOffsetBlackBoxLU();				// ブラックボックスの左上オフセット取得
	VECTOR2 GetOffsetBlackBoxRD();				// ブラックボックスの右下オフセット取得

	inline bool IsTexEmpty() const		{ return m_bTexEmpty; }	// テクスチャ透明フラグ取得
	inline wchar_t GetWideChar() const	{ return m_wcChar; }	// 文字取得 (ワイド文字)
	std::string GetChar() const;	// 文字取得 (マルチバイト文字)
	float GetOffsetOrigin();		// 原点のオフセット取得
	float GetNext();				// 次の文字までの距離取得

private:
	// メンバ変数
	CFontChar* m_pFontChar;	// フォント文字
	wchar_t m_wcChar;		// 指定文字
	float m_fCharHeight;	// 文字の縦幅
	float m_fSizeRate;		// 縦幅の割合
	float m_fAbsOriginX;	// X原点オフセットの絶対値
	bool m_bTexEmpty;		// テクスチャ透明フラグ
};

#endif	// _CHAR2D_H_
