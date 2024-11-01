//==========================================================
//
// �����蔻��̔ėp���� [collision.h]
// Author : Riku Nakamura
//
//==========================================================
#ifndef _COLLISION_H_
#define _COLLISION_H_

#include "main.h"		//main.h�Œ�`���Ă�����̂��K�v�Ȃ���include

//==========================================================
// �N���X�̒�`
//==========================================================
namespace collision
{
	/**
	@brief	�_�ƎO�p�`�|���S���̏Փ˔���
	@param	vtx1		[in]	�|���S���̒��_
	@param	vtx2		[in]	�|���S���̒��_
	@param	vtx3		[in]	�|���S���̒��_
	@param	vtxNor		[in]	�|���S���̖@���x�N�g��
	@param	posTarget	[in]	���肷��ʒu
	@param	rHeight		[out]	�߂�����
	@return	bool
	 */
	bool IsOnTrianglePolygon(D3DXVECTOR3 vtx1, D3DXVECTOR3 vtx2, D3DXVECTOR3 vtx3, D3DXVECTOR3 vtxNor, D3DXVECTOR3 posTarget, float& rHeight);

	/**
	@brief	�_�ƃ|���S���̏Փ˔���
	@param	vtx1			[in]	�|���S���̒��_
	@param	vtx2			[in]	�|���S���̒��_
	@param	vtx3			[in]	�|���S���̒��_
	@param	vtx4			[in]	�|���S���̒��_
	@param	vtxNor1			[in]	1�߂̃|���S���̖@���x�N�g��
	@param	vtxNor2			[in]	2�߂̃|���S���̖@���x�N�g��
	@param	posTarget		[in]	���肷��ʒu
	@param	posTargetOld	[in]	���肷��ߋ��ʒu
	@param	rHeight			[out]	�߂�����
	@return	bool
	 */
	bool IsOnSquarePolygon(D3DXVECTOR3 vtx1, D3DXVECTOR3 vtx2, D3DXVECTOR3 vtx3, D3DXVECTOR3 vtx4, D3DXVECTOR3 vtxNor1, D3DXVECTOR3 vtxNor2, D3DXVECTOR3 posTarget, D3DXVECTOR3 posOldTarget, float& rHeight);
	
	/**
	@brief	�_�Ɛ��̉����߂�����
	@param	vtx1			[in]		�����̎n�_
	@param	vtx2			[in]		�����̏I�_
	@param	posTarget		[in,out]	���肷��ʒu
	@param	posTargetOld	[in]		���肷��ߋ��ʒu
	@return	bool
	 */
	bool LineCrossProduct(D3DXVECTOR3 vtx1, D3DXVECTOR3 vtx2, D3DXVECTOR3* pos, D3DXVECTOR3 posOld);

	/**
	@brief	�_��OBB�̏Փ˔���
	@param	posO		[in]	�_�̈ʒu
	@param	posOldO		[in]	�_�̉ߋ��ʒu
	@param	posV		[in]	OBB���̈ʒu
	@param	rotV		[in]	OBB���̊p�x
	@param	sizeV		[in]	OBB���̃T�C�Y
	@param	posInter	[out]	�Փˈʒu
	@param	vecRef		[out]	�����߂�����
	@param	fRefIner	[in]	�����߂��̔{��
	@return	bool
	 */
	bool CollidePointToOBBTrigger(D3DXVECTOR3 posO, D3DXVECTOR3 posOldO, D3DXVECTOR3 posV, D3DXVECTOR3 rotV, D3DXVECTOR3 sizeV, D3DXVECTOR3* posInter = nullptr, D3DXVECTOR3* vecRef = nullptr, float fRefIner = 1.0f);

	/**
	@brief	�_��OBB�̉����߂�����
	@param	posO		[in,out]	�_�̈ʒu
	@param	posOldO		[in]		�_�̉ߋ��ʒu
	@param	posV		[in]		OBB���̈ʒu
	@param	rotV		[in]		OBB���̊p�x
	@param	sizeV		[in]		OBB���̃T�C�Y
	@return	bool
	 */
	bool CollidePointToOBB(D3DXVECTOR3* posO, D3DXVECTOR3 posOldO, D3DXVECTOR3 posV, D3DXVECTOR3 rotV, D3DXVECTOR3 sizeV);

