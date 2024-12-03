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

// �O���錾
class CCar;

//==========================================================
// �T���v���̃N���X��`
//==========================================================
class CCarManager : public CListManager
{
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
	bool Hit(D3DXVECTOR3& pos, const float fRange, const float fHeight, const int nDamage);

private:	// �����������A�N�Z�X�\

	// �����o�֐�
	void IdListIn(CCar* pCar);
	void IdListOut(CCar* pCar);

	// �����o�ϐ�
	Clist<CCar*>* m_pList;
	Cmaplist<CCar*> m_List;
	int m_nNum;
	static CCarManager* m_pInstance;	// �C���X�^���X
};

#endif
