//===============================================
//
// �ʐM�����̊Ǘ��w�b�_�[ [network.h]
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

	// ���M�֐�
	void SendNone();
	void SendJoin();
	void SendGetId();
	void SendDelete();
	void SendPlPos(const D3DXVECTOR3& pos);
	void SendPlRot(const D3DXVECTOR3& rot);
	void SendPlDamage(const float nowlife);
	void SendPlGoal(int nId);

	// �����o�֐�(�ݒ�)

	// �����o�֐�(�擾)
	int GetIdx() { return m_nMyIdx; }
	bool GetConnect(int nIdx = 0) { return m_aConnect[nIdx]; }

private:

	//=============================
	// �֐����X�g
	//=============================
	typedef void(CNetWork::* RECV_FUNC)(int*, const int, const char*);
	static RECV_FUNC m_RecvFunc[];

	//=============================
	// �����o�֐�
	//=============================
	// ��M�֐�
	void RecvNone(int* pByte, const int nId, const char* pRecvData);
	void RecvJoin(int* pByte, const int nId, const char* pRecvData);
	void RecvGetId(int* pByte, const int nId, const char* pRecvData);
	void RecvDelete(int* pByte, const int nId, const char* pRecvData);
	void RecvPlPos(int* pByte, const int nId, const char* pRecvData);
	void RecvPlRot(int* pByte, const int nId, const char* pRecvData);
	void RecvPlDamage(int* pByte, const int nId, const char* pRecvData);
	void RecvPlGoal(int* pByte, const int nId, const char* pRecvData);

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
	bool GetActive();

	// �����o�ϐ�
	STATE m_state;			// ���
	int m_nMyIdx;			// �������g��ID
	CClient* m_pClient;		// �N���C�A���g
	std::string m_Address;	// �ڑ���̃A�h���X
	int m_nSledCnt;			// �X���b�h��
	bool m_bEnd;			// �I������
	mutex m_aPlayerMutex[NetWork::MAX_CONNECT] = {};			// �~���[�e�b�N�X
	mutex m_mutex;
	bool m_aConnect[NetWork::MAX_CONNECT];

	// �ÓI�����o�ϐ�
	static CNetWork* m_pInstance;	// �C���X�^���X
};

#endif
