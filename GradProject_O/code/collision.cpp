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
D3DXVECTOR3 CollideOBBToPlane(D3DXVECTOR3* posOBB, D3DXVECTOR3 vecAxial, D3DXVECTOR3 posPlane, D3DXVECTOR3 vecNorPlane)
{
	// �e�������x�N�g���v�Z
	D3DXVECTOR3 axis1 = D3DXVECTOR3(vecAxial.x, 0.0f, 0.0f);
	D3DXVECTOR3 axis2 = D3DXVECTOR3(0.0f, vecAxial.y, 0.0f);
	D3DXVECTOR3 axis3 = D3DXVECTOR3(0.0f, 0.0f, vecAxial.z);

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