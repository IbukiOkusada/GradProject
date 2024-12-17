//==========================================================
//
// �x�@�}�l�[�W���[ [police_manager.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _POLICEMANAGER_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _POLICEMANAGER_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "list_manager.h"
#include "list.h"
#include "map_list.h"

// �O���錾
class CPolice;

//==========================================================
// �}�l�[�W���[�̃N���X��`
//==========================================================
class CPoliceManager : public CListManager
{
private:

	struct SInspInfo
	{
		float fInterval;	// �C���^�[�o��
		float fTime;		// �C���^�[�o������
		int nCnt;			// �J�E���g

		// �R���X�g���N�^
		SInspInfo() : fInterval(0.0f), nCnt(0), fTime(0.0f) {}
	};

private:

	CPoliceManager();		// �R���X�g���N�^
	~CPoliceManager();	// �f�X�g���N�^

public:	// �N�ł��A�N�Z�X�\

	// �����o�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	static CPoliceManager* GetInstance(void);
	static void Release(void);
	Clist<CPolice*>* GetList() { if (m_pList == nullptr) { m_pList = m_pList->Create(); }return m_pList; }	// ���X�g�擾
	void ListRelease() { if (m_pList != nullptr) { delete m_pList; m_pList = nullptr; } }			// ���X�g���
	Cmaplist<CPolice*>* GetMapList() { return &m_maplist; }
	void ListIn(CPolice* pPolice);
	void ListOut(CPolice* pPolice);
	void Warning(CPolice* pPolice);
	void SetInspection();
	void ResetInterVal() { m_InspInfo.fInterval = 0.0f; }
	bool Hit(D3DXVECTOR3& pos, const float fRange, const float fHeight, const int nDamage);
	std::vector<CPolice*>* GetNearList() { return &m_NearPoliceList; }

private:	// �����������A�N�Z�X�\

	// �����o�֐�
	void IdListIn(CPolice* pPolice);
	void IdListOut(CPolice* pPolice);

	// �����o�ϐ�
	Clist<CPolice*>* m_pList;
	Cmaplist<CPolice*> m_maplist;
	std::vector<CPolice*> m_NearPoliceList;	// �߂��x�@���X�g
	int m_nNum;
	static CPoliceManager* m_pInstance;	// �C���X�^���X
	SInspInfo m_InspInfo;
};

#endif
