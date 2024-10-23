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

#define my_max(a,b)            (((a) > (b)) ? (a) : (b))// 大きい数値を返す

#define my_min(a,b)            (((a) < (b)) ? (a) : (b))// 小さい数値を返す

#define SAFE_DELETE(p)		if ((p) != nullptr) { delete (p);		(p) = nullptr; }	// 破棄

#define SAFE_UNINIT(p)		if ((p) != nullptr) { (p)->Uninit();	(p) = nullptr; }	// Uninitを使用した破棄




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


#endif