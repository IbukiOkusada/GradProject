//==========================================================
//
// �ԃ}�l�[�W���[ [car_manager.h]
// Author : Riku Nakamura
//
//==========================================================
#ifndef _CARMANAGER_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _CARMANAGER_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "list_manager.h"
#include "list.h"
#include "map_list.h"
#include "car.h"
#include "police.h"

// �O���錾

//==========================================================
// �T���v���̃N���X��`
//==========================================================
class CCarManager : public CListManager
{
public:

	// ��M�p�\����
	struct NextCreateInfo
	{
		int nChaseId;		// �ǐՂ��Ă���v���C���[ID
		CCar::CAR_TYPE type;	// ���
		D3DXVECTOR3 pos;	// �ʒu
		D3DXVECTOR3 rot;	// ����
		D3DXVECTOR3 move;	// �ړ���
		CPolice::CHASE chase;

		// �R���X�g���N�^
		NextCreateInfo() : nChaseId(-1), type(CCar::CAR_TYPE::CAR_TYPE_MAX),
			pos(VECTOR3_ZERO), rot(VECTOR3_ZERO), move(VECTOR3_ZERO), chase(CPolice::CHASE::CHASE_MAX) {}
	};

private:

	CCarManager();		// �R���X�g���N�^
	~CCarManager();	// �f�X�g���N�^

public:	// �N�ł��A�N�Z�X�\

	// �����o�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	static CCarManager* GetInstance(void);
	static void Release(void);
	Clist<CCar*>* GetList() { if (m_pList == nullptr) { m_pList = m_pList->Create(); }return m_pList; }	// ���X�g�擾
	Cmaplist<CCar*>* GetMapList() { return &m_List; }
	void ListRelease() { if (m_pList != nullptr) { delete m_pList; m_pList = nullptr; } }			// ���X�g���
	void ListIn(CCar* pCar);
	void ListOut(CCar* pCar);
	void CreateListIn(NextCreateInfo& info, int nId);
	void CreateListOut(NextCreateInfo& info, int nId);
	NextCreateInfo* CreateGet(int nId);
	bool Hit(D3DXVECTOR3& pos, const float fRange, const float fHeight, const int nDamage);

private:	// �����������A�N�Z�X�\

	// �����o�֐�
	void IdListIn(CCar* pCar);
	void IdListOut(CCar* pCar);

	// �����o�ϐ�
	Clist<CCar*>* m_pList;
	Cmaplist<CCar*> m_List;
	Cmaplist<NextCreateInfo*> m_NextList;
	Cmaplist<NextCreateInfo*> m_NextTempList;
	int m_nNum;		// ����
	bool m_bSet;	// �ݒ蒆���ǂ���
	static CCarManager* m_pInstance;	// �C���X�^���X
};

#endif
