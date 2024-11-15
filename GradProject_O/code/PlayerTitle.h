//<=================================================
//�^�C�g���ł̃v���C���\�̓������� [PlayerTitle.h]
//
//Author : Kazuki Watanabe
//<=================================================
#ifndef _PLAYERTITLE_H_
#define _PLAYERTITLE_H_

#include "player.h"

//<***************************************
//�^�C�g���֘A�Ŏg���֐�
//<***************************************
namespace Function
{
	//�C�����C���֐��g�p�E�ړI�n�ɒ��������̔���
	inline bool MoveToDest(const D3DXVECTOR3 Pos, const D3DXVECTOR3 DestPos, const float Distance,bool bZuse = true)
	{
		//Z�����g���Ȃ�
		if (bZuse)
		{
			//�ړI�̈ʒu�ɂ�����
			if (Pos.x >= DestPos.x + Distance && Pos.x <= DestPos.x - Distance
				|| Pos.x <= DestPos.x + Distance && Pos.x >= DestPos.x - Distance
				&& Pos.z >= DestPos.z + Distance && Pos.z <= DestPos.z - Distance
				|| Pos.z <= DestPos.z + Distance && Pos.z >= DestPos.z - Distance)
			{
				return true;
			}

		}
		//Z�����g��Ȃ��Ȃ�
		else
		{
			//�ړI�̈ʒu�ɂ�����
			if (Pos.x >= DestPos.x + Distance && Pos.x <= DestPos.x - Distance
				|| Pos.x <= DestPos.x + Distance && Pos.x >= DestPos.x - Distance)
			{
				return true;
			}

		}



		return false;
	}
}

//<========================================
//�v���C���[�N���X���p�����Ē�`
//<========================================
class CPlayerTitle: public CPlayer
{
public:

	//
	enum STATE
	{

		STATE_NONE=0,
		STATE_DEBUG,
		STATE_MAX
	};


	CPlayerTitle();
	~CPlayerTitle();

	// �����o�֐�
	HRESULT Init(void);
	HRESULT Init(const char* pBodyName, const char* pLegName);	// �I�[�o�[���[�h
	void Uninit(void);
	void Update(void);
	static CPlayerTitle* Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, const D3DXVECTOR3 move, const char* pBodyName, const char* pLegName);

	bool GetReached(void) {return m_bReached;}

private:

	void Moving(void);

	STATE m_eState;				//�f�o�b�O�p�̃X�e�[�g
	bool m_bReached;			//���������ǂ���


};

#endif
