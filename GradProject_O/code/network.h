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
class CRoad;

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
	void Update();

	// ���M�֐�
	void SendNone();
	void SendJoin();
	void SendGetId();
	void SendDelete();
	void SendPlPos(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot);
	void SendPlDamage(const float nowlife);
	void SendPlGoal(int nId);
	void SendGmHit(const int nId, const D3DXVECTOR3& HitPos, const float fSpeed);
	void SendNextGoal(const int nId);
	void SendGameStartOk();
	void SendTutorialOk();
	void SendSetInspection(const int nId, const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, int nIdx, int nStartPdId);
	void SendEndInspection(int nId);
	void SendCarPos(int nId, const D3DXVECTOR3& pos, const D3DXVECTOR3& rot);
	void SendPdPos(int nId, const D3DXVECTOR3& pos, const D3DXVECTOR3& rot);
	void SendAddPdPos(int nId, const D3DXVECTOR3& pos, const D3DXVECTOR3& rot);
	void SendPdChase(int nId, int plyid);
	void SendAddPdChase(int nId, int plyid);
	void SendPdChaseEnd(int nId);
	void SendAddPdChaseEnd(int nId);

	// �����o�֐�(�ݒ�)

	// �����o�֐�(�擾)
	int GetIdx() { return m_nMyIdx; }
	bool GetConnect(int nIdx = 0) { return m_aConnect[nIdx]; }
	NetWork::CTime* GetTime() { return &m_SendTime; }

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
	void RecvPlDamage(int* pByte, const int nId, const char* pRecvData);
	void RecvPlGoal(int* pByte, const int nId, const char* pRecvData);
	void RecvGmHit(int* pByte, const int nId, const char* pRecvData);
	void RecvNextGoal(int* pByte, const int nId, const char* pRecvData);
	void RecvGameStartOk(int* pByte, const int nId, const char* pRecvData);
	void RecvGameStart(int* pByte, const int nId, const char* pRecvData);
	void RecvTutorialOk(int* pByte, const int nId, const char* pRecvData);
	void RecvTutorialEnd(int* pByte, const int nId, const char* pRecvData);
	void RecvSetInspection(int* pByte, const int nId, const char* pRecvData);
	void RecvEndInspection(int* pByte, const int nId, const char* pRecvData);
	void RecvCarPos(int* pByte, const int nId, const char* pRecvData);
	void RecvPdPos(int* pByte, const int nId, const char* pRecvData);
	void RecvAddPdPos(int* pByte, const int nId, const char* pRecvData);
	void RecvPdChase(int* pByte, const int nId, const char* pRecvData);
	void RecvAddPdChase(int* pByte, const int nId, const char* pRecvData);
	void RecvPdChaseEnd(int* pByte, const int nId, const char* pRecvData);
	void RecvAddPdChaseEnd(int* pByte, const int nId, const char* pRecvData);

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
	NetWork::CTime m_SendTime;

	// �ÓI�����o�ϐ�
	static CNetWork* m_pInstance;	// �C���X�^���X
};

#endif
