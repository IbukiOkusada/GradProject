//==========================================================
//
// �v���C���[�}�l�[�W���[ [player_manager.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _PLAYERMANAGER_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _PLAYERMANAGER_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "list_manager.h"

// �O���錾
class CPlayer;

//==========================================================
// �T���v���̃N���X��`
//==========================================================
class CPlayerManager : public CListManager
{
private:

	CPlayerManager();		// �R���X�g���N�^
	~CPlayerManager();	// �f�X�g���N�^

public:	// �N�ł��A�N�Z�X�\

	// �����o�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	static CPlayerManager* GetInstance(void);
	static void Release(void);
	int GetNum() { return m_List.size(); }
	CPlayer* GetPlayer(int nIdx = 0);
	std::map<int, CPlayer*>* GetList() { return &m_List; }
	void ListIn(CPlayer* pPlayer);
	void ListOut(CPlayer* pPlayer);
	bool Hit(D3DXVECTOR3& pos, const float fRange, const float fHeight, const int nDamage);

private:	// �����������A�N�Z�X�\

	// �����o�֐�
	
	// �����o�ϐ�
	std::map<int, CPlayer*> m_List;
	static CPlayerManager* m_pInstance;	// �C���X�^���X
};

#endif
