//============================================================
//
//	フォント処理 [font.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "font.h"
#include "manager.h"
#include "renderer.h"
#include "convenience.h"
//************************************************************
//	定数宣言
//************************************************************
namespace
{
	// 登録情報構造体
	struct SRegistInfo
	{
		// デフォルトコンストラクタ
		SRegistInfo() :
			wcChar	(L'\0'),	// 読み込む先頭文字
			nIncr	(0)			// 読み込む連続数
		{}

		// 引数付きコンストラクタ
		SRegistInfo(const wchar_t in_wcChar, const int in_nIncr) :
			wcChar	(in_wcChar),	// 読み込む先頭文字
			nIncr	(in_nIncr)		// 読み込む連続数
		{}

		// デストラクタ
		~SRegistInfo() {}

		// メンバ変数
		const wchar_t wcChar;	// 読み込む先頭文字
		const int nIncr;		// 読み込む連続数
	};
	const SRegistInfo INIT_REGIST_CHAR[] =	// 初期に登録するフォント文字
	{
		SRegistInfo(L'\0', 1),	// 終端文字
		SRegistInfo(L'0', 10),	// 0～9
		SRegistInfo(L'A', 26),	// A～Z
		SRegistInfo(L'a', 26),	// a～z
		SRegistInfo(L'あ', 85),	// あ～ん (ゖ)
		SRegistInfo(L'ア', 90),	// ア～ン (ヺ)
	};

	const char*	LOAD_EXTENSION = "data\\TXT\\EXTENSION\\font.txt";	// フォント読込拡張子相対パス
	const char*	LOAD_FOLDER = "data\\FONT";	// フォントフォルダ相対パス
	const int	FONT_HEIGHT = 240;			// フォント縦幅
}

//************************************************************
//	親クラス [CFont] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CFont::CFont()
{
	// フォント連想配列をクリア
	m_mapFont.clear();

	// 読込済みファイルパスをクリア
	m_vecFilePath.clear();

	// 読み込み可能拡張子をクリア
	m_load.clear();
}

//============================================================
//	デストラクタ
//============================================================
CFont::~CFont()
{

}

//============================================================
//	フォント初期化処理
//============================================================
HRESULT CFont::Init()
{
	// フォント連想配列を初期化
	m_mapFont.clear();

	// 読込済みファイルパスを初期化
	m_vecFilePath.clear();

	// 読み込み可能拡張子を初期化
	m_load.clear();

	// 読み込み可能拡張子の読込
	m_load = extension::LoadExtension(LOAD_EXTENSION);

	return S_OK;
}

//============================================================
//	フォント終了処理
//============================================================
void CFont::Uninit()
{
	for (auto& rMap : m_mapFont)
	{ // フォントの要素数分繰り返す

		// フォント文字の破棄
		SAFE_REF_RELEASE(rMap.second.pFontChar);

		// フォントハンドルの破棄
		SAFE_DEL_OBJECT(rMap.second.pFont);
	}

	for (auto& rVec : m_vecFilePath)
	{ // フォントパスの要素数分繰り返す

		// フォントの破棄
		int nError = RemoveFontResourceEx
		( // 引数
			rVec.c_str(),	// フォントファイルパス
			FR_PRIVATE,		// フォント特性
			nullptr
		);

		if (nError == 0)
		{ // 破棄に失敗した場合

			// エラーメッセージの作成
			std::string sError = "フォントの破棄に失敗！\nパス：";
			sError.append(rVec.c_str());

			// エラーメッセージボックス
			MessageBox(nullptr, sError.c_str(), "警告！", MB_ICONWARNING);
			assert(false);
		}
	}

	// フォント連想配列をクリア
	m_mapFont.clear();

	// 読込済みファイルパスをクリア
	m_vecFilePath.clear();

	// 読み込み可能拡張子をクリア
	m_load.clear();
}

