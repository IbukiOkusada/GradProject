//============================================================
//
//	文字送りテキスト2Dヘッダー [scrollText2D.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _SCROLL_TEXT2D_H_
#define _SCROLL_TEXT2D_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "text2D.h"
#include "sound.h"

//************************************************************
//	クラス定義
//************************************************************
// 文字送りテキスト2Dクラス
class CScrollText2D : public CText2D
{
public:
	// コンストラクタ
	CScrollText2D();

	// デストラクタ
	~CScrollText2D() override;

	// オーバーライド関数
	HRESULT Init() override;	// 初期化
	void Uninit() override;		// 終了
	void Update(const float fDeltaTime) override;	// 更新
	void Draw(CShader* pShader = nullptr) override;	// 描画
	void SetEnableDraw(const bool bDraw) override;	// 描画状況設定
	HRESULT PushFrontString(const std::string& rStr) override;	// 文字列の先頭追加 (マルチバイト文字列)
	HRESULT PushFrontString(const std::wstring& rStr) override;	// 文字列の先頭追加 (ワイド文字列)
	HRESULT PushBackString(const std::string& rStr) override;	// 文字列の最後尾追加 (マルチバイト文字列)
	HRESULT PushBackString(const std::wstring& rStr) override;	// 文字列の最後尾追加 (ワイド文字列)
	void DeleteString(const int nStrIdx) override;				// 文字列削除
	void DeleteStringAll() override;							// 文字列全削除

	// 静的メンバ関数
	static CScrollText2D* Create	// 生成
	( // 引数
		const std::string& rFilePath,			// フォントパス
		const bool bItalic,						// イタリック
		const VECTOR3& rPos,					// 原点位置
		const float fNextTime = 0.1f,			// 文字表示の待機時間
		const float fCharHeight = 100.0f,		// 文字縦幅
		const float fLineHeight = 100.0f,		// 行間縦幅
		const EAlignX alignX = XALIGN_CENTER,	// 横配置
		const EAlignY alignY = YALIGN_CENTER,	// 縦配置
		const VECTOR3& rRot = VEC3_ZERO,		// 原点向き
		const COLOR& rCol = color::White()		// 色
	);

	// メンバ関数
	inline void SetNextTime(const float fNextTime)			{ m_fNextTime = fNextTime; }		// 文字表示の待機時間設定
	inline float GetNextTime() const						{ return m_fNextTime; }				// 文字表示の待機時間取得
	inline void SetEnableScroll(const bool bScroll)			{ m_bScroll = bScroll; }			// 文字送り状況設定
	inline bool IsScroll() const							{ return m_bScroll; }				// 文字送り状況取得
	inline void SetScrollSE(const CSound::ELabel labelSE)	{ m_labelSE = labelSE; }			// 文字送り時の効果音設定
	inline CSound::ELabel GetScrollSE() const				{ return m_labelSE; }				// 文字送り時の効果音取得
	inline int GetNextCharIdx()								{ return m_nNextIdx; }				// 次の表示文字インデックス取得
	inline CChar2D* GetNextChar2D()							{ return m_vecChar[m_nNextIdx]; }	// 次の表示文字取得

private:
	// メンバ関数
	void UpdateScroll(const float fDeltaTime);	// 文字送り更新
	void PlayScrollSE(CChar2D* pChar2D);		// 文字送り効果音の再生

	// メンバ変数
	std::vector<CChar2D*> m_vecChar;	// 全文字情報
	CSound::ELabel m_labelSE;			// 文字送り再生SEラベル
	int m_nNextIdx;		// 次表示する文字インデックス
	float m_fNextTime;	// 次表示までの待機時間
	float m_fCurTime;	// 現在の待機時間
	bool m_bScroll;		// 文字送り状況
};

#endif	// _SCROLL_TEXT2D_H_
