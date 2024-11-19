//============================================================
//
//	拡張子判定関数処理 [extension.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "extension.h"

//============================================================
//	読込可能拡張子の読込処理
//============================================================
extension::ALoad extension::LoadExtension(const char* pFilePath)
{
	ALoad vecLoad;	// 拡張子の動的配列

	// ファイルを開く
	std::ifstream file(pFilePath);	// ファイルストリーム
	if (file.fail())
	{ // ファイルが開けなかった場合

		// エラーメッセージボックス
		MessageBox(nullptr, "拡張子ファイルの読み込みに失敗！", "警告！", MB_ICONWARNING);

		// 初期値を返す
		return vecLoad;
	}

	// ファイルを読込
	std::string str;	// 読込文字列
	while (file >> str)
	{ // ファイルの終端ではない場合ループ

		if (str.front() == '#')
		{ // コメントアウトされている場合

			// 一行全て読み込む
			std::getline(file, str);
		}
		else if (str == "EXTENSION")
		{
			file >> str;	// ＝を読込
			file >> str;	// 読込可能な拡張子を読込

			// 読み込んだ拡張子を最後尾に追加
			vecLoad.push_back(str);
		}
	}

	// ファイルを閉じる
	file.close();

	// 拡張子の動的配列を返す
	return vecLoad;
}

//============================================================
//	読込可能フラグの取得処理
//============================================================
bool extension::IsLoadOK(const ALoad& rLoadInfo, const char* pLoadFilePath)
{
	std::filesystem::path fsFullPath = pLoadFilePath;			// ファイルパス
	std::filesystem::path fsExtension = fsFullPath.extension();	// 拡張子

	int nSize = (int)rLoadInfo.size();	// 読み込める拡張子の総数
	for (int i = 0; i < nSize; i++)
	{ //  読み込める拡張子の総数分繰り返す

		// 拡張子が読込可能な形式があった場合抜ける
		if (fsExtension == rLoadInfo[i]) { return true; }
	}

	// 読み込み可能拡張子が一つもない場合抜ける
	return false;
}
