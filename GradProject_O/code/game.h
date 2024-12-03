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

class CScrollText2D;
// �}�N����`
#define NUM_FILTER	(2)

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
	// �����o�֐�(�|�C���^)
	CPlayer *GetPlayer(void);
	CFileLoad *GetFileLoad(void);


	void End_Success();
	void End_Fail();
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

	CFileLoad *m_pFileLoad;		// �t�@�C���ǂݍ��݂̃|�C���^
	CPlayer** m_ppPlayer;		// �v���C���[�̃|�C���^
	CMultiCamera **m_ppCamera;	// �J�����̃|�C���^
	CMeshDome *m_pMeshDome;		// ���b�V���h�[���̃|�C���^
	CGoalManager *m_pGoalManager;  // �S�[���}�l�[�W���[�̃|�C���^

	CDeliveryStatus* m_pDeliveryStatus;  // �z�B�󋵂�UI�̃|�C���^
	CTimer* m_pGameTimer;		// �^�C�}�[�̃|�C���^
	char m_aAddress[30];		// �ڑ���T�[�o�[�̃A�h���X
	int m_nSledCnt;				// ���ݓ��삵�Ă���X���b�h��
	static int m_nNumPlayer;	// �v���C�l��
	CPause *m_pPause;			// �|�[�Y���
	WSADATA m_wsaData;
	std::mutex m_mutex;
	bool m_bEnd;
	float m_fOpenDoorUISin;
	bool m_bPause;              // �|�[�Y
	int m_nTotalDeliveryStatus;  // �v���C���[���z�B���鐔
	int m_nStartCameraCount;

	STATE m_GameState;
	CScrollText2D* m_pEndText;
	CMasterSound::CObjectSound* m_pEndSound;
};

#endif