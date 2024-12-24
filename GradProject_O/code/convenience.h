#include "main.h"
#include <memory>
#include <mutex>

//=============================================
//便利関数
//=============================================
using namespace std;
#ifndef CONVENIENCE
#define CONVENIENCE		// 二重インクルード防止用マクロを定義
float GetDistance(D3DXVECTOR3 vec1, D3DXVECTOR3 vec2);
D3DXVECTOR3 VectorToAngles(const D3DXVECTOR3& vector);
D3DXVECTOR3 AnglesToVector(const D3DXVECTOR3& angles);

//============================================================
//	値の最大値制限処理
//============================================================
template<class T> bool LimitMaxNum
(
	T& rNum,	// 制限数値
	const T max	// 最大範囲
)
{
	if (rNum > max)
	{ // 制限数値が最大範囲を超えていた場合

		// 範囲内に補正
		rNum = max;
		return true;
	}

	return false;
}
void PathToBaseName(std::string* pPath);
std::wstring MultiByteToWide(const std::string& rSrcStr);
float GetTexWidthFromAspect(const float fHeight, const int nTexIdx);
std::string WideToMultiByte(const std::wstring& rSrcStr);
void Adjust(D3DXVECTOR3* rot);
void Adjust(float* rot);

#define my_max(a,b)            (((a) > (b)) ? (a) : (b))// 大きい数値を返す
#define my_min(a,b)            (((a) < (b)) ? (a) : (b))// 小さい数値を返す

#define NUM_ARRAY(a)	(sizeof((a)) / sizeof((a)[0]))	// 配列の要素数計算

#define SAFE_DELETE(p)		if ((p) != nullptr) { delete (p);		(p) = nullptr; }	// 破棄
#define SAFE_DELETEARRAY(p)		if ((p) != nullptr) { delete[] (p);		(p) = nullptr; }	// 破棄
#define SAFE_UNINIT(p)		if ((p) != nullptr) { (p)->Uninit();	(p) = nullptr; }	// Uninitを使用した破棄
#define SAFE_UNINIT_DELETE(p)		if ((p) != nullptr) { (p)->Uninit();	 delete (p);	(p) = nullptr; }	// Uninitを使用した破棄
#define SAFE_RELEASE(p)		if ((p) != nullptr) { (p)->Release();		(p) = nullptr; }	// Release関数の破棄マクロ
#define SAFE_REF_RELEASE(p)	if ((p) != nullptr) { (p)->Release((p)); }						// 参照ポインタ付きRelease関数の破棄
#define SAFE_DEL_OBJECT(p)	if ((p) != nullptr) { DeleteObject((p));	(p) = nullptr; }	// DeleteObject関数の破棄マクロ
#define VECTOR3_ZERO (D3DXVECTOR3(0.0f,0.0f,0.0))
#define VECTOR2_ZERO (D3DXVECTOR2(0.0f,0.0f))
#define VECTOR3_ONE (D3DXVECTOR3(1.0f,1.0f,1.0f))
#define VECTOR2_ONE (D3DXVECTOR2(1.0f,1.0f))


// POSGRID2関係
#define GRID2_ZERO	(POSGRID2())	// 0クリア
#define GRID2_ONE	(POSGRID2(1))	// 1クリア
//************************************************************
//	列挙型定義
//************************************************************
// 横配置列挙
enum EAlignX
{
	XALIGN_LEFT = 0,	// 左揃え
	XALIGN_CENTER,		// 中央揃え
	XALIGN_RIGHT,		// 右揃え
	XALIGN_MAX,			// この列挙型の総数
};

// 縦配置列挙
enum EAlignY
{
	YALIGN_TOP = 0,	// 上揃え
	YALIGN_CENTER,	// 中央揃え
	YALIGN_BOTTOM,	// 下揃え
	YALIGN_MAX,		// この列挙型の総数
};
//汎用テンプレート
template <typename T> class CMaster
{
public:
	CMaster() : value(make_shared<T>()) {};

	//各種アクセサメソッド
	T Get() { return *value.get(); };										//ゲット
	T Get(mutex mut) { lock_guard<mutex> lock(mut); return value.get(); };	//ゲット(スレッドセーフ)
	shared_ptr<T> GetPtr() { return value; };								//本体取得

	void Set(T in) { *value = in; };										//セット
	void Set(T in, mutex mut) { lock_guard<mutex> lock(mut); *value = in; };//セット(スレッドセーフ)
	void SetPtr(shared_ptr<T> in) { value = in; };							//本体代入
																			
private:
	shared_ptr<T> value;//本体
};
// 2整数座標
struct POSGRID2
{
	// デフォルトコンストラクタ
	POSGRID2() : x(0), y(0) {}

