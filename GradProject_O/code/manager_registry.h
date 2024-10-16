//==========================================================
//
// �}�l�[�W���[�����}�l�[�W���[ [manager_registry.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _MANAGER_REGISTRY_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _MANAGER_REGISTRY_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

// �O���錾
class CListManager;

//==========================================================
// �}�l�[�W���[�����}�l�[�W���[�̃N���X��`
//==========================================================
class CManagerRegistry
{
private:

	CManagerRegistry();		// �R���X�g���N�^
	~CManagerRegistry();	// �f�X�g���N�^

public:	// �N�ł��A�N�Z�X�\

	static CManagerRegistry* GetInstance(void);
	static void Release(void);
	void ListIn(CListManager* pListManager);
	void ListOut(CListManager* pListManager);
	int GetNum(void) { return m_nNum; }

private:	// �����������A�N�Z�X�\

	// �����o�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	CListManager* GetTop(void) { return m_pTop; }
	CListManager* GetCur(void) { return m_pCur; }

	// �����o�ϐ�
	CListManager* m_pTop;	// �擪
	CListManager* m_pCur;	// �Ō��
	int m_nNum;
	static CManagerRegistry* m_pInstance;	// �C���X�^���X
};

#endif
