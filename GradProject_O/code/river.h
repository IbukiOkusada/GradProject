//===============================================
//
// 川の処理 [river.h]
// Author : Ibuki Okusada
//
//===============================================
#ifndef _RIVER_H_
#define _RIVER_H_

#include "task.h"
#include "list.h"

namespace river
{
	const int MAX = 2;	// 枚数
}

// 前方宣言
class CMeshField;

//==========================================================
// 警察のクラス定義
//==========================================================
class CRiver : public CTask
{
private:

	struct SInfo
	{
		D3DXVECTOR3 pos;	// 座標
		D3DXVECTOR3 rot;	// 向き
		D3DXVECTOR2 size;	// 1枚辺りの幅
		D3DXVECTOR2 speed;	// 移動速度
		int nWidth;			// 横枚数
		int nHeight;		// 縦枚数

		// コンストラクタ
		SInfo() : pos(VECTOR3_ZERO), rot(VECTOR3_ZERO), size(VECTOR2_ZERO), speed(VECTOR2_ZERO), nWidth(0), nHeight(0){}
		SInfo(const D3DXVECTOR3& _pos, const D3DXVECTOR3& _rot, const D3DXVECTOR2& _size, const D3DXVECTOR2& _speed, const int _width, const int _height):
			pos(_pos), rot(_rot), size(_size), speed(_speed), nWidth(_width), nHeight(_height) {}
	};

public:

	CRiver(SInfo info = SInfo());	// コンストラクタ
	~CRiver();	// デストラクタ

	// メンバ関数
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;

	/**
	@brief	川の生成
	@param	pos		[in]	座標
	@param	rot		[in]	向き
	@param	size	[in]	一枚辺りのサイズ
	@param	speed	[in]	スクロール速度
	@param	nWidth	[in]	横枚数
	@param	nHeight	[in]	縦枚数
	@return	CRiver*
	*/
	static CRiver* Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, const D3DXVECTOR2& size, 
		const D3DXVECTOR2& speed, const int& nWidth, const int& nHeight);

private:

	// メンバ変数
	SInfo m_Info;	// 基本情報
	std::vector<CMeshField*> m_obj;	// オブジェクト情報
};

#endif
