//===============================================
//
// �ʐM�����̊Ǘ��w�b�_�[ [game.h]
// Author : Ibuki Okusada
//
//===============================================
#ifndef _NETWORK_H_
#define _NETWORK_H_

#include "protocol_online.h"

// �O���錾
class CClient;
class CPlayer;

//===============================================
// �l�b�g���[�N�N���X�̒�`
//===============================================
class CNetWork
{
public:

	enum STATE
	{
		STATE_SINGLE = 0,	// �ʐM�Ȃ�
		STATE_ONLINE,		// �I�����C��
		STATE_MAX
	};

private:

	CNetWork();		// �R���X�g���N�^
	~CNetWork();	// �f�X�g���N�^

public:

	// �C���X�^���X�Ǘ��֐�
	static CNetWork* Create();
	static CNetWork* GetInstance();
	static void Release();

	// �����o�֐�
	bool ReConnect();
	bool DisConnect();
	STATE GetState() { return m_state; }

	// �����o�֐�(�ݒ�)

	// �����o�֐�(�擾)
	int GetIdx() { return m_nMyIdx; }

private:

	//=============================
	// �֐����X�g
	//=============================
	typedef void(CNetWork::* COMMAND_FUNC)(int*, const int, const char*);
	static COMMAND_FUNC m_CommandFunc[];

	//=============================
	// �����o�֐�
	//=============================
	// �R�}���h�֐�
	void CommandNone(int* pByte, const int nId, const char* pRecvData);
	void CommandJoin(int* pByte, const int nId, const char* pRecvData);
	void CommandGetId(int* pByte, const int nId, const char* pRecvData);
	void CommandDelete(int* pByte, const int nId, const char* pRecvData);

	// �����o�֐�
	HRESULT Init();
	void Uninit();
	void LoadAddressFile();
	CPlayer* CreatePlayer(int nId);

	// TCP�ʐM�p�֐�
	void Online(void);
	void ByteCheck(char* pRecvData, int* pRecvByte);
	void DeleteData(char* pRecvData, int* pRecvByte);
	void OnlineEnd(void);

	// �����o�ϐ�
	STATE m_state;			// ���
	int m_nMyIdx;			// �������g��ID
	CClient* m_pClient;		// �N���C�A���g
	std::string m_Address;	// �ڑ���̃A�h���X
	int m_nSledCnt;			// �X���b�h��
	bool m_bEnd;			// �I������
	mutex m_mutex;			// �~���[�e�b�N�X
	bool m_aConnect[NetWork::MAX_CONNECT];

	// �ÓI�����o�ϐ�
	static CNetWork* m_pInstance;	// �C���X�^���X
};

#endif
