//===============================================
//
// ��̏��� [river.h]
// Author : Ibuki Okusada
//
//===============================================
#ifndef _RIVER_H_
#define _RIVER_H_

#include "task.h"
#include "list.h"

namespace river
{
	const int MAX = 2;	// ����
}

// �O���錾
class CMeshField;

//==========================================================
// �x�@�̃N���X��`
//==========================================================
class CRiver : public CTask
{
private:

	struct SInfo
	{
		D3DXVECTOR3 pos;	// ���W
		D3DXVECTOR3 rot;	// ����
		D3DXVECTOR2 size;	// 1���ӂ�̕�
		D3DXVECTOR2 speed;	// �ړ����x
		int nWidth;			// ������
		int nHeight;		// �c����

		// �R���X�g���N�^
		SInfo() : pos(VECTOR3_ZERO), rot(VECTOR3_ZERO), size(VECTOR2_ZERO), speed(VECTOR2_ZERO), nWidth(0), nHeight(0){}
		SInfo(const D3DXVECTOR3& _pos, const D3DXVECTOR3& _rot, const D3DXVECTOR2& _size, const D3DXVECTOR2& _speed, const int _width, const int _height):
			pos(_pos), rot(_rot), size(_size), speed(_speed), nWidth(_width), nHeight(_height) {}
	};

public:

	CRiver(SInfo info = SInfo());	// �R���X�g���N�^
	~CRiver();	// �f�X�g���N�^

	// �����o�֐�
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;

	/**
	@brief	��̐���
	@param	pos		[in]	���W
	@param	rot		[in]	����
	@param	size	[in]	�ꖇ�ӂ�̃T�C�Y
	@param	speed	[in]	�X�N���[�����x
	@param	nWidth	[in]	������
	@param	nHeight	[in]	�c����
	@return	CRiver*
	*/
	static CRiver* Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, const D3DXVECTOR2& size, 
		const D3DXVECTOR2& speed, const int& nWidth, const int& nHeight);

private:

	// �����o�ϐ�
	SInfo m_Info;	// ��{���
	std::vector<CMeshField*> m_obj;	// �I�u�W�F�N�g���
};

#endif