	// 引数付きコンストラクタ
	POSGRID2(const int XY) : x(XY), y(XY) {}
	POSGRID2(const int X, const int Y) : x(X), y(Y) {}

	// デストラクタ
	~POSGRID2() {}

	// メンバ変数
	int x;	// 方眼座標 (x)
	int y;	// 方眼座標 (y)

	//********************************************************
	//	演算子オーバーロード
	//********************************************************
	// 加算
	inline POSGRID2 operator+(const POSGRID2& rGrid) const { return POSGRID2(x + rGrid.x, y + rGrid.y); }
	inline POSGRID2 operator+(const int nVal) const { return POSGRID2(x + nVal, y + nVal); }
	inline POSGRID2 operator+() const { return *this; }

	// 減算
	inline POSGRID2 operator-(const POSGRID2& rGrid) const { return POSGRID2(x - rGrid.x, y - rGrid.y); }
	inline POSGRID2 operator-(const int nVal) const { return POSGRID2(x - nVal, y - nVal); }
	inline POSGRID2 operator-() const { return POSGRID2(-x, -y); }

	// 乗算
	inline POSGRID2 operator*(const int nVal) const { return POSGRID2(x * nVal, y * nVal); }

	// 除算
	inline POSGRID2 operator/(const int nVal) const { return POSGRID2(x / nVal, y / nVal); }

	// 加算代入
	inline POSGRID2& operator+=(const int nVal)
	{
		x += nVal;
		y += nVal;
		return *this;
	}
	inline POSGRID2& operator+=(const POSGRID2& rGrid)
	{
		x += rGrid.x;
		y += rGrid.y;
		return *this;
	}

	// 減算代入
	inline POSGRID2& operator-=(const int nVal)
	{
		x -= nVal;
		y -= nVal;
		return *this;
	}
	inline POSGRID2& operator-=(const POSGRID2& rGrid)
	{
		x -= rGrid.x;
		y -= rGrid.y;
		return *this;
	}

	// 乗算代入
	inline POSGRID2& operator*=(const int nVal)
	{
		x *= nVal;
		y *= nVal;
		return *this;
	}
	inline POSGRID2& operator*=(const POSGRID2& rGrid)
	{
		x *= rGrid.x;
		y *= rGrid.y;
		return *this;
	}

	// 除算代入
	inline POSGRID2& operator/=(const int nVal)
	{
		x /= nVal;
		y /= nVal;
		return *this;
	}
	inline POSGRID2& operator/=(const POSGRID2& rGrid)
	{
		x /= rGrid.x;
		y /= rGrid.y;
		return *this;
	}

	/*
		@brief	座標が一致するかの検証
		@param	const POSGRID2& [in] 判定する方眼座標
		@return	判定結果
	*/
	inline bool operator==(const POSGRID2& rGrid) const
	{
		if (rGrid.x == x && rGrid.y == y) { return true; }
		return false;
	}

	/*
		@brief	座標が一致しないかの検証
		@param	const POSGRID2& [in] 判定する方眼座標
		@return	判定結果
	*/
	inline bool operator!=(const POSGRID2& rGrid) const
	{
		if (rGrid.x != x || rGrid.y != y) { return true; }
		return false;
	}

	//********************************************************
	//	メンバ関数
	//********************************************************
	/* @brief 方眼座標の反転 */
	inline void Invert() { *this = POSGRID2(-x, -y); }

	/* @brief 方眼座標の絶対値 */
	inline void Absolute() { *this = POSGRID2(std::abs(x), std::abs(y)); }

	/*
		@brief	要素がすべてゼロか
		@return	判定結果
	*/
	inline bool IsZero() const
	{
		if (x == 0 && y == 0) { return true; }
		return false;
	}
};

#endif