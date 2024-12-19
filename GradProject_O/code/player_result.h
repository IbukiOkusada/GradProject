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

	enum ROTATE
	{
		TYPE_RIGHT = 0,
		TYPE_LEFT,
	};
	
	void Move();
	void MovePtnSelect();
	void MoveEnd();
	void MoveRot();
	void MovePtnSet(const D3DXVECTOR3 pos, const D3DXVECTOR3 move, const D3DXVECTOR3 rot,
		const D3DXVECTOR3 angle, const D3DXVECTOR3 Endpos, const D3DXVECTOR3 Endrot, const ROTATE type);

	D3DXVECTOR3 m_EndPos;
	D3DXVECTOR3 m_EndRot;
	D3DXVECTOR3 m_CameraAngle;
	bool m_bStartPtn;
	bool m_bEndPtn;
	int m_FadeStartCnt;

	ROTATE m_type;
};

#endif