//============================================================
//	フォント全読込処理
//============================================================
HRESULT CFont::LoadAll()
{
	// フォントの全読込
	if (FAILED(SearchFolderAll(LOAD_FOLDER)))
	{ // 読込に失敗した場合

		assert(false);
		return E_FAIL;
	}

	return S_OK;
}

//============================================================
//	フォント読込処理
//============================================================
HRESULT CFont::Load(const std::string& rFilePath)
{
	// 既に読込済みかを検索
	auto itr = std::find(m_vecFilePath.begin(), m_vecFilePath.end(), rFilePath);	// 引数のフォントパスを検索
	if (itr != m_vecFilePath.end())
	{ // 読込済みの場合

		return S_OK;
	}

	// フォントの読込
	int nError = AddFontResourceEx
	( // 引数
		rFilePath.c_str(),	// フォントファイルパス
		FR_PRIVATE,			// フォント特性
		nullptr
	);
	if (nError == 0)
	{ // 読込に失敗した場合

		// エラーメッセージの作成
		std::string sError = "フォントの読込に失敗！\nパス：";
		sError.append(rFilePath.c_str());

		// エラーメッセージボックス
		MessageBox(nullptr, sError.c_str(), "警告！", MB_ICONWARNING);
		return E_FAIL;
	}

	// 読み込んだフォントパスを保存
	m_vecFilePath.push_back(rFilePath);

#if NDEBUG	// Release版以外では事前登録を行わない

	// フォントとフォント文字の事前登録
	RegistPrepare(rFilePath);

#endif	// NDEBUG

	return S_OK;
}

//============================================================
//	フォント登録処理
//============================================================
CFont::SFont CFont::Regist(const std::string& rFilePath, const bool bItalic)
{
	// 既に生成済みかを検索
	auto itr = m_mapFont.find(SKey(rFilePath, bItalic));	// 引数のフォントを検索
	if (itr != m_mapFont.end())
	{ // 生成済みの場合

		// 読込済みのフォント情報を返す
		return itr->second;
	}

	// 設定用フォント情報を宣言
	SFont tempFont;

	// フォント名を設定
	tempFont.sFontName = rFilePath;					// ファイルパスを設定
	PathToBaseName(&tempFont.sFontName);	// ベースネームのみに変換

	// フォント属性の設定
	LOGFONT lf;
	lf.lfHeight			= FONT_HEIGHT;						// フォントの縦幅
	lf.lfWidth			= 0;								// 平均文字幅
	lf.lfEscapement		= 0;								// 描画角度
	lf.lfOrientation	= 0;								// テキスト方向
	lf.lfWeight			= FW_NORMAL;						// フォントの太さ
	lf.lfItalic			= bItalic;							// イタリック
	lf.lfUnderline		= false;							// アンダーライン
	lf.lfStrikeOut		= false;							// 打ち消し線
	lf.lfCharSet		= DEFAULT_CHARSET;					// 文字セット
	lf.lfOutPrecision	= OUT_TT_ONLY_PRECIS;				// 出力精度
	lf.lfClipPrecision	= CLIP_DEFAULT_PRECIS;				// クリッピング精度
	lf.lfQuality		= PROOF_QUALITY;					// 出力品質
	lf.lfPitchAndFamily	= (DEFAULT_PITCH | FF_MODERN);		// ピッチとファミリー
	strcpy(&lf.lfFaceName[0], tempFont.sFontName.c_str());	// フォント名

	// フォントハンドルの生成
	tempFont.pFont = CreateFontIndirectA(&lf);
	if (tempFont.pFont == nullptr)
	{ // 生成に失敗した場合

		// 初期値を返す
		assert(false);
		return {};
	}

	// フォント文字の生成
	tempFont.pFontChar = CFontChar::Create(tempFont.pFont, rFilePath, bItalic);
	if (tempFont.pFontChar == nullptr)
	{ // 生成に失敗した場合

		// 初期値を返す
		assert(false);
		return {};
	}

	// フォント情報を保存
	m_mapFont.insert(std::make_pair(SKey(rFilePath, bItalic), tempFont));

	// 生成したフォント情報を返す
	return tempFont;
}

