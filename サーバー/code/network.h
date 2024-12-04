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
class CServer;

//===============================================
// �l�b�g���[�N�N���X�̒�`
//===============================================
class CNetWork
{
private:

	// �t���O�܂Ƃ�
	struct SFlagInfo
	{
		bool bStart;	// �J�n
		bool bTutorial;	// �`���[�g���A��

		// �R���X�g���N�^
		SFlagInfo() : bStart(false), bTutorial(false) {}
	};

private:

	CNetWork();		// �R���X�g���N�^
	~CNetWork();	// �f�X�g���N�^

public:

	// �C���X�^���X�Ǘ��֐�
	static CNetWork* Create();
	static CNetWork* GetInstance();
	static void Release();

private:

	//=============================
	// �֐����X�g
	//=============================
	typedef void(CNetWork::* COMMAND_FUNC)(const int, const char*, CClient*, int*);
	static COMMAND_FUNC m_CommandFunc[];

	//=============================
	// �����o�֐�
	//=============================
	// �R�}���h�֐�
	void CommandNone(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte);
	void CommandJoin(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte);
	void CommandGetId(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte);
	void CommandDelete(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte);
	void CommandPlPos(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte);
	void CommandPlDamage(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte);
	void CommandPlGoal(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte);
	void CommandGmHit(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte);
	void CommandNextGoal(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte);
	void CommandGameStartOk(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte);
	void CommandGameStart(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte);
	void CommandTutorialOk(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte);
	void CommandTutorialEnd(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte);
	void CommandSetInspection(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte);
	void CommandEndInspection(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte);
	void CommandCarPos(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte);
	void CommandPdPos(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte);
	void CommandAddPdPos(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte);
	void CommandPdChase(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte);
	void CommandAddPdChase(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte);
	void CommandPdChaseEnd(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte);
	void CommandAddPdChaseEnd(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte);

	// �����o�֐�
	HRESULT Init();
	void Uninit();

	// TCP�ʐM�p�֐�
	void Access(CClient* pClient);
	void Accept(CServer* pServer);
	void Send(CServer** ppServer);
	void Leave(int nId, CClient* pClient);

	// �����o�ϐ�
	CClient* m_apClient[NetWork::MAX_CONNECT];	// �N���C�A���g
	char m_aSendData[NetWork::MAX_SEND_DATA] = {};	// ���M�f�[�^
	int m_nSendByte;	// ���M�f�[�^��
	int m_nSledCnt;		// �X���b�h��
	bool m_bEnd;		// �I������
	CServer* m_pServer;	// �T�[�o�[
	int m_nConnectCnt;	// �ڑ���
	bool m_bStart;		// �J�n�t���O

	// �t���O�֘A
	SFlagInfo m_aFlag[NetWork::MAX_CONNECT];	// �N���C�A���g���ꂼ��̃t���O
	SFlagInfo m_ServerFlag;	// �T�[�o�[�̃t���O

	// �ÓI�����o�ϐ�
	static CNetWork* m_pInstance;	// �C���X�^���X
};

#endif
