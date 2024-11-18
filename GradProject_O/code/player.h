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
using namespace std;
// �O���錾
class CWaist;
class CCharacter;
class CObjectX;
class CRoad;
class CBaggage;
class CMultiCamera;

// �}�N����`
#define MAX_ITEM  (1280)  // �����ł���A�C�e���̍ő吔

//==========================================================
// �v���C���[�̃N���X��`(�h���N���X)
//==========================================================
class CPlayer : public CTask
{
public:

	// �����ޗ񋓌^
	enum TYPE
	{
		TYPE_NONE,		// ����s�\
		TYPE_SEND,		// �f�[�^���M
		TYPE_ACTIVE,	// ����\
		TYPE_MAX
	};



private:	// �����������A�N�Z�X�\�Ȓ�`

	// ��ԗ񋓌^
	enum STATE
	{
		STATE_APPEAR = 0,	// �o�����
		STATE_NORMAL,		// �ʏ���
		STATE_NITRO	,		// �u�[�X�g���
		STATE_DAMAGE,		// �_���[�W���
		STATE_DEATH,		// ���S���
		STATE_SPAWN,		// ���������
		STATE_MAX
	};

	// ���\����
	struct SInfo
	{
		D3DXVECTOR3 pos;		// �ʒu
		D3DXVECTOR3 rot;		// ����
		D3DXVECTOR3 move;		// �ړ���
		float fSlideMove;		// �X���C�f�B���O�J�n�ړ���
		D3DXVECTOR3 posOld;		// �ݒ�ʒu
		D3DXMATRIX mtxWorld;	// ���[���h�}�g���b�N�X
		STATE state;			// ���
		float fStateCounter;	// ��ԊǗ��J�E���^�[
		CRoad* pRoad;			// �Ŋ��̓�
	};

public:	// �N�ł��A�N�Z�X�\

	CPlayer();	// �R���X�g���N�^
	~CPlayer();	// �f�X�g���N�^

	// �����o�֐�
	HRESULT Init(void);
	HRESULT Init(const char *pBodyName, const char *pLegName);	// �I�[�o�[���[�h
	void Uninit(void);
	void Update(void);
	static CPlayer *Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, const D3DXVECTOR3 move,
		const char *pBodyName, const char *pLegName);

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

	void SetNumDeliveryStatus(int nNum) { m_nNumDeliveryStatus = nNum; }
	void AddDeliveryCount(void) { m_nNumDeliveryStatus++; }
	
	// �����o�֐�(�擾)
	D3DXVECTOR3 GetMove(void) { return m_Info.move; }
	D3DXVECTOR3 GetPosition(void) { return m_Info.pos; }
	D3DXVECTOR3 GetRotation(void) { return m_Info.rot; }
	D3DXVECTOR3 GetOldPosition(void) { return m_Info.posOld; }
	CRoad* GetRoad(void) { return m_Info.pRoad; }
	CPlayer* GetNext(void) { return m_pNext; }
	CObjectX* GetObj() { return m_pObj; }
	CPlayer* GetPrev(void) { return m_pPrev; }
	int GetModelIndex(void) { return m_pObj->GetIdx(); }
	float GetEngine(void) { return m_fEngine; }
	int GetNumDeliverStatus(void) { return m_nNumDeliveryStatus; }
	float GetLifeOrigin() { return m_fLifeOrigin; }

protected:	// �����������A�N�Z�X�\

	// �����o�֐�
	void SetMatrix(void);
	void StateSet(void);
	void Controller(void);
	void Move(void);
	void Rotate(void);
	void Adjust(void);
	bool Collision(void);
	void Engine(float fThrottle);
	void SearchRoad(void);
	void Nitro();
	void GetBestPath();
	void DEBUGKEY();
	// �����o�ϐ�
	CPlayer *m_pPrev;			// �O�̃I�u�W�F�N�g�ւ̃|�C���^
	CPlayer *m_pNext;			// ���̃I�u�W�F�N�g�ւ̃|�C���^
	SInfo m_Info;				// �������g�̏��
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
	int m_nNumDeliveryStatus;  // �z�B������
};


#endif