	/**
	@brief	�_��OBB�̉����߂�����
	@param	pOut		[out]		�����߂������̃|�C���^
	@param	posO		[in,out]	�_�̈ʒu
	@param	posOldO		[in]		�_�̉ߋ��ʒu
	@param	posV		[in]		OBB���̈ʒu
	@param	rotV		[in]		OBB���̊p�x
	@param	sizeV		[in]		OBB���̃T�C�Y
	@return	bool
	 */
	bool CollidePointToOBB(D3DXVECTOR3* pOut, D3DXVECTOR3* posO, D3DXVECTOR3 posOldO, D3DXVECTOR3 posV, D3DXVECTOR3 rotV, D3DXVECTOR3 sizeV);

	/**
	@brief	�_��OBB�̔��˔���
	@param	pOut		[out]	�����߂������̃|�C���^
	@param	posO		[in]	�_�̈ʒu
	@param	posOldO		[in]	�_�̉ߋ��ʒu
	@param	posV		[in]	OBB���̈ʒu
	@param	rotV		[in]	OBB���̊p�x
	@param	sizeV		[in]	OBB���̃T�C�Y
	@param	fRefIner	[in]	���˂̔{��
	@return	bool
	 */
	bool ReflectPointToOBB(D3DXVECTOR3* pOut, D3DXVECTOR3* posO, D3DXVECTOR3* moveO, D3DXVECTOR3 posOldO, D3DXVECTOR3 posV, D3DXVECTOR3 rotV, D3DXVECTOR3 sizeV, float fRefIner);

	/**
	@brief	OBB�ƕ��ʂ̉����߂�����
	@param	posOBB			[in,out]	OBB�̈ʒu
	@param	vecOBBAxial		[in]		OBB�̊e������
	@param	posPlane		[in]		���ʂ̈ʒu
	@param	vecNorPlane		[in]		���ʂ̖@���x�N�g��
	@return	D3DXVECTOR3
	 */
	D3DXVECTOR3 CollideOBBToPlane(D3DXVECTOR3* posOBB, D3DXVECTOR3 vecOBBAxial, D3DXVECTOR3 posPlane, D3DXVECTOR3 vecNorPlane);

	/**
	@brief	���C��OBB�̏Փ˔���
	@param	pOut		[out]	�Փ˒n�_
	@param	posO		[in]	���C���̈ʒu
	@param	vecO		[in]	���C���̕����x�N�g��
	@param	posV		[in]	OBB���̈ʒu
	@param	rotV		[in]	OBB���̊p�x
	@param	sizeMaxV	[in]	OBB���̍ő�T�C�Y
	@param	sizeMinV	[in]	OBB���̍ŏ��T�C�Y
	@return	bool
	 */
	bool CollideRayToOBB(D3DXVECTOR3* pOut, D3DXVECTOR3 posO, D3DXVECTOR3 vecO, D3DXVECTOR3 posV, D3DXVECTOR3 rotV, D3DXVECTOR3 sizeMaxV, D3DXVECTOR3 sizeMinV);

	/**
	@brief	OBB��OBB�̏Փ˔���
	@param	posO		[in]	OBB1���̈ʒu
	@param	rotO		[in]	OBB1���̊p�x
	@param	sizeO		[in]	OBB1���̃T�C�Y
	@param	posV		[in]	OBB2���̈ʒu
	@param	rotV		[in]	OBB2���̊p�x
	@param	sizeV		[in]	OBB2���̃T�C�Y
	@return	bool
	 */
	bool CollideOBBToOBBTrigger(D3DXVECTOR3 posO, D3DXVECTOR3 rotO, D3DXVECTOR3 sizeO, D3DXVECTOR3 posV, D3DXVECTOR3 rotV, D3DXVECTOR3 sizeV);

	// �����蔻����v�Z�����p�֐��Q
	float lengthAxis(D3DXVECTOR3 separationAxis, D3DXVECTOR3 e1, D3DXVECTOR3 e2, D3DXVECTOR3 e3);
	D3DXVECTOR3 PosRelativeMtx(D3DXVECTOR3 posO, D3DXVECTOR3 rot, D3DXVECTOR3 offset);
	D3DXVECTOR3 NorRelativeMtx(D3DXVECTOR3 posO, D3DXVECTOR3 rot, D3DXVECTOR3 offsetMax, D3DXVECTOR3 offsetMin);
	D3DXVECTOR3 GetMtxPos(D3DXMATRIX mtx);
	float CrossProduct(D3DXVECTOR3 pos1, D3DXVECTOR3 pos2);
}

#endif
