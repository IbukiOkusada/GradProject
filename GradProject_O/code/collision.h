//==========================================================
//
// 当たり判定の汎用処理 [collision.h]
// Author : Riku Nakamura
//
//==========================================================
#ifndef _COLLISION_H_
#define _COLLISION_H_

#include "main.h"		//main.hで定義しているものが必要なためinclude

//==========================================================
// クラスの定義
//==========================================================
namespace collision
{
	/**
	@brief	点と三角形ポリゴンの衝突判定
	@param	vtx1		[in]	ポリゴンの頂点
	@param	vtx2		[in]	ポリゴンの頂点
	@param	vtx3		[in]	ポリゴンの頂点
	@param	vtxNor		[in]	ポリゴンの法線ベクトル
	@param	posTarget	[in]	判定する位置
	@param	rHeight		[out]	戻す高さ
	@return	bool
	 */
	bool IsOnTrianglePolygon(D3DXVECTOR3 vtx1, D3DXVECTOR3 vtx2, D3DXVECTOR3 vtx3, D3DXVECTOR3 vtxNor, D3DXVECTOR3 posTarget, float& rHeight);

	/**
	@brief	点とポリゴンの衝突判定
	@param	vtx1			[in]	ポリゴンの頂点
	@param	vtx2			[in]	ポリゴンの頂点
	@param	vtx3			[in]	ポリゴンの頂点
	@param	vtx4			[in]	ポリゴンの頂点
	@param	vtxNor1			[in]	1つめのポリゴンの法線ベクトル
	@param	vtxNor2			[in]	2つめのポリゴンの法線ベクトル
	@param	posTarget		[in]	判定する位置
	@param	posTargetOld	[in]	判定する過去位置
	@param	rHeight			[out]	戻す高さ
	@return	bool
	 */
	bool IsOnSquarePolygon(D3DXVECTOR3 vtx1, D3DXVECTOR3 vtx2, D3DXVECTOR3 vtx3, D3DXVECTOR3 vtx4, D3DXVECTOR3 vtxNor1, D3DXVECTOR3 vtxNor2, D3DXVECTOR3 posTarget, D3DXVECTOR3 posOldTarget, float& rHeight);
	
	/**
	@brief	点と線の押し戻し判定
	@param	vtx1			[in]		線分の始点
	@param	vtx2			[in]		線分の終点
	@param	posTarget		[in,out]	判定する位置
	@param	posTargetOld	[in]		判定する過去位置
	@return	bool
	 */
	bool LineCrossProduct(D3DXVECTOR3 vtx1, D3DXVECTOR3 vtx2, D3DXVECTOR3* pos, D3DXVECTOR3 posOld);

	/**
	@brief	点とOBBの衝突判定
	@param	posO		[in]	点の位置
	@param	posOldO		[in]	点の過去位置
	@param	posV		[in]	OBB側の位置
	@param	rotV		[in]	OBB側の角度
	@param	sizeV		[in]	OBB側のサイズ
	@param	posInter	[out]	衝突位置
	@param	vecRef		[out]	押し戻し距離
	@param	fRefIner	[in]	押し戻しの倍率
	@return	bool
	 */
	bool CollidePointToOBBTrigger(D3DXVECTOR3 posO, D3DXVECTOR3 posOldO, D3DXVECTOR3 posV, D3DXVECTOR3 rotV, D3DXVECTOR3 sizeV, D3DXVECTOR3* posInter = nullptr, D3DXVECTOR3* vecRef = nullptr, float fRefIner = 1.0f);

	/**
	@brief	点とOBBの押し戻し判定
	@param	posO		[in,out]	点の位置
	@param	posOldO		[in]		点の過去位置
	@param	posV		[in]		OBB側の位置
	@param	rotV		[in]		OBB側の角度
	@param	sizeV		[in]		OBB側のサイズ
	@return	bool
	 */
	bool CollidePointToOBB(D3DXVECTOR3* posO, D3DXVECTOR3 posOldO, D3DXVECTOR3 posV, D3DXVECTOR3 rotV, D3DXVECTOR3 sizeV);

