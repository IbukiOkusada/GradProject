//==========================================================
//
// ���}�l�[�W���[ [road_manager.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _ROADMANAGER_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _ROADMANAGER_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "list_manager.h"
#include "list.h"
#include "road.h"

// �O���錾

//==========================================================
// �T���v���̃N���X��`
//==========================================================
class CRoadManager : public CListManager
{
private:

	CRoadManager();		// �R���X�g���N�^
	~CRoadManager();	// �f�X�g���N�^

public:	// �N�ł��A�N�Z�X�\

	// �����o�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	static CRoadManager* GetInstance(void);
	static void Release(void);
	void ListIn(CRoad* pRoad);
	void ListOut(CRoad* pRoad);
	bool Hit(D3DXVECTOR3& pos, const float fRange, const float fHeight, const int nDamage);
	Clist<CRoad::SInfo*>* GetInfoList() { return &m_InfoList; }
	Clist<CRoad*>* GetList() { if (m_pList == nullptr) { m_pList = m_pList->Create(); }return m_pList; }	// ���X�g�擾
	void ListRelease() { if (m_pList != nullptr) { delete m_pList; m_pList = nullptr; } }			// ���X�g���

	// ���A���֐�
	void AllConnect(void);
	void SearchRoadConnect(void);

private:	// �����������A�N�Z�X�\

	// �����o�֐�
	void SideConnectCheck(CRoad* pRoad, CRoad* pCheckRoad);		// ���A������
	void VerticalConnectCheck(CRoad* pRoad, CRoad* pCheckRoad);	// �c�A������

	// �����o�ϐ�
	Clist<CRoad*>* m_pList;
	static CRoadManager* m_pInstance;	// �C���X�^���X
	Clist<CRoad::SInfo*> m_InfoList;
	
};

#endif

