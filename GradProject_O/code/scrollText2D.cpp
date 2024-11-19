//============================================================
//
//	文字送りテキスト2D処理 [scrollText2D.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "scrollText2D.h"
#include "manager.h"
#include "string2D.h"
#include "char2D.h"

//************************************************************
//	子クラス [CScrollText2D] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CScrollText2D::CScrollText2D() :
	m_labelSE	(CSound::LABEL_NONE),	// 文字送り再生SEラベル
	m_nNextIdx	(0),	// 次表示する文字インデックス
	m_fNextTime	(0.0f),	// 次表示するまでの時間
	m_fCurTime	(0.0f),	// 現在の待機時間
	m_bScroll	(false)	// 文字送り状況
{
	// 全文字情報配列をクリア
	m_vecChar.clear();
}

//============================================================
//	デストラクタ
//============================================================
CScrollText2D::~CScrollText2D()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CScrollText2D::Init()
{
	// メンバ変数を初期化
	m_labelSE	= CSound::LABEL_NONE;	// 文字送り再生SEラベル
	m_nNextIdx	= 0;		// 次表示する文字インデックス
	m_fNextTime	= 0.0f;		// 次表示するまでの時間
	m_fCurTime	= 0.0f;		// 現在の待機時間
	m_bScroll	= false;	// 文字送り状況

	// 全文字情報配列を初期化
	m_vecChar.clear();

	// テキスト2Dの初期化
	if (FAILED(CText2D::Init()))
	{ // 初期化に失敗した場合

		assert(false);
		return E_FAIL;
	}

	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CScrollText2D::Uninit()
{
	// 全文字情報配列をクリア
	m_vecChar.clear();

	// テキスト2Dの終了
	CText2D::Uninit();
}

//============================================================
//	更新処理
//============================================================
void CScrollText2D::Update(const float fDeltaTime)
{
	// 文字送りの更新
	UpdateScroll(fDeltaTime);

	// テキスト2Dの更新
	CText2D::Update(fDeltaTime);
}

//============================================================
//	描画処理
//============================================================
void CScrollText2D::Draw(CShader* pShader)
{
	// テキスト2Dの描画
	CText2D::Draw(pShader);
}

//============================================================
//	描画状況の設定処理
//============================================================
void CScrollText2D::SetEnableDraw(const bool bDraw)
{
	// 次表示する文字インデックスを描画設定に応じて反映
	m_nNextIdx = (bDraw) ? (int)m_vecChar.size() - 1 : 0;	// ONなら最後尾、OFFなら先頭

	// 現在の待機時間を初期化
	m_fCurTime = 0.0f;

	// 描画状況の設定
	CText2D::SetEnableDraw(bDraw);
}

//============================================================
//	文字列の先頭追加処理 (マルチバイト文字列)
//============================================================
HRESULT CScrollText2D::PushFrontString(const std::string& rStr)
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
HRESULT CScrollText2D::PushFrontString(const std::wstring& rStr)
{
	// 文字列を先頭に追加
	if (FAILED(CText2D::PushFrontString(rStr)))
	{ // 追加に失敗した場合

		return E_FAIL;
	}

	CString2D* pHeadStr = GetString2D(0);	// 先頭の文字列情報
	std::vector<CChar2D*> vecAdd;

	// 先頭文字列の自動描画をOFFにする
	pHeadStr->SetEnableDraw(false);

	// 文字列内の文字を配列に追加
	int nNumChar = pHeadStr->GetNumChar();	// 文字数
	for (int nCntChar = 0; nCntChar < nNumChar; nCntChar++)
	{ // 文字数分繰り返す

		// 最後尾に文字アドレスを追加
		vecAdd.push_back(pHeadStr->GetChar2D(nCntChar));
	}

	// メンバ変数の文字配列に作成した文字配列を追加
	m_vecChar.reserve((int)m_vecChar.size() + nNumChar);				// 容量をあらかじめ確保
	m_vecChar.insert(m_vecChar.begin(), vecAdd.begin(), vecAdd.end());	// 先頭にまとめて挿入

	return S_OK;
}

//============================================================
//	文字列の最後尾追加処理 (マルチバイト文字列)
//============================================================
HRESULT CScrollText2D::PushBackString(const std::string& rStr)
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
HRESULT CScrollText2D::PushBackString(const std::wstring& rStr)
{
	// 文字列を最後尾に追加
	if (FAILED(CText2D::PushBackString(rStr)))
	{ // 追加に失敗した場合

		return E_FAIL;
	}

	int nTailStrIdx = GetNumString() - 1;			// 最後尾の文字列インデックス
	CString2D* pTailStr = GetString2D(nTailStrIdx);	// 最後尾の文字列情報

	// 最後尾文字列の自動描画をOFFにする
	pTailStr->SetEnableDraw(false);

	// 文字列内の文字を配列に追加
	int nNumChar = pTailStr->GetNumChar();	// 文字数
	for (int nCntChar = 0; nCntChar < nNumChar; nCntChar++)
	{ // 文字数分繰り返す

		// 最後尾に文字アドレスを追加
		m_vecChar.push_back(pTailStr->GetChar2D(nCntChar));
	}

	return S_OK;
}

//============================================================
//	文字列の削除処理
//============================================================
void CScrollText2D::DeleteString(const int nStrIdx)
{
	int nDelHead = 0;	// 削除する先頭文字インデックス
	for (int i = 0; i < nStrIdx; i++)
	{ // 削除する文字列の手前まで繰り返す

		// 削除する文字の先頭インデックスまで値を進める
		nDelHead += GetString2D(i)->GetNumChar();
	}

	// 削除するイテレーターの先頭を求める
	auto itrDelHead = m_vecChar.begin();	// 削除する先頭イテレーター
	std::advance(itrDelHead, nDelHead);		// 先頭まで進める

	// 削除するイテレーターの最後尾を求める
	auto itrDelTail = m_vecChar.begin();	// 削除する最後尾イテレーター
	std::advance(itrDelTail, nDelHead + GetString2D(nStrIdx)->GetNumChar());	// 最後尾まで進める

	// 先頭から最後尾までのイテレーターを削除
	m_vecChar.erase(itrDelHead, itrDelTail);

	// 文字列の削除
	CText2D::DeleteString(nStrIdx);

	// 文字インデックスを制限する
	useful::LimitMaxNum(m_nNextIdx, (int)m_vecChar.size());
}

//============================================================
//	文字列の全削除処理
//============================================================
void CScrollText2D::DeleteStringAll()
{
	// 文字列の全削除
	CText2D::DeleteStringAll();

	// 全文字情報配列をクリア
	m_vecChar.clear();

	// 文字インデックスを初期化
	m_nNextIdx = 0;
}

//============================================================
//	生成処理
//============================================================
CScrollText2D* CScrollText2D::Create
(
	const std::string& rFilePath,	// フォントパス
	const bool bItalic,				// イタリック
	const VECTOR3& rPos,			// 原点位置
	const float fNextTime,			// 文字表示の待機時間
	const float fCharHeight,		// 文字縦幅
	const float fLineHeight,		// 行間縦幅
	const EAlignX alignX,			// 横配置
	const EAlignY alignY,			// 縦配置
	const VECTOR3& rRot,			// 原点向き
	const COLOR& rCol				// 色
)
{
	// 文字送りテキスト2Dの生成
	CScrollText2D* pScrollText2D = new CScrollText2D;
	if (pScrollText2D == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// 文字送りテキスト2Dの初期化
		if (FAILED(pScrollText2D->Init()))
		{ // 初期化に失敗した場合

			// 文字送りテキスト2Dの破棄
			SAFE_DELETE(pScrollText2D);
			return nullptr;
		}

		// フォントを設定
		pScrollText2D->SetFont(rFilePath, bItalic);

		// 原点位置を設定
		pScrollText2D->SetVec3Position(rPos);

		// 原点向きを設定
		pScrollText2D->SetVec3Rotation(rRot);

		// 色を設定
		pScrollText2D->SetColor(rCol);

		// 文字表示の待機時間を設定
		pScrollText2D->SetNextTime(fNextTime);

		// 文字縦幅を設定
		pScrollText2D->SetCharHeight(fCharHeight);

		// 行間縦幅を設定
		pScrollText2D->SetLineHeight(fLineHeight);

		// 横配置を設定
		pScrollText2D->SetAlignX(alignX);

		// 縦配置を設定
		pScrollText2D->SetAlignY(alignY);

		// 確保したアドレスを返す
		return pScrollText2D;
	}
}

//============================================================
//	文字送りの更新処理
//============================================================
void CScrollText2D::UpdateScroll(const float fDeltaTime)
{
	// 文字送りがOFFなら抜ける
	if (!m_bScroll) { return; }

	// 文字がない場合抜ける
	if (m_vecChar.empty()) { m_bScroll = false; return; }

	// 現在の待機時間を加算
	m_fCurTime += fDeltaTime;
	while (m_fCurTime >= m_fNextTime)
	{ // 待機し終わった場合

		// 文字の自動描画をONにする
		assert(m_vecChar[m_nNextIdx] != nullptr);
		m_vecChar[m_nNextIdx]->SetEnableDraw(true);

		// 現在の待機時間から待機時間を減算
		m_fCurTime -= m_fNextTime;

		// 文字送り中の効果音を再生
		PlayScrollSE(m_vecChar[m_nNextIdx]);

		// 次の文字インデックスに移行
		m_nNextIdx++;

		if (useful::LimitMaxNum(m_nNextIdx, (int)m_vecChar.size() - 1))
		{ // 最後の文字に到達した場合

			// 現在の待機時間を初期化
			m_fCurTime = 0.0f;

			// 文字送りをOFFにする
			m_bScroll = false;

			break;
		}
	}
}

//============================================================
//	文字送り効果音の再生処理
//============================================================
void CScrollText2D::PlayScrollSE(CChar2D* pChar2D)
{
	// ラベルが指定なしの場合抜ける
	if (m_labelSE == CSound::LABEL_NONE) { return; }

	// テクスチャが透明な場合抜ける
	if (pChar2D->IsTexEmpty()) { return; }

	// 指定ラベルのSEを再生
	PLAY_SOUND(m_labelSE);
}