	/**
	@brief	点とOBBの押し戻し判定
	@param	pOut		[out]		押し戻し距離のポインタ
	@param	posO		[in,out]	点の位置
	@param	posOldO		[in]		点の過去位置
	@param	posV		[in]		OBB側の位置
	@param	rotV		[in]		OBB側の角度
	@param	sizeV		[in]		OBB側のサイズ
	@return	bool
	 */
	bool CollidePointToOBB(D3DXVECTOR3* pOut, D3DXVECTOR3* posO, D3DXVECTOR3 posOldO, D3DXVECTOR3 posV, D3DXVECTOR3 rotV, D3DXVECTOR3 sizeV);

	/**
	@brief	点とOBBの反射判定
	@param	pOut		[out]	押し戻し距離のポインタ
	@param	posO		[in]	点の位置
	@param	posOldO		[in]	点の過去位置
	@param	posV		[in]	OBB側の位置
	@param	rotV		[in]	OBB側の角度
	@param	sizeV		[in]	OBB側のサイズ
	@param	fRefIner	[in]	反射の倍率
	@return	bool
	 */
	bool ReflectPointToOBB(D3DXVECTOR3* pOut, D3DXVECTOR3* posO, D3DXVECTOR3* moveO, D3DXVECTOR3 posOldO, D3DXVECTOR3 posV, D3DXVECTOR3 rotV, D3DXVECTOR3 sizeV, float fRefIner);

	/**
	@brief	OBBと平面の押し戻し判定
	@param	posOBB			[in,out]	OBBの位置
	@param	vecOBBAxial		[in]		OBBの各軸方向
	@param	posPlane		[in]		平面の位置
	@param	vecNorPlane		[in]		平面の法線ベクトル
	@return	D3DXVECTOR3
	 */
	D3DXVECTOR3 CollideOBBToPlane(D3DXVECTOR3* posOBB, D3DXVECTOR3 vecOBBAxial, D3DXVECTOR3 posPlane, D3DXVECTOR3 vecNorPlane);

	/**
	@brief	レイとOBBの衝突判定
	@param	pOut		[out]	衝突地点
	@param	posO		[in]	レイ側の位置
	@param	vecO		[in]	レイ側の方向ベクトル
	@param	posV		[in]	OBB側の位置
	@param	rotV		[in]	OBB側の角度
	@param	sizeMaxV	[in]	OBB側の最大サイズ
	@param	sizeMinV	[in]	OBB側の最小サイズ
	@return	bool
	 */
	bool CollideRayToOBB(D3DXVECTOR3* pOut, D3DXVECTOR3 posO, D3DXVECTOR3 vecO, D3DXVECTOR3 posV, D3DXVECTOR3 rotV, D3DXVECTOR3 sizeMaxV, D3DXVECTOR3 sizeMinV);

	/**
	@brief	OBBとOBBの衝突判定
	@param	posO		[in]	OBB1側の位置
	@param	rotO		[in]	OBB1側の角度
	@param	sizeO		[in]	OBB1側のサイズ
	@param	posV		[in]	OBB2側の位置
	@param	rotV		[in]	OBB2側の角度
	@param	sizeV		[in]	OBB2側のサイズ
	@return	bool
	 */
	bool CollideOBBToOBBTrigger(D3DXVECTOR3 posO, D3DXVECTOR3 rotO, D3DXVECTOR3 sizeO, D3DXVECTOR3 posV, D3DXVECTOR3 rotV, D3DXVECTOR3 sizeV);

	// 当たり判定内計算処理用関数群
	float lengthAxis(D3DXVECTOR3 separationAxis, D3DXVECTOR3 e1, D3DXVECTOR3 e2, D3DXVECTOR3 e3);
	D3DXVECTOR3 PosRelativeMtx(D3DXVECTOR3 posO, D3DXVECTOR3 rot, D3DXVECTOR3 offset);
	D3DXVECTOR3 NorRelativeMtx(D3DXVECTOR3 posO, D3DXVECTOR3 rot, D3DXVECTOR3 offsetMax, D3DXVECTOR3 offsetMin);
	D3DXVECTOR3 GetMtxPos(D3DXMATRIX mtx);
	float CrossProduct(D3DXVECTOR3 pos1, D3DXVECTOR3 pos2);
}

#endif
