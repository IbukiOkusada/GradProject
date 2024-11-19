//==========================================================
//
// �i�r [navi.h]
// Author : �O�엳�V��
//
//==========================================================
#ifndef _Navi_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _Navi_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "task.h"
#include "camera.h"
#include "road.h"
#include "effekseerControl.h"
#include "list.h"
#include "goal.h"
//#include "task.h"	// ����Ńt�@�C���C���N���[�h�ł��܂�

//==========================================================
// �T���v���̃N���X��`
//==========================================================
class CNavi : public CTask
{

public:	// �N�ł��A�N�Z�X�\

	CNavi();	// �R���X�g���N�^(�I�[�o�[���[�h)
	~CNavi();	// �f�X�g���N�^

	// �����o�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	static CNavi* Create(void);

	// �����o�֐�(�擾)

	// �����o�֐�(�ݒ�)
	struct SEffect
	{
		CRoad::SSearch* pTarget;
		CEffekseer::CEffectData* pLine;
	
		SEffect():pTarget(nullptr), pLine(nullptr){}
	};
private:	// �����������A�N�Z�X�\

	// �����o�֐�
	void StartNavigation();
	void UpdateNavigation();
	void Reach();
	void CreateEffect();
	// �����o�ϐ�
	CGole* m_pGole;
	int m_IdxPath;
	std::vector<CRoad::SSearch*> m_Path;
	Clist<SEffect*> m_Effects;
};

#endif