//============================================================
//	フォント文字登録
//============================================================
CFontChar::SChar CFont::RegistChar
(
	const wchar_t wcChar,			// 指定文字
	const std::string& rFilePath,	// ファイルパス
	bool bItalic					// イタリック
)
{
	// 生成したフォントの文字テクスチャインデックスを返す
	return Regist(rFilePath, bItalic).pFontChar->Regist(wcChar);
}

//============================================================
//	生成処理
//============================================================
CFont* CFont::Create()
{
	// フォントの生成
	CFont* pFont = DEBUG_NEW CFont;
	if (pFont == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// フォントの初期化
		if (FAILED(pFont->Init()))
		{ // フォント初期化に失敗した場合

			// フォントの破棄
			SAFE_DELETE(pFont);
			return nullptr;
		}

		// 確保したアドレスを返す
		return pFont;
	}
}

//============================================================
//	破棄処理
//============================================================
void CFont::Release(CFont*& prFont)
{
	// フォントの終了
	assert(prFont != nullptr);
	prFont->Uninit();

	// メモリ開放
	SAFE_DELETE(prFont);
}

//============================================================
//	フォント・フォント文字の事前登録
//============================================================
void CFont::RegistPrepare(const std::string& rFilePath)
{
	// 引数パスのフォントを登録
	Regist(rFilePath);

	// 使用頻度の高いフォント文字を登録する
	for (int i = 0; i < NUM_ARRAY(INIT_REGIST_CHAR); i++)
	{ // 読み込む文字の先頭数分繰り返す

		for (int j = 0; j < INIT_REGIST_CHAR[i].nIncr; j++)
		{ // 読み込む文字の連続数分繰り返す

			// オフセット分ずらした文字を計算
			wchar_t wcOffset = INIT_REGIST_CHAR[i].wcChar + (wchar_t)j;

			// フォント文字を登録
			RegistChar(wcOffset, rFilePath);
		}
	}
}

//============================================================
//	フォルダ全検索処理
//============================================================
HRESULT CFont::SearchFolderAll(const std::string& rFolderPath)
{
	HANDLE hFile;	// 検索ハンドル
	WIN32_FIND_DATA findFileData;	// ファイル情報

	// 引数パスのディレクトリを取得
	std::string sAllLoadPath = rFolderPath + "\\*.*";	// 全読込パス
	hFile = FindFirstFile(sAllLoadPath.c_str(), &findFileData);
	if (INVALID_HANDLE_VALUE == hFile)
	{ // ハンドルが無効の場合

		assert(false);
		return E_FAIL;
	}

	do
	{ // ファイル内の情報全てを読み込む

		// 現在のディレクトリ、親ディレクトリの場合次のループに移行
		if (strcmp(findFileData.cFileName, ".") == 0)	{ continue; }
		if (strcmp(findFileData.cFileName, "..") == 0)	{ continue; }

		// ファイル名を相対パスに変換
		std::string sFullPath = rFolderPath;	// 現在の相対パスを設定
		sFullPath += "\\";						// パス区切り文字を追加
		sFullPath += findFileData.cFileName;	// ファイル名を追加

		if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{ // ディレクトリだった場合

			// 新たなディレクトリを全検索
			SearchFolderAll(sFullPath);
		}
		else
		{ // ファイルだった場合

			if (extension::IsLoadOK(m_load, sFullPath.c_str()))
			{ // 読込可能な拡張子だった場合

				// フォントを読込
				if (FAILED(Load(sFullPath.c_str())))
				{ // 登録に失敗した場合

					assert(false);
					return E_FAIL;
				}
			}
		}

	} while (FindNextFile(hFile, &findFileData));	// 次のファイルを検索

	// 検索ハンドルを閉じる
	FindClose(hFile);

	return S_OK;
}
