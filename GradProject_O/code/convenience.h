#include "main.h"
//=============================================
//距離取得関数
//=============================================
float GetDistance(D3DXVECTOR3 vec1, D3DXVECTOR3 vec2);
#ifndef my_max
#define my_max(a,b)            (((a) > (b)) ? (a) : (b))// 大きい数値を返す
#endif

#ifndef my_min
#define my_min(a,b)            (((a) < (b)) ? (a) : (b))// 小さい数値を返す
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)		if ((p) != nullptr) { delete (p);		(p) = nullptr; }	// 破棄
#endif

#ifndef SAFE_UNINIT
#define SAFE_UNINIT(p)		if ((p) != nullptr) { (p)->Uninit();	(p) = nullptr; }	// Uninitを使用した破棄
#endif