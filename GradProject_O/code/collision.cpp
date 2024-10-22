//==========================================================
//
// �����蔻��̔ėp���� [collision.cpp]
// Author : Riku Nakamura
//
//==========================================================
#include "collision.h"

namespace collision
{
//========================================
// �O�ς̌v�Z
//========================================
float CrossProduct(D3DXVECTOR3 pos1, D3DXVECTOR3 pos2)
{
	return pos1.z * pos2.x - pos1.x * pos2.z;
}

//========================================
// �O�p�`�̉��ɂ��邩�̔���
//========================================
bool IsOnTrianglePolygon(D3DXVECTOR3 vtx1, D3DXVECTOR3 vtx2, D3DXVECTOR3 vtx3, D3DXVECTOR3 vtxNor, D3DXVECTOR3 posTarget, float& rHeight)
{
	D3DXVECTOR3 vecP, vecTemp;
	float fHeight, fDot;

	// �|���S���̊O���x�N�g����ݒ�
	D3DXVECTOR3 vecVtx[3];
	vecVtx[0] = vtx2 - vtx1;
	vecVtx[1] = vtx3 - vtx2;
	vecVtx[2] = vtx1 - vtx3;

	if (D3DXVec3Length(&vecVtx[0]) <= 0 ||
		D3DXVec3Length(&vecVtx[1]) <= 0 ||
		D3DXVec3Length(&vecVtx[2]) <= 0)
		return false;

	// �|���S���̊e�_����ڕW�ւ̃x�N�g����ݒ�
	D3DXVECTOR3 vecVtxTarget[3];
	vecVtxTarget[0] = posTarget - vtx1;
	vecVtxTarget[1] = posTarget - vtx2;
	vecVtxTarget[2] = posTarget - vtx3;

	// �ڕW�ʒu���|���S���̓����ɂ��邩����
	if (D3DXVec3Cross(&vecTemp, &vecVtxTarget[0], &vecVtx[0])->y < 0 &&
		D3DXVec3Cross(&vecTemp, &vecVtxTarget[1], &vecVtx[1])->y < 0 &&
		D3DXVec3Cross(&vecTemp, &vecVtxTarget[2], &vecVtx[2])->y < 0)
	{
		// y���@����0�ł͂Ȃ�������
		if (!vtxNor.y)
			return false;

		// ���ς��v�Z
		fDot = (vtxNor.x * vecVtxTarget[0].x) + (vtxNor.z * vecVtxTarget[0].z);

		// ���ς�p���č������v�Z
		fHeight = -(fDot / vtxNor.y) + vtx1.y;

		// �������ڕW�ʒu��荂��������
		if (fHeight >= rHeight)
		{
			// �������
			rHeight = fHeight;
			return true;
		}
	}

	return false;
}

//========================================
// �|���S���̉��ɂ��邩�̔���
//========================================
bool IsOnSquarePolygon(D3DXVECTOR3 vtx1, D3DXVECTOR3 vtx2, D3DXVECTOR3 vtx3, D3DXVECTOR3 vtx4, D3DXVECTOR3 vtxNor1, D3DXVECTOR3 vtxNor2, D3DXVECTOR3 posTarget, D3DXVECTOR3 posOldTarget, float& rHeight)
{
	bool bColllision = false;

	// 1�ڂ̃|���S���Ɠ��O����
	if (IsOnTrianglePolygon(vtx1, vtx2, vtx3, vtxNor1, posTarget, rHeight))
		bColllision = true;

	// 2�ڂ̃|���S���Ɠ��O����
	if (IsOnTrianglePolygon(vtx4, vtx3, vtx2, vtxNor2, posTarget, rHeight))
		bColllision = true;

	return bColllision;
}

//========================================
// ���ɑ΂���O�ς̉����߂�����
//========================================
bool LineCrossProduct(D3DXVECTOR3 vtx1, D3DXVECTOR3 vtx2, D3DXVECTOR3* pos, D3DXVECTOR3 posOld)
{
	D3DXVECTOR3 vecMove, vecLine, vecToPos, vecToPosOld;
	float fRate, fOutUnit, fOutRate;

	// �p�x�N�g���v�Z
	vecMove = *pos - posOld;
	vecLine = vtx1 - vtx2;
	vecToPos = *pos - vtx2;
	vecToPosOld = posOld - vtx2;

	// y������0�ɂ���
	vecMove.y = 0.0f;
	vecLine.y = 0.0f;
	vecToPos.y = 0.0f;
	vecToPosOld.y = 0.0f;

	// ��_�{���v�Z
	fOutUnit = CrossProduct(vecLine, vecMove);
	fOutRate = CrossProduct(vecToPos, vecMove);
	fRate = fOutRate / fOutUnit;

	// ��������܂����ł��邩����
	if (CrossProduct(vecLine, vecToPos) > 0.0f &&
		CrossProduct(vecLine, vecToPosOld) <= 0.0f &&
		fRate >= 0.0f && fRate < 1.0f)
	{
		D3DXVECTOR3 vecNor, posCross;
		float fDot;

		//�@���x�N�g���v�Z
		vecNor.x = -vecLine.z;
		vecNor.y = 0.0f;
		vecNor.z = vecLine.x;

		//�@���x�N�g���P�ʉ�
		D3DXVec3Normalize(&vecNor, &vecNor);

		//��_�v�Z
		posCross = vtx2 + (vecLine * fRate);
		vecMove = *pos - posCross;
		vecMove.y = 0.0f;
		posCross.y = 0.0f;

		//���������傫��
		fDot = D3DXVec3Dot(&-vecMove, &vecNor);

		*pos = posCross + (vecMove + (vecNor * fDot));

		return true;
	}

	return false;
}

//========================================
// �l�p�̉��ɂ��邩�̔���
// 1���玞�v���ɑ�����Ă�������
//========================================
bool IsOnSquare(D3DXVECTOR3 vtx1, D3DXVECTOR3 vtx2, D3DXVECTOR3 vtx3, D3DXVECTOR3 vtx4, D3DXVECTOR3 vtxNor, D3DXVECTOR3 posTarget, D3DXVECTOR3 posOldTarget, float& rHeight)
{
	D3DXVECTOR3 vecP, vecTemp;
	float fHeight, fDot;

	// �|���S���̊O���x�N�g����ݒ�
	D3DXVECTOR3 vecVtx[4];
	vecVtx[0] = vtx2 - vtx1;
	vecVtx[1] = vtx3 - vtx2;
	vecVtx[2] = vtx4 - vtx3;
	vecVtx[3] = vtx1 - vtx4;

	// �|���S���̊e�_����ڕW�ւ̃x�N�g����ݒ�
	D3DXVECTOR3 vecVtxTarget[4];
	vecVtxTarget[0] = posTarget - vtx1;
	vecVtxTarget[1] = posTarget - vtx2;
	vecVtxTarget[2] = posTarget - vtx3;
	vecVtxTarget[3] = posTarget - vtx4;

	// �ڕW�ʒu���|���S���̓����ɂ��邩����
	if (D3DXVec3Cross(&vecTemp, &vecVtxTarget[0], &vecVtx[0])->y < 0 &&
		D3DXVec3Cross(&vecTemp, &vecVtxTarget[1], &vecVtx[1])->y < 0 &&
		D3DXVec3Cross(&vecTemp, &vecVtxTarget[2], &vecVtx[2])->y < 0 &&
		D3DXVec3Cross(&vecTemp, &vecVtxTarget[3], &vecVtx[3])->y < 0)
	{
		// y���@����0�ł͂Ȃ�������
		if (!vtxNor.y)
			return false;

		// ���ς��v�Z
		fDot = (vtxNor.x * vecVtxTarget[0].x) + (vtxNor.z * vecVtxTarget[0].z);

		// ���ς�p���č������v�Z
		fHeight = -(fDot / vtxNor.y) + vtx1.y;

		// �������ڕW�ʒu��荂��������
		if (fHeight > posTarget.y)
		{
			// �������
			rHeight = fHeight;
			return true;
		}
	}

	return false;
}

//========================================
// OBB�̕��ʂɑ΂��鉟���߂����菈��
//========================================
D3DXVECTOR3 CollideOBBToPlane(D3DXVECTOR3* posOBB, D3DXVECTOR3 vecOBBAxial, D3DXVECTOR3 posPlane, D3DXVECTOR3 vecNorPlane)
{
	// �e�������x�N�g���v�Z
	D3DXVECTOR3 axis1 = D3DXVECTOR3(vecOBBAxial.x, 0.0f, 0.0f);
	D3DXVECTOR3 axis2 = D3DXVECTOR3(0.0f, vecOBBAxial.y, 0.0f);
	D3DXVECTOR3 axis3 = D3DXVECTOR3(0.0f, 0.0f, vecOBBAxial.z);

	vecNorPlane.y = 0.0f;

	// �ˉe���̒������v�Z
	float lenProjection = lengthAxis(vecNorPlane, axis1, axis2, axis3);

	// �����ƃ^�[�Q�b�g�̈ʒu�֌W���v�Z
	float lenPos = D3DXVec3Dot(&(*posOBB - posPlane), &vecNorPlane);

	// �߂荞��ł���
	if (lenProjection < fabs(lenPos))
		return D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	// �߂荞�݋�Ŗ߂�������ς���
	if (lenPos > 0.0f)
		return vecNorPlane * (lenProjection - fabs(lenPos));
	else
		return vecNorPlane * (lenProjection + fabs(lenPos));
}

//==================================================================================================
//�_�Ɣ��̓����蔻��
//==================================================================================================
bool CollidePointToOBB(D3DXVECTOR3* posO, D3DXVECTOR3 posOldO, D3DXVECTOR3 posV, D3DXVECTOR3 rotV, D3DXVECTOR3 sizeV)
{
	D3DXVECTOR3 posCorner[8] = {};
	D3DXVECTOR3 posPlaneCenter[6] = {};
	D3DXVECTOR3 vecIntersect = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vecNorPlaneCenter = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXPLANE plane = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	int nCheckCollision = 0;

	//���̊e�ʂ̒��S�����߂�
	posPlaneCenter[0] = PosRelativeMtx(posV, rotV, D3DXVECTOR3(sizeV.x, 0.0f, 0.0f));
	posPlaneCenter[1] = PosRelativeMtx(posV, rotV, D3DXVECTOR3(-sizeV.x, 0.0f, 0.0f));
	posPlaneCenter[2] = PosRelativeMtx(posV, rotV, D3DXVECTOR3(0.0f, sizeV.y, 0.0f));
	posPlaneCenter[3] = PosRelativeMtx(posV, rotV, D3DXVECTOR3(0.0f, -sizeV.y, 0.0f));
	posPlaneCenter[4] = PosRelativeMtx(posV, rotV, D3DXVECTOR3(0.0f, 0.0f, sizeV.z));
	posPlaneCenter[5] = PosRelativeMtx(posV, rotV, D3DXVECTOR3(0.0f, 0.0f, -sizeV.z));

	for (int nCnt = 0; nCnt < 6; nCnt++)
	{
		//�e�ʂ̖@���x�N�g�����v�Z����
		vecNorPlaneCenter = posV - posPlaneCenter[nCnt];
		D3DXVec3Normalize(&vecNorPlaneCenter, &vecNorPlaneCenter);

		//�@���x�N�g�����畽�ʂ̎����v�Z����
		D3DXPlaneFromPointNormal(&plane, &posPlaneCenter[nCnt], &vecNorPlaneCenter);

		//���ʂ̎��Ɠ_����
		if (D3DXPlaneDotCoord(&plane, posO) < 0.0f || D3DXPlaneDotCoord(&plane, &posOldO) > 0.0f)
		{
			continue;
		}

		// �Փ˒n�_���v�Z
		D3DXPlaneIntersectLine(&vecIntersect, &plane, posO, &posOldO);

		for (int nCnt = 0; nCnt < 6; nCnt++)
		{
			//�e�ʂ̖@���x�N�g�����v�Z����
			D3DXVECTOR3 vecNorPlaneCenterDest = posV - posPlaneCenter[nCnt];
			D3DXVec3Normalize(&vecNorPlaneCenterDest, &vecNorPlaneCenterDest);

			//�@���x�N�g�����畽�ʂ̎����v�Z����
			D3DXPlaneFromPointNormal(&plane, &posPlaneCenter[nCnt], &vecNorPlaneCenterDest);

			//���ʂ̎��Ɠ_����
			if (D3DXPlaneDotCoord(&plane, &vecIntersect) >= 0.0f)
			{
				nCheckCollision++;
			}
		}

		// ���̒��ɂ��邩����
		if (nCheckCollision != 6)
		{
			continue;
		}

		// �v�Z�p�ϐ�
		D3DXVECTOR3 vecMove, vecMoveRef;
		float fDot;

		// �ړ��ʌv�Z
		vecMove = vecIntersect - *posO;

		// �����߂������v�Z
		fDot = D3DXVec3Dot(&vecMove, &vecNorPlaneCenter);
		vecMove = *posO - vecIntersect;
		vecMoveRef = vecMove + (vecNorPlaneCenter * fDot * 1.0f);

		// �����߂��������
		*posO = vecIntersect + vecMoveRef;

		return true;
	}

	return false;
}

//========================================
// OBB��OBB�̏Փ˔��菈��
//========================================
bool CollideOBBToOBBTrigger(D3DXVECTOR3 posO, D3DXVECTOR3 rotO, D3DXVECTOR3 sizeO, D3DXVECTOR3 posV, D3DXVECTOR3 rotV, D3DXVECTOR3 sizeV)
{
	// ������
	D3DXVECTOR3 axisA1, axisNA1;
	D3DXVECTOR3 axisA2, axisNA2;
	D3DXVECTOR3 axisA3, axisNA3; 
	D3DXVECTOR3 axisB1, axisNB1;
	D3DXVECTOR3 axisB2, axisNB2;
	D3DXVECTOR3 axisB3, axisNB3;

	// 1�ڂ̃I�u�W�F�N�g�̕������v�Z
	axisA1 = PosRelativeMtx(D3DXVECTOR3(0.0f, 0.0f, 0.0f), rotO, D3DXVECTOR3(sizeO.x, 0.0f, 0.0f));
	axisA2 = PosRelativeMtx(D3DXVECTOR3(0.0f, 0.0f, 0.0f), rotO, D3DXVECTOR3(0.0f, sizeO.y, 0.0f));
	axisA3 = PosRelativeMtx(D3DXVECTOR3(0.0f, 0.0f, 0.0f), rotO, D3DXVECTOR3(0.0f, 0.0f, sizeO.z));
	D3DXVec3Normalize(&axisNA1, &axisA1);
	D3DXVec3Normalize(&axisNA2, &axisA2);
	D3DXVec3Normalize(&axisNA3, &axisA3);

	// 2�ڂ̃I�u�W�F�N�g�̕������v�Z
	axisB1 = PosRelativeMtx(D3DXVECTOR3(0.0f, 0.0f, 0.0f), rotV, D3DXVECTOR3(sizeV.x, 0.0f, 0.0f));
	axisB2 = PosRelativeMtx(D3DXVECTOR3(0.0f, 0.0f, 0.0f), rotV, D3DXVECTOR3(0.0f, sizeV.y, 0.0f));
	axisB3 = PosRelativeMtx(D3DXVECTOR3(0.0f, 0.0f, 0.0f), rotV, D3DXVECTOR3(0.0f, 0.0f, sizeV.z));
	D3DXVec3Normalize(&axisNB1, &axisB1);
	D3DXVec3Normalize(&axisNB2, &axisB2);
	D3DXVec3Normalize(&axisNB3, &axisB3);

	// 2�̃I�u�W�F�N�g�Ԃ̋����v�Z�p�x�N�g��
	D3DXVECTOR3 lengthCollider = posO - posV;

	float rA, rB, length;
	D3DXVECTOR3 Cross;

	// ����������=========================================
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
// Ray��OBB�̏Փ˔��菈��
//========================================
bool CollideRayToOBB(D3DXVECTOR3* pOut, D3DXVECTOR3 posO, D3DXVECTOR3 vecO, D3DXVECTOR3 posV, D3DXVECTOR3 rotV, D3DXVECTOR3 sizeMaxV, D3DXVECTOR3 sizeMinV)
{
	D3DXVECTOR3 posPlaneCenter[6] = {};
	D3DXVECTOR3 vecIntersect = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vecNorPlaneCenter = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 sizeV = (sizeMaxV - sizeMinV) * 0.5f;
	D3DXPLANE plane = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	int nCheckCollision = 0;

	//���̊e�ʂ̒��S�����߂�
	posPlaneCenter[0] = PosRelativeMtx(posV, rotV, D3DXVECTOR3(sizeV.x, 0.0f, 0.0f));
	posPlaneCenter[1] = PosRelativeMtx(posV, rotV, D3DXVECTOR3(-sizeV.x, 0.0f, 0.0f));
	posPlaneCenter[2] = PosRelativeMtx(posV, rotV, D3DXVECTOR3(0.0f, sizeV.y, 0.0f));
	posPlaneCenter[3] = PosRelativeMtx(posV, rotV, D3DXVECTOR3(0.0f, -sizeV.y, 0.0f));
	posPlaneCenter[4] = PosRelativeMtx(posV, rotV, D3DXVECTOR3(0.0f, 0.0f, sizeV.z));
	posPlaneCenter[5] = PosRelativeMtx(posV, rotV, D3DXVECTOR3(0.0f, 0.0f, -sizeV.z));

	for (int nCnt = 0; nCnt < 6; nCnt++)
	{
		//�e�ʂ̖@���x�N�g�����v�Z����
		vecNorPlaneCenter = posV - posPlaneCenter[nCnt];
		D3DXVec3Normalize(&vecNorPlaneCenter, &vecNorPlaneCenter);

		//�@���x�N�g�����畽�ʂ̎����v�Z����
		D3DXPlaneFromPointNormal(&plane, &posPlaneCenter[nCnt], &vecNorPlaneCenter);

		// �Փ˒n�_���v�Z
		if (D3DXPlaneIntersectLine(&vecIntersect, &plane, &posO, &(posO + vecO)) == nullptr)
		{
			continue;
		}

		for (int nCnt = 0; nCnt < 6; nCnt++)
		{
			//�e�ʂ̖@���x�N�g�����v�Z����
			D3DXVECTOR3 vecNorPlaneCenterDest = posV - posPlaneCenter[nCnt];
			D3DXVec3Normalize(&vecNorPlaneCenterDest, &vecNorPlaneCenterDest);

			//�@���x�N�g�����畽�ʂ̎����v�Z����
			D3DXPlaneFromPointNormal(&plane, &posPlaneCenter[nCnt], &vecNorPlaneCenterDest);

			//���ʂ̎��Ɠ_����
			if (D3DXPlaneDotCoord(&plane, &vecIntersect) >= 0.0f)
			{
				nCheckCollision++;
			}
		}

		// ���̒��ɂ��邩����
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
// �����ɑ΂���̎ˉe�ϊ�
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
//3������Ԃł̍s��ɂ���]���W�ϊ��֐�
//(�C�ӂ̓_����̃I�t�Z�b�g�ʒu���p�x�Ƌ����ŕϊ�)
//========================================
D3DXVECTOR3 PosRelativeMtx(D3DXVECTOR3 posO, D3DXVECTOR3 rot, D3DXVECTOR3 offset)
{
	D3DXVECTOR3 posAnswer;
	D3DXMATRIX mtxO, mtxAnswer;
	D3DXMATRIX mtxRot, mtxTrans;		//�v�Z�p�}�g���b�N�X
	D3DXMATRIX mtxRotModel, mtxTransModel, mtxPalent;		//�v�Z�p�}�g���b�N�X

	//�p�[�c�̃��[���h�}�g���b�N�X������
	D3DXMatrixIdentity(&mtxO);

	//�����𔽉f
	D3DXMatrixRotationYawPitchRoll(&mtxRot,
		rot.y, rot.x, rot.z);
	D3DXMatrixMultiply(&mtxO, &mtxO, &mtxRot);

	//�ʒu�𔽉f
	D3DXMatrixTranslation(&mtxTransModel,
		posO.x, posO.y, posO.z);
	D3DXMatrixMultiply(&mtxO, &mtxO, &mtxTransModel);

	mtxPalent = mtxO;

	//�p�[�c�̃��[���h�}�g���b�N�X������
	D3DXMatrixIdentity(&mtxAnswer);

	//�����𔽉f
	D3DXMatrixRotationYawPitchRoll(&mtxRot,
		3.14f, 3.14f, 3.14f);
	D3DXMatrixMultiply(&mtxO, &mtxO, &mtxRot);

	//�ʒu�𔽉f
	D3DXMatrixTranslation(&mtxTransModel,
		offset.x, offset.y, offset.z);
	D3DXMatrixMultiply(&mtxAnswer, &mtxAnswer, &mtxTransModel);

	//�Z�o�����p�[�c�̃��[���h�}�g���b�N�X�Ɛe�̃}�g���b�N�X���������킹��
	D3DXMatrixMultiply(&mtxAnswer,
		&mtxAnswer,
		&mtxPalent);

	posAnswer = GetMtxPos(mtxAnswer);

	return posAnswer;
}

//========================================
//3������Ԃł̍s��ɂ���]���W�ϊ�����̖@���v�Z�֐�
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
// �}�g���b�N�X����ʒu�̎擾
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