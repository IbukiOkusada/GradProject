//==========================================================
//
// �x�@AI�}�l�[�W���[ [police_AI_manager.h]
// Author : Riku Nakamura
//
//==========================================================
#ifndef _POLICEAIMANAGER_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _POLICEAIMANAGER_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "list_manager.h"
#include "list.h"
#include "map_list.h"

// �O���錾
class CPoliceAI;

//==========================================================
// �}�l�[�W���[�̃N���X��`
//==========================================================
class CPoliceAIManager : public CListManager
{
private:

	CPoliceAIManager();		// �R���X�g���N�^
	~CPoliceAIManager();	// �f�X�g���N�^

public:	// �N�ł��A�N�Z�X�\

	// �����o�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	static CPoliceAIManager* GetInstance(void);
	static void Release(void);
	Cmaplist<CPoliceAI*>* GetMapList() { return &m_maplist; }
	void ListIn(CPoliceAI* pPoliceAI);
	void ListOut(CPoliceAI* pPoliceAI);

	bool GetCall() { return m_bCall; }
	void SetCall(bool bCall) { m_bCall = bCall; }

private:	// �����������A�N�Z�X�\

	// �����o�֐�
	void Search(void);

	// �����o�ϐ�
	bool m_bFlag;
	bool m_bCall;
	Cmaplist<CPoliceAI*> m_maplist;
	static CPoliceAIManager* m_pInstance;	// �C���X�^���X
};

#endif
