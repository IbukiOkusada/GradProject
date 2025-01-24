//===============================================
//
// �Q�[����ʂ̊Ǘ����� [game.h]
// Author : Ibuki Okusada
//
//===============================================
#ifndef _GAME_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _GAME_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "manager.h"
#include <mutex>
#include "objectsound.h"
#include "shaderlight.h"

// �O���錾
class CScore;
class CTime;
class CPlayer;
class CFileLoad;
class CClient;
class CMeshDome;
class CPause;
class CSpeedMeter;
class CDeliveryStatus;
class CTimer;
class CMultiCamera;
class CGoalManager;
class CFog;
class CScrollText2D;
class CDoll;

namespace Game
{
	const float DOME_LENGTH = 15000.0f;
}

//===============================================
// �Q�[���N���X�̒�`(�h���N���X)
//===============================================
class CGame : public CScene
{
public:
	
	enum STATE
	{
		STATE_NONE = 0,
		STATE_PROG,
		STATE_SUCCESS,
		STATE_FAIL,
		STATE_MULTIEND,
		STATE_MAX
	};

public:

	// �����o�֐�
	CGame();	// �R���X�g���N�^
	CGame(int nNumPlayer);	// �R���X�g���N�^
	~CGame();	// �f�X�g���N�^

	// �����o�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	static void SetNumPlayer(int nNum) { m_nNumPlayer = nNum; }
	static int GetNumPlayer(void) { return m_nNumPlayer; }
	void SetGameState(STATE state) { m_GameState = state; }
	STATE GetState() { return m_GameState; }
	// �����o�֐�(�|�C���^)
	int GetTotalDeliveryStatus() { return m_nTotalDeliveryStatus; }
	int GetRestDeliveryStatus();

	void End_Success();
	void End_Fail();
	void End_MultiEnd();
	static CGame* GetInstance() { return m_pInstance; }
private:

	//=============================
	// �֐����X�g
	//=============================
	typedef void(CGame::* CREATE_PL_FUNC)();
	static CREATE_PL_FUNC m_CreatePlayerFunc[];

	// ��Ԑݒ�֐�
	void CreateSinglePlayer();
	void CreateMultiPlayer();

	// �����o�֐�
	void StartIntro();  // �Q�[���X�^�[�g���̉��o
	bool StartDirection();
	void CreatePolice();
	void CreateCar();

	CMeshDome *m_pMeshDome;		// ���b�V���h�[���̃|�C���^
	CGoalManager *m_pGoalManager;  // �S�[���}�l�[�W���[�̃|�C���^
	CDeliveryStatus* m_pDeliveryStatus;  // �z�B�󋵂�UI�̃|�C���^
	CTimer* m_pGameTimer;		// �^�C�}�[�̃|�C���^
	int m_nSledCnt;				// ���ݓ��삵�Ă���X���b�h��
	static int m_nNumPlayer;	// �v���C�l��
	CPause *m_pPause;			// �|�[�Y���
	bool m_bEnd;
	float m_fOpenDoorUISin;
	bool m_bPause;              // �|�[�Y
	int m_nTotalDeliveryStatus;  // �v���C���[���z�B���鐔
	int m_nStartCameraCount;
	CDoll* m_pDoll[7];

	STATE m_GameState;
	CScrollText2D* m_pEndText;
	CMasterSound::CObjectSound* m_pEndSound;
	static CGame* m_pInstance;
};

#endif