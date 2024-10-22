//==========================================================
//
// 当たり判定の汎用処理 [collision.cpp]
// Author : Riku Nakamura
//
//==========================================================
#include "collision.h"

namespace collision
{
//========================================
// 外積の計算
//========================================
float CrossProduct(D3DXVECTOR3 pos1, D3DXVECTOR3 pos2)
{
	return pos1.z * pos2.x - pos1.x * pos2.z;
}

//========================================
// 三角形の下にいるかの判定
//========================================
bool IsOnTrianglePolygon(D3DXVECTOR3 vtx1, D3DXVECTOR3 vtx2, D3DXVECTOR3 vtx3, D3DXVECTOR3 vtxNor, D3DXVECTOR3 posTarget, float& rHeight)
{
	D3DXVECTOR3 vecP, vecTemp;
	float fHeight, fDot;

	// ポリゴンの外周ベクトルを設定
	D3DXVECTOR3 vecVtx[3];
	vecVtx[0] = vtx2 - vtx1;
	vecVtx[1] = vtx3 - vtx2;
	vecVtx[2] = vtx1 - vtx3;

	if (D3DXVec3Length(&vecVtx[0]) <= 0 ||
		D3DXVec3Length(&vecVtx[1]) <= 0 ||
		D3DXVec3Length(&vecVtx[2]) <= 0)
		return false;

	// ポリゴンの各点から目標へのベクトルを設定
	D3DXVECTOR3 vecVtxTarget[3];
	vecVtxTarget[0] = posTarget - vtx1;
	vecVtxTarget[1] = posTarget - vtx2;
	vecVtxTarget[2] = posTarget - vtx3;

	// 目標位置がポリゴンの内側にいるか判定
	if (D3DXVec3Cross(&vecTemp, &vecVtxTarget[0], &vecVtx[0])->y < 0 &&
		D3DXVec3Cross(&vecTemp, &vecVtxTarget[1], &vecVtx[1])->y < 0 &&
		D3DXVec3Cross(&vecTemp, &vecVtxTarget[2], &vecVtx[2])->y < 0)
	{
		// y軸法線が0ではないか判定
		if (!vtxNor.y)
			return false;

		// 内積を計算
		fDot = (vtxNor.x * vecVtxTarget[0].x) + (vtxNor.z * vecVtxTarget[0].z);

		// 内積を用いて高さを計算
		fHeight = -(fDot / vtxNor.y) + vtx1.y;

		// 高さが目標位置より高いか判定
		if (fHeight >= rHeight)
		{
			// 高さ代入
			rHeight = fHeight;
			return true;
		}
	}

	return false;
}

//========================================
// ポリゴンの下にいるかの判定
//========================================
bool IsOnSquarePolygon(D3DXVECTOR3 vtx1, D3DXVECTOR3 vtx2, D3DXVECTOR3 vtx3, D3DXVECTOR3 vtx4, D3DXVECTOR3 vtxNor1, D3DXVECTOR3 vtxNor2, D3DXVECTOR3 posTarget, D3DXVECTOR3 posOldTarget, float& rHeight)
{
	bool bColllision = false;

	// 1つ目のポリゴンと内外判定
	if (IsOnTrianglePolygon(vtx1, vtx2, vtx3, vtxNor1, posTarget, rHeight))
		bColllision = true;

	// 2つ目のポリゴンと内外判定
	if (IsOnTrianglePolygon(vtx4, vtx3, vtx2, vtxNor2, posTarget, rHeight))
		bColllision = true;

	return bColllision;
}

//========================================
// 線に対する外積の押し戻し判定
//========================================
bool LineCrossProduct(D3DXVECTOR3 vtx1, D3DXVECTOR3 vtx2, D3DXVECTOR3* pos, D3DXVECTOR3 posOld)
{
	D3DXVECTOR3 vecMove, vecLine, vecToPos, vecToPosOld;
	float fRate, fOutUnit, fOutRate;

	// 角ベクトル計算
	vecMove = *pos - posOld;
	vecLine = vtx1 - vtx2;
	vecToPos = *pos - vtx2;
	vecToPosOld = posOld - vtx2;

	// y方向を0にする
	vecMove.y = 0.0f;
	vecLine.y = 0.0f;
	vecToPos.y = 0.0f;
	vecToPosOld.y = 0.0f;

	// 交点倍率計算
	fOutUnit = CrossProduct(vecLine, vecMove);
	fOutRate = CrossProduct(vecToPos, vecMove);
	fRate = fOutRate / fOutUnit;

	// 線分上をまたいでいるか判定
	if (CrossProduct(vecLine, vecToPos) > 0.0f &&
		CrossProduct(vecLine, vecToPosOld) <= 0.0f &&
		fRate >= 0.0f && fRate < 1.0f)
	{
		D3DXVECTOR3 vecNor, posCross;
		float fDot;

		//法線ベクトル計算
		vecNor.x = -vecLine.z;
		vecNor.y = 0.0f;
		vecNor.z = vecLine.x;

		//法線ベクトル単位化
		D3DXVec3Normalize(&vecNor, &vecNor);

		//交点計算
		posCross = vtx2 + (vecLine * fRate);
		vecMove = *pos - posCross;
		vecMove.y = 0.0f;
		posCross.y = 0.0f;

		//水平方向大きさ
		fDot = D3DXVec3Dot(&-vecMove, &vecNor);

		*pos = posCross + (vecMove + (vecNor * fDot));

		return true;
	}

	return false;
}

//========================================
// 四角の下にいるかの判定
// 1から時計回りに代入してください
//========================================
bool IsOnSquare(D3DXVECTOR3 vtx1, D3DXVECTOR3 vtx2, D3DXVECTOR3 vtx3, D3DXVECTOR3 vtx4, D3DXVECTOR3 vtxNor, D3DXVECTOR3 posTarget, D3DXVECTOR3 posOldTarget, float& rHeight)
{
	D3DXVECTOR3 vecP, vecTemp;
	float fHeight, fDot;

	// ポリゴンの外周ベクトルを設定
	D3DXVECTOR3 vecVtx[4];
	vecVtx[0] = vtx2 - vtx1;
	vecVtx[1] = vtx3 - vtx2;
	vecVtx[2] = vtx4 - vtx3;
	vecVtx[3] = vtx1 - vtx4;

	// ポリゴンの各点から目標へのベクトルを設定
	D3DXVECTOR3 vecVtxTarget[4];
	vecVtxTarget[0] = posTarget - vtx1;
	vecVtxTarget[1] = posTarget - vtx2;
	vecVtxTarget[2] = posTarget - vtx3;
	vecVtxTarget[3] = posTarget - vtx4;

	// 目標位置がポリゴンの内側にいるか判定
	if (D3DXVec3Cross(&vecTemp, &vecVtxTarget[0], &vecVtx[0])->y < 0 &&
		D3DXVec3Cross(&vecTemp, &vecVtxTarget[1], &vecVtx[1])->y < 0 &&
		D3DXVec3Cross(&vecTemp, &vecVtxTarget[2], &vecVtx[2])->y < 0 &&
		D3DXVec3Cross(&vecTemp, &vecVtxTarget[3], &vecVtx[3])->y < 0)
	{
		// y軸法線が0ではないか判定
		if (!vtxNor.y)
			return false;

		// 内積を計算
		fDot = (vtxNor.x * vecVtxTarget[0].x) + (vtxNor.z * vecVtxTarget[0].z);

		// 内積を用いて高さを計算
		fHeight = -(fDot / vtxNor.y) + vtx1.y;

		// 高さが目標位置より高いか判定
		if (fHeight > posTarget.y)
		{
			// 高さ代入
			rHeight = fHeight;
			return true;
		}
	}

	return false;
}

//========================================
// OBBの平面に対する押し戻し判定処理
//========================================
D3DXVECTOR3 CollideOBBToPlane(D3DXVECTOR3* posOBB, D3DXVECTOR3 vecOBBAxial, D3DXVECTOR3 posPlane, D3DXVECTOR3 vecNorPlane)
{
	// 各方向軸ベクトル計算
	D3DXVECTOR3 axis1 = D3DXVECTOR3(vecOBBAxial.x, 0.0f, 0.0f);
	D3DXVECTOR3 axis2 = D3DXVECTOR3(0.0f, vecOBBAxial.y, 0.0f);
	D3DXVECTOR3 axis3 = D3DXVECTOR3(0.0f, 0.0f, vecOBBAxial.z);

	vecNorPlane.y = 0.0f;

	// 射影線の長さを計算
	float lenProjection = lengthAxis(vecNorPlane, axis1, axis2, axis3);

	// 線分とターゲットの位置関係を計算
	float lenPos = D3DXVec3Dot(&(*posOBB - posPlane), &vecNorPlane);

	// めり込んでいる
	if (lenProjection < fabs(lenPos))
		return D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	// めり込み具合で戻す距離を変える
	if (lenPos > 0.0f)
		return vecNorPlane * (lenProjection - fabs(lenPos));
	else
		return vecNorPlane * (lenProjection + fabs(lenPos));
}

//==================================================================================================
//点と箱の当たり判定
//==================================================================================================
bool CollidePointToOBB(D3DXVECTOR3* posO, D3DXVECTOR3 posOldO, D3DXVECTOR3 posV, D3DXVECTOR3 rotV, D3DXVECTOR3 sizeV)
{
	D3DXVECTOR3 posCorner[8] = {};
	D3DXVECTOR3 posPlaneCenter[6] = {};
	D3DXVECTOR3 vecIntersect = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vecNorPlaneCenter = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXPLANE plane = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	int nCheckCollision = 0;

	//箱の各面の中心を求める
	posPlaneCenter[0] = PosRelativeMtx(posV, rotV, D3DXVECTOR3(sizeV.x, 0.0f, 0.0f));
	posPlaneCenter[1] = PosRelativeMtx(posV, rotV, D3DXVECTOR3(-sizeV.x, 0.0f, 0.0f));
	posPlaneCenter[2] = PosRelativeMtx(posV, rotV, D3DXVECTOR3(0.0f, sizeV.y, 0.0f));
	posPlaneCenter[3] = PosRelativeMtx(posV, rotV, D3DXVECTOR3(0.0f, -sizeV.y, 0.0f));
	posPlaneCenter[4] = PosRelativeMtx(posV, rotV, D3DXVECTOR3(0.0f, 0.0f, sizeV.z));
	posPlaneCenter[5] = PosRelativeMtx(posV, rotV, D3DXVECTOR3(0.0f, 0.0f, -sizeV.z));

	for (int nCnt = 0; nCnt < 6; nCnt++)
	{
		//各面の法線ベクトルを計算する
		vecNorPlaneCenter = posV - posPlaneCenter[nCnt];
		D3DXVec3Normalize(&vecNorPlaneCenter, &vecNorPlaneCenter);

		//法線ベクトルから平面の式を計算する
		D3DXPlaneFromPointNormal(&plane, &posPlaneCenter[nCnt], &vecNorPlaneCenter);

		//平面の式と点から
		if (D3DXPlaneDotCoord(&plane, posO) < 0.0f || D3DXPlaneDotCoord(&plane, &posOldO) > 0.0f)
		{
			continue;
		}

		// 衝突地点を計算
		D3DXPlaneIntersectLine(&vecIntersect, &plane, posO, &posOldO);

		for (int nCnt = 0; nCnt < 6; nCnt++)
		{
			//各面の法線ベクトルを計算する
			D3DXVECTOR3 vecNorPlaneCenterDest = posV - posPlaneCenter[nCnt];
			D3DXVec3Normalize(&vecNorPlaneCenterDest, &vecNorPlaneCenterDest);

			//法線ベクトルから平面の式を計算する
			D3DXPlaneFromPointNormal(&plane, &posPlaneCenter[nCnt], &vecNorPlaneCenterDest);

			//平面の式と点から
			if (D3DXPlaneDotCoord(&plane, &vecIntersect) >= 0.0f)
			{
				nCheckCollision++;
			}
		}

		// 箱の中にいるか判定
		if (nCheckCollision != 6)
		{
			continue;
		}

		// 計算用変数
		D3DXVECTOR3 vecMove, vecMoveRef;
		float fDot;

		// 移動量計算
		vecMove = vecIntersect - *posO;

		// 押し戻し距離計算
		fDot = D3DXVec3Dot(&vecMove, &vecNorPlaneCenter);
		vecMove = *posO - vecIntersect;
		vecMoveRef = vecMove + (vecNorPlaneCenter * fDot * 1.0f);

		// 押し戻し距離代入
		*posO = vecIntersect + vecMoveRef;

		return true;
	}

	return false;
}

//========================================
// OBBとOBBの衝突判定処理
//========================================
bool CollideOBBToOBBTrigger(D3DXVECTOR3 posO, D3DXVECTOR3 rotO, D3DXVECTOR3 sizeO, D3DXVECTOR3 posV, D3DXVECTOR3 rotV, D3DXVECTOR3 sizeV)
{
	// 分離軸
	D3DXVECTOR3 axisA1, axisNA1;
	D3DXVECTOR3 axisA2, axisNA2;
	D3DXVECTOR3 axisA3, axisNA3; 
	D3DXVECTOR3 axisB1, axisNB1;
	D3DXVECTOR3 axisB2, axisNB2;
	D3DXVECTOR3 axisB3, axisNB3;

	// 1つ目のオブジェクトの分離軸計算
	axisA1 = PosRelativeMtx(D3DXVECTOR3(0.0f, 0.0f, 0.0f), rotO, D3DXVECTOR3(sizeO.x, 0.0f, 0.0f));
	axisA2 = PosRelativeMtx(D3DXVECTOR3(0.0f, 0.0f, 0.0f), rotO, D3DXVECTOR3(0.0f, sizeO.y, 0.0f));
	axisA3 = PosRelativeMtx(D3DXVECTOR3(0.0f, 0.0f, 0.0f), rotO, D3DXVECTOR3(0.0f, 0.0f, sizeO.z));
	D3DXVec3Normalize(&axisNA1, &axisA1);
	D3DXVec3Normalize(&axisNA2, &axisA2);
	D3DXVec3Normalize(&axisNA3, &axisA3);

	// 2つ目のオブジェクトの分離軸計算
	axisB1 = PosRelativeMtx(D3DXVECTOR3(0.0f, 0.0f, 0.0f), rotV, D3DXVECTOR3(sizeV.x, 0.0f, 0.0f));
	axisB2 = PosRelativeMtx(D3DXVECTOR3(0.0f, 0.0f, 0.0f), rotV, D3DXVECTOR3(0.0f, sizeV.y, 0.0f));
	axisB3 = PosRelativeMtx(D3DXVECTOR3(0.0f, 0.0f, 0.0f), rotV, D3DXVECTOR3(0.0f, 0.0f, sizeV.z));
	D3DXVec3Normalize(&axisNB1, &axisB1);
	D3DXVec3Normalize(&axisNB2, &axisB2);
	D3DXVec3Normalize(&axisNB3, &axisB3);

	// 2つのオブジェクト間の距離計算用ベクトル
	D3DXVECTOR3 lengthCollider = posO - posV;

	float rA, rB, length;
	D3DXVECTOR3 Cross;

	// 分離軸判定=========================================
	// A1
	rA = D3DXVec3Length(&axisA1);
	rB = lengthAxis(axisNA1, axisB1, axisB2, axisB3);
	length = fabs(D3DXVec3Dot(&lengthCollider, &axisNA1));
	if (length > rA + rB)
	{
		return false;
	}

	// A2
	rA = D3DXVec3Length(&axisA2);
	rB = lengthAxis(axisNA2, axisB1, axisB2, axisB3);
	length = fabs(D3DXVec3Dot(&lengthCollider, &axisNA2));
	if (length > rA + rB)
	{
		return false;
	}

	// A3
	rA = D3DXVec3Length(&axisA3);
	rB = lengthAxis(axisNA2, axisB1, axisB2, axisB3);
	length = fabs(D3DXVec3Dot(&lengthCollider, &axisNA3));
	if (length > rA + rB)
	{
		return false;
	}

	// B1
	rA = D3DXVec3Length(&axisB1);
	rB = lengthAxis(axisNB1, axisA1, axisA2, axisA3);
	length = fabs(D3DXVec3Dot(&lengthCollider, &axisNB1));
	if (length > rA + rB)
	{
		return false;
	}

	// B2
	rA = D3DXVec3Length(&axisB2);
	rB = lengthAxis(axisNB2, axisA1, axisA2, axisA3);
	length = fabs(D3DXVec3Dot(&lengthCollider, &axisNB2));
	if (length > rA + rB)
	{
		return false;
	}

	// B3
	rA = D3DXVec3Length(&axisB3);
	rB = lengthAxis(axisNB3, axisA1, axisA2, axisA3);
	length = fabs(D3DXVec3Dot(&lengthCollider, &axisNB3));
	if (length > rA + rB)
	{
		return false;
	}

	// C11
	D3DXVec3Cross(&Cross, &axisNA1, &axisNB1);
	rA = lengthAxis(Cross, axisA2, axisA3, D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	rB = lengthAxis(Cross, axisB2, axisB3, D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	length = fabs(D3DXVec3Dot(&lengthCollider, &Cross));
	if (length > rA + rB)
	{
		return false;
	}

	// C12
	D3DXVec3Cross(&Cross, &axisNA1, &axisNB2);
	rA = lengthAxis(Cross, axisA2, axisA3, D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	rB = lengthAxis(Cross, axisB1, axisB3, D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	length = fabs(D3DXVec3Dot(&lengthCollider, &Cross));
	if (length > rA + rB)
	{
		return false;
	}

	// C13
	D3DXVec3Cross(&Cross, &axisNA1, &axisNB3);
	rA = lengthAxis(Cross, axisA2, axisA3, D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	rB = lengthAxis(Cross, axisB1, axisB2, D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	length = fabs(D3DXVec3Dot(&lengthCollider, &Cross));
	if (length > rA + rB)
	{
		return false;
	}

	// C21
	D3DXVec3Cross(&Cross, &axisNA2, &axisNB1);
	rA = lengthAxis(Cross, axisA1, axisA3, D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	rB = lengthAxis(Cross, axisB2, axisB3, D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	length = fabs(D3DXVec3Dot(&lengthCollider, &Cross));
	if (length > rA + rB)
	{
		return false;
	}

	// C22
	D3DXVec3Cross(&Cross, &axisNA2, &axisNB2);
	rA = lengthAxis(Cross, axisA1, axisA3, D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	rB = lengthAxis(Cross, axisB1, axisB3, D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	length = fabs(D3DXVec3Dot(&lengthCollider, &Cross));
	if (length > rA + rB)
	{
		return false;
	}

	// C23
	D3DXVec3Cross(&Cross, &axisNA3, &axisNB3);
	rA = lengthAxis(Cross, axisA1, axisA2, D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	rB = lengthAxis(Cross, axisB1, axisB2, D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	length = fabs(D3DXVec3Dot(&lengthCollider, &Cross));
	if (length > rA + rB)
	{
		return false;
	}

	// C31
	D3DXVec3Cross(&Cross, &axisNA3, &axisNB1);
	rA = lengthAxis(Cross, axisA1, axisA2, D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	rB = lengthAxis(Cross, axisB2, axisB3, D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	length = fabs(D3DXVec3Dot(&lengthCollider, &Cross));
	if (length > rA + rB)
	{
		return false;
	}

	// C32
	D3DXVec3Cross(&Cross, &axisNA3, &axisNB2);
	rA = lengthAxis(Cross, axisA1, axisA2, D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	rB = lengthAxis(Cross, axisB1, axisB3, D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	length = fabs(D3DXVec3Dot(&lengthCollider, &Cross));
	if (length > rA + rB)
	{
		return false;
	}

	// C33
	D3DXVec3Cross(&Cross, &axisNA2, &axisNB3);
	rA = lengthAxis(Cross, axisA1, axisA3, D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	rB = lengthAxis(Cross, axisB1, axisB2, D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	length = fabs(D3DXVec3Dot(&lengthCollider, &Cross));
	if (length > rA + rB)
	{
		return false;
	}

	return true;
}

//========================================
// RayとOBBの衝突判定処理
//========================================
bool CollideRayToOBB(D3DXVECTOR3* pOut, D3DXVECTOR3 posO, D3DXVECTOR3 vecO, D3DXVECTOR3 posV, D3DXVECTOR3 rotV, D3DXVECTOR3 sizeMaxV, D3DXVECTOR3 sizeMinV)
{
	D3DXVECTOR3 posPlaneCenter[6] = {};
	D3DXVECTOR3 vecIntersect = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vecNorPlaneCenter = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 sizeV = (sizeMaxV - sizeMinV) * 0.5f;
	D3DXPLANE plane = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	int nCheckCollision = 0;

	//箱の各面の中心を求める
	posPlaneCenter[0] = PosRelativeMtx(posV, rotV, D3DXVECTOR3(sizeV.x, 0.0f, 0.0f));
	posPlaneCenter[1] = PosRelativeMtx(posV, rotV, D3DXVECTOR3(-sizeV.x, 0.0f, 0.0f));
	posPlaneCenter[2] = PosRelativeMtx(posV, rotV, D3DXVECTOR3(0.0f, sizeV.y, 0.0f));
	posPlaneCenter[3] = PosRelativeMtx(posV, rotV, D3DXVECTOR3(0.0f, -sizeV.y, 0.0f));
	posPlaneCenter[4] = PosRelativeMtx(posV, rotV, D3DXVECTOR3(0.0f, 0.0f, sizeV.z));
	posPlaneCenter[5] = PosRelativeMtx(posV, rotV, D3DXVECTOR3(0.0f, 0.0f, -sizeV.z));

	for (int nCnt = 0; nCnt < 6; nCnt++)
	{
		//各面の法線ベクトルを計算する
		vecNorPlaneCenter = posV - posPlaneCenter[nCnt];
		D3DXVec3Normalize(&vecNorPlaneCenter, &vecNorPlaneCenter);

		//法線ベクトルから平面の式を計算する
		D3DXPlaneFromPointNormal(&plane, &posPlaneCenter[nCnt], &vecNorPlaneCenter);

		// 衝突地点を計算
		if (D3DXPlaneIntersectLine(&vecIntersect, &plane, &posO, &(posO + vecO)) == nullptr)
		{
			continue;
		}

		for (int nCnt = 0; nCnt < 6; nCnt++)
		{
			//各面の法線ベクトルを計算する
			D3DXVECTOR3 vecNorPlaneCenterDest = posV - posPlaneCenter[nCnt];
			D3DXVec3Normalize(&vecNorPlaneCenterDest, &vecNorPlaneCenterDest);

			//法線ベクトルから平面の式を計算する
			D3DXPlaneFromPointNormal(&plane, &posPlaneCenter[nCnt], &vecNorPlaneCenterDest);

			//平面の式と点から
			if (D3DXPlaneDotCoord(&plane, &vecIntersect) >= 0.0f)
			{
				nCheckCollision++;
			}
		}

		// 箱の中にいるか判定
		if (nCheckCollision != 6)
		{
			continue;
		}

		*pOut = vecIntersect;
		return true;
	}

	return false;
}

//========================================
// 線分に対するの射影変換
//========================================
float lengthAxis(D3DXVECTOR3 separationAxis, D3DXVECTOR3 e1, D3DXVECTOR3 e2, D3DXVECTOR3 e3)
{
	float length1, length2, length3;

	length1 = fabs(D3DXVec3Dot(&separationAxis, &e1));
	length2 = fabs(D3DXVec3Dot(&separationAxis, &e2));
	length3 = fabs(D3DXVec3Dot(&separationAxis, &e3));

	return length1 + length2 + length3;
}

//========================================
//3次元空間での行列による回転座標変換関数
//(任意の点からのオフセット位置を角度と距離で変換)
//========================================
D3DXVECTOR3 PosRelativeMtx(D3DXVECTOR3 posO, D3DXVECTOR3 rot, D3DXVECTOR3 offset)
{
	D3DXVECTOR3 posAnswer;
	D3DXMATRIX mtxO, mtxAnswer;
	D3DXMATRIX mtxRot, mtxTrans;		//計算用マトリックス
	D3DXMATRIX mtxRotModel, mtxTransModel, mtxPalent;		//計算用マトリックス

	//パーツのワールドマトリックス初期化
	D3DXMatrixIdentity(&mtxO);

	//向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot,
		rot.y, rot.x, rot.z);
	D3DXMatrixMultiply(&mtxO, &mtxO, &mtxRot);

	//位置を反映
	D3DXMatrixTranslation(&mtxTransModel,
		posO.x, posO.y, posO.z);
	D3DXMatrixMultiply(&mtxO, &mtxO, &mtxTransModel);

	mtxPalent = mtxO;

	//パーツのワールドマトリックス初期化
	D3DXMatrixIdentity(&mtxAnswer);

	//向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot,
		3.14f, 3.14f, 3.14f);
	D3DXMatrixMultiply(&mtxO, &mtxO, &mtxRot);

	//位置を反映
	D3DXMatrixTranslation(&mtxTransModel,
		offset.x, offset.y, offset.z);
	D3DXMatrixMultiply(&mtxAnswer, &mtxAnswer, &mtxTransModel);

	//算出したパーツのワールドマトリックスと親のマトリックスをかけ合わせる
	D3DXMatrixMultiply(&mtxAnswer,
		&mtxAnswer,
		&mtxPalent);

	posAnswer = GetMtxPos(mtxAnswer);

	return posAnswer;
}

//========================================
//3次元空間での行列による回転座標変換からの法線計算関数
//========================================
D3DXVECTOR3 NorRelativeMtx(D3DXVECTOR3 posO, D3DXVECTOR3 rot, D3DXVECTOR3 offsetMax, D3DXVECTOR3 offsetMin)
{
	D3DXVECTOR3 posMax, posMin;
	D3DXVECTOR3 vecNor;

	posMax = PosRelativeMtx(posO, rot, D3DXVECTOR3(offsetMax.x, offsetMax.y, offsetMax.z));
	posMin = PosRelativeMtx(posO, rot, D3DXVECTOR3(offsetMax.x, offsetMin.y, offsetMax.z));

	vecNor = posMax - posMin;

	D3DXVec3Normalize(&vecNor, &vecNor);

	return vecNor;
}

//========================================
// マトリックスから位置の取得
//========================================
D3DXVECTOR3 GetMtxPos(D3DXMATRIX mtx)
{
	D3DXVECTOR3 pos =
	{
		mtx._41,
		mtx._42,
		mtx._43
	};

	return pos;
}
}