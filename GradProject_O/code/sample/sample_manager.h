//==========================================================
//
// �T���v���}�l�[�W���[ [sample_manager.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _SAMPLEMANAGER_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _SAMPLEMANAGER_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

// �O���錾
class CSample;

#include "../list_manager.h"

//==========================================================
// �T���v���}�l�[�W���[�̃N���X��`
//==========================================================
class CSampleManager : public CListManager
{
private:

	CSampleManager();		// �R���X�g���N�^
	~CSampleManager();	// �f�X�g���N�^

public:	// �N�ł��A�N�Z�X�\

	// �����o�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	static CSampleManager* GetInstance(void);
	static void Release(void);
	CSample* GetTop(void) { return m_pTop; }
	CSample* GetCur(void) { return m_pCur; }
	void ListIn(CSample* pSample);
	void ListOut(CSample* pSample);
	int GetNum(void) { return m_nNum; }

private:	// �����������A�N�Z�X�\

	// �����o�֐�
	

	// �����o�ϐ�
	CSample* m_pTop;	// �擪
	CSample* m_pCur;	// �Ō��
	int m_nNum;
	static CSampleManager* m_pInstance;	// �C���X�^���X
};

#endif

