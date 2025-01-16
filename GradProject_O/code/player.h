//==========================================================
//
// �v���C���[�̏��� [player.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _PLAYER_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _PLAYER_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "main.h"
#include "task.h"
#include "effekseerControl.h"
#include "objectsound.h"
#include "road.h"
#include "shaderlight.h"
using namespace std;

// �O���錾
class CWaist;
class CCharacter;
class CObjectX;
class CBaggage;
class CMultiCamera;
class CPredRoute;
class CContainer;
class CNavi;
class CScrollText2D;
class CRadio;

// �}�N����`
#define MAX_ITEM  (1280)  // �����ł���A�C�e���̍ő吔
#define NUM_TXT  (4)  
//==========================================================
// �v���C���[�̃N���X��`(�h���N���X)
//==========================================================
class CPlayer : public CTask
{
public:

	// �����ޗ񋓌^
	enum TYPE
	{
		TYPE_SEND = 0,			// �f�[�^���M
		TYPE_RECV,				// �f�[�^��M������
		TYPE_ACTIVE,			// ����\
		TYPE_GAMESTARTOK,		// ��������
		TYPE_TUTOLERIAL_ACTIVE,  // �`���[�g���A����
		TYPE_NONE,
		TYPE_MAX
	};

	// ��ԗ񋓌^
	enum STATE
	{
		STATE_APPEAR = 0,	// �o�����
		STATE_NORMAL,		// �ʏ���
		STATE_NITRO,		// �u�[�X�g���
		STATE_DAMAGE,		// �_���[�W���
		STATE_DEATH,		// ���S���
		STATE_SPAWN,		// ���������
		STATE_MAX
	};

	// ���[�V�����񋓌^
	enum MOTION
	{
		MOTION_NEUTRAL = 0,	// �ҋ@���[�V����
		MOTION_LEFT,		// ���Ȃ���
		MOTION_RIGHT,		// ���Ȃ���
		MOTION_WIN,			// �������[�V����
		MOTION_CLEAR,		// �N���A���[�V����
		MOTION_FAILED,		// ���s���[�V����
		MOTION_THROW,		// �������[�V����
		MOTION_MAX
	};

private:	// �����������A�N�Z�X�\�Ȓ�`

	// ���\����
	struct SInfo
	{
		D3DXVECTOR3 pos;		// �ʒu
		D3DXVECTOR3 rot;		// ����
		D3DXVECTOR3 move;		// �ړ���
		float fSlideMove;		// �X���C�f�B���O�J�n�ړ���
		D3DXVECTOR3 posOld;		// �ݒ�ʒu
		D3DXVECTOR3 rotOld;		// �ߋ�����
		D3DXMATRIX mtxWorld;	// ���[���h�}�g���b�N�X
		STATE state;			// ���
		float fStateCounter;	// ��ԊǗ��J�E���^�[
		CRoad* pRoad;			// �Ŋ��̓�
		bool bHit;
		int bHitInterval;

		// �R���X�g���N�^
		SInfo() : pos(VECTOR3_ZERO), rot(VECTOR3_ZERO), move(VECTOR3_ZERO), fSlideMove(0.0f), 
			posOld(VECTOR3_ZERO), mtxWorld(), state(STATE::STATE_NORMAL), fStateCounter(0.0f), pRoad(nullptr) {}
	};

	// ��M�p���\����
	struct SRecvInfo
	{
		D3DXVECTOR3 pos;		// �ʒu
		D3DXVECTOR3 rot;		// ����

		// �R���X�g���N�^
		SRecvInfo() : pos(VECTOR3_ZERO), rot(VECTOR3_ZERO) {}
	};

public:	// �N�ł��A�N�Z�X�\

	CPlayer(int nId = -1);
	virtual ~CPlayer();	// �f�X�g���N�^

