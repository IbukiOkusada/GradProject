//==========================================================
//
// �v���C���[�̏��� [player.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _PLAYER_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _PLAYER_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "main.h"
#include "object.h"
#include "objectX.h"
#include "task.h"
#include "effekseerControl.h"
#include "convenience.h"
#include "objectsound.h"
#include "radio.h"
#include "navi.h"
#include "road.h"
#include "scrollString2D.h"
#include "scrollText2D.h"
using namespace std;
// �O���錾
class CWaist;
class CCharacter;
class CObjectX;
class CRoad;
class CBaggage;
class CMultiCamera;
class CPredRoute;
class CContainer;

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
		TYPE_SEND = 0,	// �f�[�^���M
		TYPE_RECV,		// �f�[�^��M������
		TYPE_ACTIVE,	// ����\
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
	~CPlayer();	// �f�X�g���N�^

	// �����o�֐�
	HRESULT Init(void);
	HRESULT Init(const char *pBodyName, const char *pLegName);	// �I�[�o�[���[�h
	void Uninit(void);
	void Update(void);
	static CPlayer* Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, const D3DXVECTOR3 move,
		const int nId);

	// �����o�֐�(�ݒ�)
	void SetMove(const D3DXVECTOR3 move) { m_Info.move = move; }
	void SetPosition(const D3DXVECTOR3 pos) { m_Info.pos = pos; }
	void SetRotation(const D3DXVECTOR3 rot) { m_Info.rot = rot; }
	void SetType(TYPE type);
	void BindId(int nId) { m_nId = nId; }
	void SetMotion(int nMotion);
	void SetDraw(bool bDraw);
	void SetNext(CPlayer* pNext) { m_pNext = pNext; }
	void SetPrev(CPlayer* pPrev) { m_pPrev = pPrev; }
	void SetRotDiff(float fDiff) { m_fRotDest = fDiff; }
	CBaggage* ThrowBaggage(D3DXVECTOR3* pTarget);
	void Damage(float fDamage);

	// ��M�p���
	void SetRecvPosition(const D3DXVECTOR3 pos) { m_RecvInfo.pos = pos; }
	void SetRecvRotation(const D3DXVECTOR3 rot) { m_RecvInfo.rot = rot; }

	void SetNumDeliveryStatus(int nNum) { m_nNumDeliveryStatus = nNum; }
	void AddDeliveryCount(void) { m_nNumDeliveryStatus++; }
	
	// �����o�֐�(�擾)
	D3DXVECTOR3& GetMove(void) { return m_Info.move; }
	D3DXVECTOR3& GetPosition(void) { return m_Info.pos; }
	D3DXVECTOR3& GetRotation(void) { return m_Info.rot; }
	D3DXVECTOR3& GetOldPosition(void) { return m_Info.posOld; }
	D3DXVECTOR3& GetOldRotation(void) { return m_Info.rotOld; }
	CRoad* GetRoad(void) { return m_Info.pRoad; }
	CPlayer* GetNext(void) { return m_pNext; }
	CObjectX* GetObj() { return m_pObj; }
	CPlayer* GetPrev(void) { return m_pPrev; }
	CPredRoute* GetPredRoute() { return m_pPredRoute; }
	STATE GetState(void) { return m_Info.state; }
	int GetModelIndex(void) { return m_pObj->GetIdx(); }
	float GetEngine(void) { return m_fEngine; }
	float GetLife(void) { return m_fLife; }
	int GetNumDeliverStatus(void) { return m_nNumDeliveryStatus; }
	float GetLifeOrigin() { return m_fLifeOrigin; }
	int GetId() { return m_nId; }

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

	// �����o�֐�
	void SetMatrix(void);
	void StateSet(void);
	void Controller(void);
	void Move(void);
	void Rotate(void);
	void Adjust(void);
	bool Collision(void);
	bool CollisionObjX(void);
	bool CollisionRoad(void);
	bool CollisionGimick(void);
	void Engine(float fThrottle);
	void SearchRoad(void);
	void Nitro();
	void GetBestPath();
	void DEBUGKEY();

	// ��M�p�����o�֐�
	void RecvInerSet();

	// �����o�ϐ�
	CPlayer *m_pPrev;			// �O�̃I�u�W�F�N�g�ւ̃|�C���^
	CPlayer *m_pNext;			// ���̃I�u�W�F�N�g�ւ̃|�C���^
	SInfo m_Info;				// �������g�̏��
	SRecvInfo m_RecvInfo;		// ��M���
	float m_fRotMove;			// ���݂̊p�x
	float m_fRotDiff;			// �ړI�̊p�x
	float m_fRotDest;			// �p�x�v�Z
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
	int m_nId;					// ID
	TYPE m_type;			// ���
	CObjectX* m_pObj;		// �`��I�u�W�F�N�g
	CBaggage* m_pBaggage;	// �ו�
	CNavi* m_pNavi;			// �i�r
	CContainer* m_pContainer;
	CPredRoute* m_pPredRoute;	// �\���p
	CEffekseer::CEffectData * m_pTailLamp;
	CEffekseer::CEffectData* m_pBackdust;
	CEffekseer::CEffectData* m_pAfterburner;
	CEffekseer::CEffectData* m_pDamageEffect;
	CMasterSound::CObjectSound* m_pSound;
	CMasterSound::CObjectSound* m_pSoundBrake;
	std::vector<CRoad::SSearch*> m_pPath;
	float m_fbrakeVolume;
	float m_fbrakePitch;
	CRadio* pRadio;
	CMasterSound::CObjectSound* m_pCollSound;
	CScrollText2D* m_pFont[NUM_TXT];
	int m_nNumDeliveryStatus;  // �z�B������
};


#endif
