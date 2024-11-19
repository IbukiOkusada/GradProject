//============================================================
//
//	文字2D処理 [char2D.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "char2D.h"
#include "manager.h"
#include "texture.h"
#include "font.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const int PRIORITY = 6;	// 文字2Dの優先順位
}

//************************************************************
//	子クラス [CChar2D] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CChar2D::CChar2D() : CObject2D(CObject::LABEL_UI, CObject::DIM_2D, PRIORITY),
	m_pFontChar		(nullptr),	// フォント文字
	m_wcChar		(0),		// 指定文字
	m_fCharHeight	(0.0f),		// 文字の縦幅
	m_fSizeRate		(0.0f),		// 縦幅の割合
	m_fAbsOriginX	(0.0f),		// X原点オフセットの絶対値
	m_bTexEmpty		(false)		// テクスチャ透明フラグ
{

}

//============================================================
//	デストラクタ
//============================================================
CChar2D::~CChar2D()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CChar2D::Init()
{
	// メンバ変数を初期化
	m_pFontChar		= nullptr;	// フォント文字
	m_wcChar		= 0;		// 指定文字
	m_fCharHeight	= 0.0f;		// 文字の縦幅
	m_fSizeRate		= 1.0f;		// 縦幅の割合
	m_fAbsOriginX	= 0.0f;		// X原点オフセットの絶対値
	m_bTexEmpty		= false;	// テクスチャ透明フラグ

	// オブジェクト2Dの初期化
	if (FAILED(CObject2D::Init()))
	{ // 初期化に失敗した場合

		assert(false);
		return E_FAIL;
	}

	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CChar2D::Uninit()
{
	// オブジェクト2Dの終了
	CObject2D::Uninit();
}

//============================================================
//	更新処理
//============================================================
void CChar2D::Update(const float fDeltaTime)
{
	// オブジェクト2Dの更新
	CObject2D::Update(fDeltaTime);
}

//============================================================
//	描画処理
//============================================================
void CChar2D::Draw(CShader* pShader)
{
	// オブジェクト2Dの描画
	CObject2D::Draw(pShader);
}

//============================================================
//	大きさの設定処理
//============================================================
void CChar2D::SetVec3Size(const VECTOR3& /*rSize*/)
{
	/*
		こっちで大きさを自由に変えられると
		文字の比率がおかしくなるのと、
		縦幅の割合なども変更できないため
		大きさ変更は SetCharHeight を利用してね。
	*/

	assert(false);
}

//============================================================
//	生成処理 (マルチバイト文字)
//============================================================
CChar2D* CChar2D::Create
(
	const std::string& rFilePath,	// フォントパス
	const bool bItalic,				// イタリック
	const std::string& rChar,		// 指定文字
	const VECTOR3& rPos,			// 位置
	const float fHeight,			// 縦幅
	const VECTOR3& rRot,			// 向き
	const COLOR& rCol				// 色
)
{
	// 文字2Dの生成
	CChar2D* pChar2D = new CChar2D;
	if (pChar2D == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// 文字2Dの初期化
		if (FAILED(pChar2D->Init()))
		{ // 初期化に失敗した場合

			// 文字2Dの破棄
			SAFE_DELETE(pChar2D);
			return nullptr;
		}

		// フォントを設定
		pChar2D->SetFont(rFilePath, bItalic);

		// 文字を設定
		pChar2D->SetChar(rChar);

		// 位置を設定
		pChar2D->SetVec3Position(rPos);

		// 向きを設定
		pChar2D->SetVec3Rotation(rRot);

		// 文字縦幅を設定
		pChar2D->SetCharHeight(fHeight);

		// 色を設定
		pChar2D->SetColor(rCol);

		// 確保したアドレスを返す
		return pChar2D;
	}
}

//============================================================
//	生成処理 (ワイド文字)
//============================================================
CChar2D* CChar2D::Create
(
	const std::string& rFilePath,	// フォントパス
	const bool bItalic,				// イタリック
	const wchar_t wcChar,			// 指定文字
	const VECTOR3& rPos,			// 位置
	const float fHeight,			// 縦幅
	const VECTOR3& rRot,			// 向き
	const COLOR& rCol				// 色
)
{
	// 文字2Dの生成
	CChar2D* pChar2D = new CChar2D;
	if (pChar2D == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// 文字2Dの初期化
		if (FAILED(pChar2D->Init()))
		{ // 初期化に失敗した場合

			// 文字2Dの破棄
			SAFE_DELETE(pChar2D);
			return nullptr;
		}

		// フォントを設定
		pChar2D->SetFont(rFilePath, bItalic);

		// 文字を設定
		pChar2D->SetChar(wcChar);

		// 位置を設定
		pChar2D->SetVec3Position(rPos);

		// 向きを設定
		pChar2D->SetVec3Rotation(rRot);

		// 文字縦幅を設定
		pChar2D->SetCharHeight(fHeight);

		// 色を設定
		pChar2D->SetColor(rCol);

		// 確保したアドレスを返す
		return pChar2D;
	}
}

//============================================================
//	フォントの設定処理
//============================================================
void CChar2D::SetFont
(
	const std::string& rFilePath,	// フォントパス
	const bool bItalic				// イタリック
)
{
	// フォント文字情報を設定
	CFont* pFont = GET_MANAGER->GetFont();	// フォント情報
	m_pFontChar = pFont->Regist(rFilePath, bItalic).pFontChar;

	// 指定文字を再設定
	SetChar(m_wcChar);
}

//============================================================
//	文字の設定処理 (マルチバイト文字)
//============================================================
void CChar2D::SetChar(const std::string& rChar)
{
	// ワイド変換後の先頭文字を設定
	SetChar(useful::MultiByteToWide(rChar)[0]);
}

//============================================================
//	文字の設定処理 (ワイド文字)
//============================================================
void CChar2D::SetChar(const wchar_t wcChar)
{
	CFontChar::SChar infoChar = m_pFontChar->Regist(wcChar);	// 文字情報

	// 指定文字を保存
	m_wcChar = wcChar;

	// X原点オフセットの絶対値を保存
	m_fAbsOriginX = fabsf((float)infoChar.glyph.gmptGlyphOrigin.x);

	// テクスチャ透明フラグを保存
	m_bTexEmpty = infoChar.bEmpty;

	// テクスチャを登録・割当
	BindTexture(infoChar.nTexIdx);

	// 文字縦幅の再設定
	SetCharHeight(m_fCharHeight);
}

//============================================================
//	文字縦幅の設定処理
//============================================================
void CChar2D::SetCharHeight(const float fHeight)
{
	int nTexIdx = GetTextureIndex();	// フォントのテクスチャインデックス
	float fTexWidth = useful::GetTexWidthFromAspect(fHeight, nTexIdx);			// テクスチャ横幅
	D3DXIMAGE_INFO status = GET_MANAGER->GetTexture()->GetInfo(nTexIdx).status;	// テクスチャステータス

	// 文字の縦幅を保存
	m_fCharHeight = fHeight;

	// 縦幅の増減割合を保存
	m_fSizeRate = fHeight / (float)status.Height;

	// 大きさを設定
	CObject2D::SetVec3Size(VECTOR3(fTexWidth, fHeight, 0.0f));
}

//============================================================
//	ブラックボックスの左上オフセット取得処理
//============================================================
VECTOR2 CChar2D::GetOffsetBlackBoxLU()
{
	CFontChar::SChar infoChar = m_pFontChar->Regist(m_wcChar);	// 文字情報

	VECTOR2 tempOffset;	// float変換オフセット格納用
	tempOffset.x = (float)infoChar.offsetBlackBox.lu.x;
	tempOffset.y = (float)infoChar.offsetBlackBox.lu.y;

	// ブラックボックスの左上オフセットを返す
	return tempOffset * m_fSizeRate;
}

//============================================================
//	ブラックボックスの右下オフセット取得処理
//============================================================
VECTOR2 CChar2D::GetOffsetBlackBoxRD()
{
	CFontChar::SChar infoChar = m_pFontChar->Regist(m_wcChar);	// 文字情報

	VECTOR2 tempOffset;	// float変換オフセット格納用
	tempOffset.x = (float)infoChar.offsetBlackBox.rd.x;
	tempOffset.y = (float)infoChar.offsetBlackBox.rd.y;

	// ブラックボックスの右下オフセットを返す
	return tempOffset * m_fSizeRate;
}

//============================================================
//	文字の取得処理 (マルチバイト文字)
//============================================================
std::string CChar2D::GetChar() const
{
	std::wstring wsChar;	// 文字列変換用

	// 変換文字列に文字を割当
	wsChar = m_wcChar;

	// マルチバイト変換後の文字列を返す
	return useful::WideToMultiByte(wsChar);
}

//============================================================
//	原点のオフセット取得処理
//============================================================
float CChar2D::GetOffsetOrigin()
{
	CFontChar::SChar infoChar = m_pFontChar->Regist(m_wcChar);	// 文字情報

	// 文字ブラックボックスの横幅
	float fBlackBoxX = (float)infoChar.glyph.gmBlackBoxX;

	// 文字原点のオフセットを返す
	return (m_fAbsOriginX + fBlackBoxX * 0.5f) * m_fSizeRate;
}

//============================================================
//	次の文字までの距離取得処理
//============================================================
float CChar2D::GetNext()
{
	CFontChar::SChar infoChar = m_pFontChar->Regist(m_wcChar);	// 文字情報

	// 次の文字の距離を返す
	return (float)infoChar.glyph.gmCellIncX * m_fSizeRate;
}
