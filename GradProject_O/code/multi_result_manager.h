//==========================================================
//
// �}���`���U���g�}�l�[�W���[ [multi_result_manager.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _MULTI_RESULT_MANAGER_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _MULTI_RESULT_MANAGER_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "list_manager.h"
#include "network.h"

//==========================================================
// �}�l�[�W���[�̔h���N���X��`
//==========================================================
class CMultiResultManager : public CListManager
{
public:

	// �󂯎����
	struct SInfo
	{
		bool bActive;	// �g�p����Ă��邩
		int nNumDelv;	// �z�B��
		int nId;		// ID

		// �R���X�g���N�^
		SInfo() : bActive(false), nNumDelv(0), nId(-1) {}
	};

private:

	CMultiResultManager();	// �R���X�g���N�^
	~CMultiResultManager();	// �f�X�g���N�^

public:	// �N�ł��A�N�Z�X�\

	// �����o�֐�
	HRESULT Init(void);
	void Uninit(void) override;
	void Update(void);

	// �擾�n
	SInfo* GetInfo() { return &m_aInfo[0]; }
	int GetNumPlayer() { return m_nNumPlayer; }
	int GetMyId() { return m_nMyId; }

	// �ݒ�n
	void DataSet(void);

	// �C���X�^���X�֘A
	static CMultiResultManager* GetInstance(void);
	static void Release(void);

private:	// �����������A�N�Z�X�\

	// �����o�֐�

	// �����o�ϐ�
	int m_nMyId;
	int m_nNumPlayer;
	SInfo m_aInfo[NetWork::MAX_CONNECT];

	// �ÓI�����o�ϐ�
	static CMultiResultManager* m_pInstance;	// �C���X�^���X
};

#endif