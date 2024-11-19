//============================================================
//
//	テキスト2D処理 [text2D.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "text2D.h"
#include "manager.h"
#include "font.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const int PRIORITY = 6;	// テキスト2Dの優先順位
}

//************************************************************
//	子クラス [CText2D] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CText2D::CText2D() : CObject(CObject::LABEL_UI, CObject::DIM_2D, PRIORITY),
	m_pFontChar		(nullptr),			// フォント文字
	m_pos			(VEC3_ZERO),		// 位置
	m_rot			(VEC3_ZERO),		// 向き
	m_col			(color::White()),	// 色
	m_alignX		(XALIGN_CENTER),	// 横配置
	m_alignY		(YALIGN_CENTER),	// 縦配置
	m_fCharHeight	(0.0f),				// 文字の縦幅
	m_fLineHeight	(0.0f)				// 文字の行間
{
	// 文字列リストをクリア
	m_listString.clear();
}

//============================================================
//	デストラクタ
//============================================================
CText2D::~CText2D()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CText2D::Init()
{
	// メンバ変数を初期化
	m_pFontChar		= nullptr;			// フォント文字
	m_pos			= VEC3_ZERO;		// 位置
	m_rot			= VEC3_ZERO;		// 向き
	m_col			= color::White();	// 色
	m_alignX		= XALIGN_CENTER;	// 横配置
	m_alignY		= YALIGN_CENTER;	// 縦配置
	m_fCharHeight	= 0.0f;				// 文字の縦幅
	m_fLineHeight	= 0.0f;				// 文字の行間

	// 文字列リストを初期化
	m_listString.clear();

	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CText2D::Uninit()
{
	for (auto& rList : m_listString)
	{ // 文字列の格納数分繰り返す

		// 文字列の終了
		SAFE_UNINIT(rList);
	}

	// 文字列リストをクリア
	m_listString.clear();

	// オブジェクトを破棄
	Release();
}

//============================================================
//	更新処理
//============================================================
void CText2D::Update(const float fDeltaTime)
{
	// 相対位置の設定
	SetPositionRelative();
}

//============================================================
//	描画処理
//============================================================
void CText2D::Draw(CShader* /*pShader*/)
{

}

//============================================================
//	優先順位の設定処理
//============================================================
void CText2D::SetPriority(const int nPriority)
{
	// 自身の優先順位を設定
	CObject::SetPriority(nPriority);

	for (auto& rList : m_listString)
	{ // 文字列の格納数分繰り返す

		// 文字列の優先順位を設定
		rList->SetPriority(nPriority);
	}
}

//============================================================
//	更新状況の設定処理
//============================================================
void CText2D::SetEnableUpdate(const bool bUpdate)
{
	for (auto& rList : m_listString)
	{ // 文字列の格納数分繰り返す

		// 文字列の更新状況を設定
		rList->SetEnableUpdate(bUpdate);
	}
}

//============================================================
//	描画状況の設定処理
//============================================================
void CText2D::SetEnableDraw(const bool bDraw)
{
	for (auto& rList : m_listString)
	{ // 文字列の格納数分繰り返す

		// 文字列の描画状況を設定
		rList->SetEnableDraw(bDraw);
	}
}

//============================================================
//	位置の設定処理
//============================================================
void CText2D::SetVec3Position(const VECTOR3& rPos)
{
	// 引数の位置を設定
	m_pos = rPos;

	// 相対位置の設定
	SetPositionRelative();
}

//============================================================
//	向きの設定処理
//============================================================
void CText2D::SetVec3Rotation(const VECTOR3& rRot)
{
	// 設定する向きを保存
	m_rot = rRot;

	for (auto& rList : m_listString)
	{ // 文字列の格納数分繰り返す

		// 文字列向きの設定
		assert(rList != nullptr);
		rList->SetVec3Rotation(rRot);
	}

	// 相対位置の設定
	SetPositionRelative();
}

//============================================================
//	生成処理
//============================================================
CText2D* CText2D::Create
(
	const std::string& rFilePath,	// フォントパス
	const bool bItalic,				// イタリック
	const VECTOR3& rPos,			// 原点位置
	const float fCharHeight,		// 文字縦幅
	const float fLineHeight,		// 行間縦幅
	const EAlignX alignX,			// 横配置
	const EAlignY alignY,			// 縦配置
	const VECTOR3& rRot,			// 原点向き
	const COLOR& rCol				// 色
)
{
	// テキスト2Dの生成
	CText2D* pText2D = new CText2D;
	if (pText2D == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// テキスト2Dの初期化
		if (FAILED(pText2D->Init()))
		{ // 初期化に失敗した場合

			// テキスト2Dの破棄
			SAFE_DELETE(pText2D);
			return nullptr;
		}

		// フォントを設定
		pText2D->SetFont(rFilePath, bItalic);

		// 原点位置を設定
		pText2D->SetVec3Position(rPos);

		// 原点向きを設定
		pText2D->SetVec3Rotation(rRot);

		// 色を設定
		pText2D->SetColor(rCol);

		// 文字縦幅を設定
		pText2D->SetCharHeight(fCharHeight);

		// 行間縦幅を設定
		pText2D->SetLineHeight(fLineHeight);

		// 横配置を設定
		pText2D->SetAlignX(alignX);

		// 縦配置を設定
		pText2D->SetAlignY(alignY);

		// 確保したアドレスを返す
		return pText2D;
	}
}

//============================================================
//	文字列の先頭追加処理 (マルチバイト文字列)
//============================================================
HRESULT CText2D::PushFrontString(const std::string& rStr)
{
	// 文字列をワイド変換
	std::wstring wsStr = useful::MultiByteToWide(rStr);

	// 文字列を先頭に追加
	if (FAILED(PushFrontString(wsStr)))
	{ // 追加に失敗した場合

		return E_FAIL;
	}

	return S_OK;
}

//============================================================
//	文字列の先頭追加処理 (ワイド文字列)
//============================================================
HRESULT CText2D::PushFrontString(const std::wstring& rStr)
{
	// 空の文字列の場合抜ける
	if (rStr.empty()) { return E_FAIL; }

	// 文字列の生成
	CString2D* pStr = CreateString2D(rStr);
	if (pStr == nullptr)
	{ // 生成に失敗した場合

		assert(false);
		return E_FAIL;
	}

	// 先頭に生成した文字列を追加
	m_listString.push_front(pStr);

	// 相対位置の設定
	SetPositionRelative();

	return S_OK;
}

//============================================================
//	文字列の最後尾追加処理 (マルチバイト文字列)
//============================================================
HRESULT CText2D::PushBackString(const std::string& rStr)
{
	// 文字列をワイド変換
	std::wstring wsStr = useful::MultiByteToWide(rStr);

	// 文字列を最後尾に追加
	if (FAILED(PushBackString(wsStr)))
	{ // 追加に失敗した場合

		return E_FAIL;
	}

	return S_OK;
}

//============================================================
//	文字列の最後尾追加処理 (ワイド文字列)
//============================================================
HRESULT CText2D::PushBackString(const std::wstring& rStr)
{
	// 空の文字列の場合抜ける
	if (rStr.empty()) { return E_FAIL; }

	// 文字列の生成
	CString2D* pStr = CreateString2D(rStr);
	if (pStr == nullptr)
	{ // 生成に失敗した場合

		assert(false);
		return E_FAIL;
	}

	// 最後尾に生成した文字列を追加
	m_listString.push_back(pStr);

	// 相対位置の設定
	SetPositionRelative();

	return S_OK;
}

//============================================================
//	文字列の削除処理
//============================================================
void CText2D::DeleteString(const int nStrIdx)
{
	// 文字列がない場合抜ける
	int nStrSize = (int)m_listString.size();
	if (nStrSize <= 0) { assert(false); return; }

	// インデックスが範囲外の場合抜ける
	if (nStrIdx <= NONE_IDX || nStrIdx >= nStrSize) { assert(false); return; }

	// イテレーターをインデックス分ずらす
	auto itr = m_listString.begin();
	std::advance(itr, nStrIdx);

	// イテレーター内の文字列2Dを終了
	SAFE_UNINIT(*itr);

	// イテレーターをリストから削除
	m_listString.erase(itr);
}

//============================================================
//	文字列の全削除処理
//============================================================
void CText2D::DeleteStringAll()
{
	for (auto& rList : m_listString)
	{ // 文字列の格納数分繰り返す

		// 文字列の終了
		assert(rList != nullptr);
		SAFE_UNINIT(rList);
	}

	// 文字列リストをクリア
	m_listString.clear();
}

//============================================================
//	フォントの設定処理
//============================================================
void CText2D::SetFont
(
	const std::string& rFilePath,	// フォントパス
	const bool bItalic				// イタリック
)
{
	// フォント文字情報を設定
	CFont* pFont = GET_MANAGER->GetFont();	// フォント情報
	m_pFontChar = pFont->Regist(rFilePath, bItalic).pFontChar;

	for (auto& rList : m_listString)
	{ // 文字列の格納数分繰り返す

		// 文字列フォントの設定
		assert(rList != nullptr);
		rList->SetFont(rFilePath, bItalic);
	}

	// 相対位置の設定
	SetPositionRelative();
}

//============================================================
//	透明度の設定処理
//============================================================
void CText2D::SetAlpha(const float fAlpha)
{
	// 引数の透明度を保存
	m_col.a = fAlpha;

	// 色の設定
	SetColor(m_col);
}

//============================================================
//	色の設定処理
//============================================================
void CText2D::SetColor(const COLOR& rCol)
{
	// 設定する色を保存
	m_col = rCol;

	for (auto& rList : m_listString)
	{ // 文字列の格納数分繰り返す

		// 文字列色の設定
		assert(rList != nullptr);
		rList->SetColor(rCol);
	}
}

//============================================================
//	文字縦幅の設定処理
//============================================================
void CText2D::SetCharHeight(const float fHeight)
{
	// 設定する文字の縦幅を保存
	m_fCharHeight = fHeight;

	for (auto& rList : m_listString)
	{ // 文字列の格納数分繰り返す

		// 文字列縦幅の設定
		assert(rList != nullptr);
		rList->SetCharHeight(fHeight);
	}

	// 相対位置の設定
	SetPositionRelative();
}

//============================================================
//	行間縦幅の設定処理
//============================================================
void CText2D::SetLineHeight(const float fHeight)
{
	// 引数の行間を設定
	m_fLineHeight = fHeight;

	// 相対位置の設定
	SetPositionRelative();
}

//============================================================
//	横配置の設定処理
//============================================================
void CText2D::SetAlignX(const EAlignX align)
{
	// 引数の横配置を設定
	m_alignX = align;

	for (auto& rList : m_listString)
	{ // 文字列の格納数分繰り返す

		// 文字列横幅の設定
		assert(rList != nullptr);
		rList->SetAlignX(align);
	}

	// 相対位置の設定
	SetPositionRelative();
}

//============================================================
//	縦配置の設定処理
//============================================================
void CText2D::SetAlignY(const EAlignY align)
{
	// 引数の縦配置を設定
	m_alignY = align;

	// 相対位置の設定
	SetPositionRelative();
}

//============================================================
//	文字列の取得処理
//============================================================
CString2D* CText2D::GetString2D(const int nStrIdx) const
{
	// 文字列がない場合抜ける
	int nStrSize = (int)m_listString.size();
	if (nStrSize <= 0) { assert(false); return nullptr; }

	// インデックスが範囲外の場合抜ける
	if (nStrIdx <= NONE_IDX || nStrIdx >= nStrSize) { assert(false); return nullptr; }

	// 引数インデックスの文字列を返す
	auto itr = m_listString.begin();	// 先頭イテレーター
	std::advance(itr, nStrIdx);			// イテレーターを進める
	return *itr;						// イテレーターの中身を返す
}

//============================================================
//	テキストの縦幅取得処理
//============================================================
float CText2D::GetTextHeight() const
{
	// 文字列がない場合抜ける
	if ((int)m_listString.size() <= 0) { assert(false); return 0.0f; }

	float fTextHeight = 0.0f;	// テキストの縦幅
	int nEndStrIdx = (int)m_listString.size() - 1;	// 終端文字列のインデックス
	for (int i = 0; i < nEndStrIdx; i++)
	{ // 終端文字列を抜いた文字列数分繰り返す

		// 次の文字列までの行間を加算
		fTextHeight += m_fLineHeight;
	}

	// 先頭と終端の文字列の無視されたサイズを加算
	fTextHeight += m_listString.front()->GetCharHeight() * 0.5f;	// 先頭文字列の原点上サイズ
	fTextHeight += m_listString.back()->GetCharHeight() * 0.5f;		// 終端文字列の原点下サイズ

	// テキストの縦幅を返す
	return fTextHeight;
}

//============================================================
//	文字列の生成処理
//============================================================
CString2D* CText2D::CreateString2D(const std::wstring& rStr)
{
	// 文字列オブジェクトを生成
	CString2D* pStr = CString2D::Create
	( // 引数
		m_pFontChar->GetFilePath(),	// フォントパス
		m_pFontChar->GetItalic(),	// イタリック
		rStr,			// 指定文字列
		m_pos,			// 原点位置
		m_fCharHeight,	// 文字縦幅
		m_alignX,		// 横配置
		m_rot,			// 原点向き
		m_col			// 色
	);
	if (pStr != nullptr)
	{ // 生成に成功した場合

		// 文字列のラベルを指定なしにする
		pStr->SetLabel(LABEL_NONE);

		// 文字列の優先順位を自身のものにする
		pStr->SetPriority(GetPriority());
	}

	// 文字列クラスを返す
	return pStr;
}

//============================================================
//	相対位置の設定処理
//============================================================
void CText2D::SetPositionRelative()
{
	// 文字列がない場合抜ける
	if ((int)m_listString.size() <= 0) { return; }

	float fTextHeight	= GetTextHeight() * 0.5f;	// テキスト全体の縦幅
	float fFrontHeight	= m_listString.front()->GetCharHeight() * 0.5f;	// 先頭文字列の縦幅
	float fStartOffset	= -fTextHeight + fFrontHeight - (fTextHeight * (m_alignY - 1));	// 文字列の開始位置オフセット

	VECTOR3 posStart = VEC3_ZERO;	// 文字の開始位置
	posStart.x = m_pos.x + sinf(m_rot.z) * fStartOffset;	// 開始位置X
	posStart.y = m_pos.y + cosf(m_rot.z) * fStartOffset;	// 開始位置Y

	for (auto& rList : m_listString)
	{ // 文字列の格納数分繰り返す

		// 位置を反映
		assert(rList != nullptr);
		rList->SetVec3Position(posStart);

		// 次の設定座標の開始点を保存
		posStart.x += sinf(m_rot.z) * m_fLineHeight;
		posStart.y += cosf(m_rot.z) * m_fLineHeight;
	}
}
