#include "manager.h"
#include "texture.h"
//=============================================
//距離取得関数
//=============================================
float GetDistance(D3DXVECTOR3 vec1, D3DXVECTOR3 vec2)
{
	D3DXVECTOR3 Dis = vec1 - vec2;
	if (Dis.x < 0.0f)
	{
		Dis.x *= -1.0f;
	}
	if (Dis.y < 0.0f)
	{
		Dis.y *= -1.0f;
	}
	if (Dis.z < 0.0f)
	{
		Dis.z *= -1.0f;
	}
	float fDis = Dis.x + Dis.y + Dis.z;
	return fDis;
}
//============================================================
//	縦幅からテクスチャのアスペクト比を考慮した横幅の取得処理
//============================================================
float GetTexWidthFromAspect(const float fHeight, const int nTexIdx)
{
	// テクスチャアスペクト比を取得
	D3DXVECTOR2 aspect = CManager::GetInstance()->GetTexture()->GetTexFile(nTexIdx)->aspect;

	// アスペクト比から計算した横幅を返す
	return fHeight * aspect.x;
}

//============================================================
//	ワイド文字列のマルチバイト文字列変換
//============================================================
std::string WideToMultiByte(const std::wstring& rSrcStr)
{
	int nSrcSize = (int)rSrcStr.size();	// 変換前の文字列のサイズ
	if (nSrcSize <= 0) { return ""; }	// 文字列がない場合抜ける

	// 変換後の文字列サイズを取得
	int nDestSize = WideCharToMultiByte
	( // 引数
		CP_ACP,				// 変換コードページ
		0,					// 変換フラグ
		&rSrcStr.front(),	// 変換前文字列の先頭アドレス
		nSrcSize,			// 変換前文字列のサイズ
		nullptr,			// 変換後文字列の先頭アドレス
		0,					// 変換後文字列のサイズ
		nullptr,			// 変換不可時の置換文字
		nullptr				// 変換不可な文字が存在したか
	);

	// 文字列を変換
	std::string sDest(nDestSize, '\0');	// 変換後の文字列
	WideCharToMultiByte
	( // 引数
		CP_ACP,				// 変換コードページ
		0,					// 変換フラグ
		&rSrcStr.front(),	// 変換前文字列の先頭アドレス
		nSrcSize,			// 変換前文字列のサイズ
		&sDest.front(),		// 変換後文字列の先頭アドレス
		(int)sDest.size(),	// 変換後文字列のサイズ
		nullptr,			// 変換不可時の置換文字
		nullptr				// 変換不可な文字が存在したか
	);

	// 変換後の文字列を返す
	return sDest;
}

//=============================================
// 向き補正(全体)
//=============================================
void Adjust(D3DXVECTOR3& rot)
{
	// 一つずつ補正
	Adjust(rot.x);
	Adjust(rot.y);
	Adjust(rot.z);
}

//=============================================
// 向き補正(単体)
//=============================================
void Adjust(float& rot)
{
	// -3.14から3.14の間に補正
	while (1)
	{
		if (rot > D3DX_PI)
		{
			rot += -D3DX_PI * 2;
		}
		else if (rot < -D3DX_PI)
		{
			rot += D3DX_PI * 2;
		}
		else
		{
			break;
		}
	}
}


// ベクトルを角度に変換する関数
D3DXVECTOR3 VectorToAngles(const D3DXVECTOR3& vector)
{
	D3DXVECTOR3 angles;

	// Yaw（ヨー）を計算
	angles.y = atan2f(vector.x, vector.z);

	// Pitch（ピッチ）を計算
	angles.x = atan2f(vector.y, sqrt(vector.x * vector.x + vector.z * vector.z));

	// Roll（ロール）は0度に設定
	angles.z = 0.0f;



	return angles;
}

// 角度をベクトルに変換する関数
D3DXVECTOR3 AnglesToVector(const D3DXVECTOR3& angles)
{
	D3DXVECTOR3 vector;

	// ラジアンを度に変換
	float yaw = (angles.y);
	float pitch = (angles.x);

	// ベクトルを計算
	vector.x = sinf(yaw) * cosf(pitch);
	vector.y = sinf(pitch);
	vector.z = cosf(yaw) * cosf(pitch);

	return vector;
}

//============================================================
//	パスのベースネーム変換
//============================================================
void PathToBaseName(std::string* pPath)
{
	std::filesystem::path fsPath(*pPath);	// パス

	// パスからベースネームを取得
	*pPath = fsPath.stem().string();
}

//============================================================
//	マルチバイト文字列のワイド文字列変換
//============================================================
std::wstring MultiByteToWide(const std::string& rSrcStr)
{
	int nSrcSize = (int)rSrcStr.size();	// 変換前の文字列のサイズ
	if (nSrcSize <= 0) { return L""; }	// 文字列がない場合抜ける

	// 文字列を変換
	std::wstring wsDest(nSrcSize, L'\0');	// 変換後の文字列
	int nDestSize = MultiByteToWideChar
	( // 引数
		CP_ACP,				// 変換コードページ
		0,					// 変換フラグ
		&rSrcStr.front(),	// 変換前文字列の先頭アドレス
		nSrcSize,			// 変換前文字列のサイズ
		&wsDest.front(),	// 変換後文字列の先頭アドレス
		(int)wsDest.size()	// 変換後文字列のサイズ
	);

	// 文字列サイズを修正
	wsDest.resize(nDestSize);

	// 変換後の文字列を返す
	return wsDest;
}