	// �����o�֐�
	HRESULT Init(void);
	HRESULT Init(const char *pBodyName, const char *pLegName);	// �I�[�o�[���[�h
	void Uninit(void);
	void Update(void);
	static CPlayer* Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, const D3DXVECTOR3& move,
		const int nId);

	// �����o�֐�(�ݒ�)
	void SetMove(const D3DXVECTOR3& move) { m_Info.move = move; }
	void SetPosition(const D3DXVECTOR3& pos) { m_Info.pos = pos; }
	void SetRotation(const D3DXVECTOR3& rot) { m_Info.rot = rot; }
	void SetType(TYPE type);
	void BindId(int nId) { m_nId = nId; }
	void SetMotion(int nMotion);
	void SetDraw(bool bDraw);
	void SetRotDiff(float fDiff) { m_fRotDest = fDiff; }
	CBaggage* ThrowBaggage(D3DXVECTOR3* pTarget);
	void Damage(float fDamage);

	// ��M�p���
	void SetRecvPosition(const D3DXVECTOR3& pos) { m_RecvInfo.pos = pos; }
	void SetRecvRotation(const D3DXVECTOR3& rot) { m_RecvInfo.rot = rot; }

	void SetNumDeliveryStatus(int nNum) { m_nNumDeliveryStatus = nNum; }
	void AddDeliveryCount(void) { m_nNumDeliveryStatus++; }

	void EffectUninit(void);
	
	// �����o�֐�(�擾)
	D3DXVECTOR3& GetMove(void) { return m_Info.move; }
	D3DXVECTOR3& GetPosition(void) { return m_Info.pos; }
	D3DXVECTOR3& GetRotation(void) { return m_Info.rot; }
	D3DXVECTOR3& GetOldPosition(void) { return m_Info.posOld; }
	D3DXVECTOR3& GetOldRotation(void) { return m_Info.rotOld; }
	CRoad* GetRoad(void) { return m_Info.pRoad; }
	CObjectX* GetObj() { return m_pObj; }
	CCharacter* GetCharacteter() { return m_pCharacter; }
	CPredRoute* GetPredRoute() { return m_pPredRoute; }
	int GetModelIndex(void);
	STATE GetState(void) { return m_Info.state; }
	float GetEngine(void) { return m_fEngine; }
	float GetLife(void) { return m_fLife; }
	int GetNumDeliverStatus(void) { return m_nNumDeliveryStatus; }
	float GetLifeOrigin() { return m_fLifeOrigin; }
	int GetId() { return m_nId; }
	CRadio* GetRadio() { return m_pRadio; }
	TYPE GetType() { return m_type; }

protected:	// �����������A�N�Z�X�\

	//=============================
	// �֐����X�g
	//=============================
	typedef void(CPlayer::* SETTYPE_FUNC)();
	static SETTYPE_FUNC m_SetTypeFunc[];

	// ��Ԑݒ�֐�
	void SetStateSend();
	void SetStateRecv();
	void SetStateActive();
	void SetStateGameStartOk();
	void SetStateTutorialActive();
	void SetStateNone();

	// �����o�֐�
	void SetMatrix(void);
	void StateSet(void);
	void Controller(void);
	void Move(void);
	void Rotate(void);
	bool Collision(void);
	bool CollisionObjX(void);
	bool CollisionEnemy(void);
	bool CollisionRoad(void);
	bool CollisionGimick(void);
	bool CollisionField(void);
	void Engine(float fThrottle);
	void SearchRoad(void);
	void Nitro();
	void GetBestPath();
	void DEBUGKEY();
	void SendData();
	void Respawn();
	void SetCol();
	void CameraMove(const D3DXVECTOR3& rot);

	// ��M�p�����o�֐�
	void RecvInerSet();

	// �����o�ϐ�
	SInfo m_Info;				// �������g�̏��
	float m_fRotMove;			// ���݂̊p�x
	float m_fRotDiff;			// �ړI�̊p�x
	float m_fRotDest;			// �p�x�v�Z

	// ���x�v�Z�p
	float m_fSpeed;				//���x(����)
	float m_fBrake;
	float m_fEngine;
	float m_fTurnSpeed;
	float m_fHandle;
	float m_fLife;
	float m_fLifeOrigin;
	float m_fCamera;
	float m_fSlowRate = 1.0f;
	float m_fNitroCool;

	int m_nId;				// ID
	TYPE m_type;			// ���
	CObjectX* m_pObj;		// �o�C�N
	CCharacter* m_pCharacter;	// �L�����N�^�[
	CBaggage* m_pBaggage;	// �ו�
	CNavi* m_pNavi;			// �i�r
	CContainer* m_pContainer;
	CPredRoute* m_pPredRoute;	// �\���p
	std::vector<CRoad::SSearch*> m_pPath;
	CScrollText2D* m_pFont[NUM_TXT];
	int m_nNumDeliveryStatus;  // �z�B������

	// ���ӂ�������
	CEffekseer::CEffectData * m_pTailLamp;
	CEffekseer::CEffectData* m_pBackdust;
	CEffekseer::CEffectData* m_pAfterburner;
	CEffekseer::CEffectData* m_pDamageEffect;
	CEffekseer::CEffectData* m_pDust;
	// �T�E���h
	CMasterSound::CObjectSound* m_pSound;
	CMasterSound::CObjectSound* m_pSoundBrake;
	CMasterSound::CObjectSound* m_pCollSound;
	CMasterSound::CObjectSound* m_pambient;
	float m_fbrakeVolume;
	float m_fbrakePitch;
	CRadio* m_pRadio;
	//�V�F�[�_�[���C�g
	CShaderLight::SLight* m_pShaderLight;
	// �I�����C���֘A
	SRecvInfo m_RecvInfo;		// ��M���
};


#endif
