//========================================
// 
//�^�C�g���ł̃v���C���\�̓������� [player_result.h]
//Author : Kenta Hashimoto
// 
//========================================
#ifndef _PLAYER_RESULT_H_
#define _PLAYER_RESULT_H_

#include "player.h"

//========================================
//�v���C���[�N���X���p�����Ē�`
//========================================
class CPlayerResult : public CPlayer
{
public:

	CPlayerResult();
	~CPlayerResult();

	// �����o�֐�
	HRESULT Init(void);
	HRESULT Init(const char* pBodyName, const char* pLegName);	// �I�[�o�[���[�h
	void Uninit(void);
	void Update(void);
	static CPlayerResult* Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, const D3DXVECTOR3 move, const char* pBodyName, const char* pLegName);

	void Moving();

private:
	
	void Move();
	void MovePtnSelect();
	void MoveEnd();

	D3DXVECTOR3 m_EndPos;
	bool m_bStartPtn;

};

#endif
