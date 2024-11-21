//===============================================
//
// �v���C���[�̏��� [player.cpp]
// Author : Ibuki Okusada
//
//===============================================
#include "player.h"
#include "manager.h"
#include "renderer.h"
#include "input.h"
#include "debugproc.h"
#include "camera.h"
#include "Xfile.h"
#include "slow.h"
#include "texture.h"
#include "meshfield.h"
#include "Xfile.h"
#include "meshorbit.h"
#include "game.h"
#include "filter.h"
#include "object2D.h"
#include "meshwall.h"
#include "objectX.h"
#include "sound.h"
#include <assert.h>
#include "billboard.h"
#include "character.h"
#include "motion.h"
#include "sound.h"
#include "model.h"
#include <math.h>
#include "meshwall.h"
#include "player_manager.h"
#include "fade.h"
#include "input_keyboard.h"
#include "input_gamepad.h"
#include "camera_manager.h"
#include "road.h"
#include "road_manager.h"
#include "collision.h"
#include "deltatime.h"
#include "bridge.h"
#include "meter.h"
#include "baggage.h"
#include "camera_action.h"
#include "baggage.h"
#include "goal.h"
#include "a_star.h"
#include "container.h"
#include "pred_route.h"
#include "scrollString2D.h"
#include "scrollText2D.h"
//===============================================
// �}�N����`
//===============================================

namespace
{
	const float DAMAGE_APPEAR = (110.0f);	// ���G���ԃC���^�[�o��
	const float DEATH_INTERVAL = (120.0f);	// ���S�C���^�[�o��
	const float SPAWN_INTERVAL = (60.0f);	// �����C���^�[�o��
	const float CAMROT_INER = (0.2f);			// �J��������
	const float KICK_LENGTH = (1000.0f);	// �U���͈�
	const float LIFE = (100.0f);
	const float CAMERA_NORMAL = (2600.0f);
	const float ENGINE_ADDCAMERA = (800.0f);
	const float CAMERA_DETAH = (6000.0f);
	const float MOVE = (6.2f * 0.7f);		// �ړ���
	const float BRAKE = (0.7f);		// �u���[�L
	const float DRIFT = (+0.3f);		// �h���t�g���̕␳��
	const float TURN = (0.006f);		// �����
	const float GRAVITY = (-12.0f);		//�v���C���[�d��
	const float ROT_MULTI = (1.0f);	// �����␳�{��
	const float WIDTH = (20.0f);	// ��
	const float HEIGHT = (80.0f);	// ����
	const float INER = (0.9f);		// ����
	const float ENGINE_INER = (0.01f);		// ����
	const float ENGINE_BRAKE = (0.006f);		// ����
	const float TUURN_INER = (0.9f);		// ����
	const float DRIFT_INER = (0.975f);		// �h���t�g����
	const float BRAKE_INER = (0.05f);
	const float RES = (1.98f);		// ����
	const float JUMP = (16.0f);
	const float REF_INER = (1.2f);	// �ǂŔ��˂��鎞�̔{��
	const float FRAME_RATE_SCALER = 60.0f;  // �t���[�����[�g���l���������x�̒���
	const float BAGGAGE_LENGTH = 200.0f;
	const float CAMERA_ENGINEMULTI = 0.15f;
	const float NITRO_COUNTER = 6.0f;
	const float NITRO_COOL = 120.0f;
	const vector<string> START_TEXT[NUM_TXT]
	{
	{">> BOOT SEQUENCE INITIATED.......................................",
	">> CORE ENGINE ONLINE : FUSION - CYCLE MK.7",
	">> BATTERY LEVEL : 97 % | RANGE ESTIMATE : 320 KM	",
	">> NEURAL LINK : SYNCHRONIZED",
	">> COOLING SYSTEM : INTEGRATED | STATUS : OPTIMAL	",
	">> MODULE CHECK :									",
	"-DELIVERY MODULE : ACTIVE							",
	"- FREEZER UNIT : LOCKED & COLD(-20��C)			   " ,
	"- AI NAVIGATION : ONLINE							",
	"",
	"",
	"",
	"WELCOME BACK, RIDER. ALL SYSTEMS GO. READY TO DELIVER."},
	{
	">> FREEZER UNIT - STATUS CHECK :										  ",
	"-TEMPERATURE : -20��C													  ",
	"- INVENTORY CAPACITY : 50 %											  ",
	">> CURRENT STOCK :														  ",
	"-VANILLA BEAM : 12 UNITS												  ",
	"- NEON STRAWBERRY : 10 UNITS											  ",
	"- CHROME CHOCO : 15 UNITS												  ",
	"- CYBER MINT : 8 UNITS													  ",
	"- GLITCH COCONUT : 5 UNITS												  ",
	"																		  ",
	">> WARNING : COOLANT RESERVOIR BELOW 15 % .REPLENISH TO AVOID TEMP SPIKE.",
	},
	{
	">> DELIVERY MANAGEMENT SYSTEM :							   ",
	">> PENDING DELIVERIES :										   ",
	"1. CLIENT : SILVER SKY RESORT | ITEM : 6x NEON STRAWBERRY		   ",
	"STATUS : PACKED & READY | ETA : 15 MIN							   ",
	"2. CLIENT : AXON PLAZA | ITEM : 3x CYBER MINT, 2x GLITCH COCONUT  ",
	"STATUS : PENDING | ETA : 35 MIN								   ",
	"3. CLIENT : DARK CIRCUIT CLUB | ITEM : 10x CHROME CHOCO		   ",
	"STATUS : PACKED | ETA : 50 MIN									   ",
	"																   ",
	">> ROUTE OPTIMIZATION :										   ",
	"CALCULATING...													   ",
	"SUGGESTED ROUTE : 1 �� 3 �� 2 (LOW TRAFFIC ZONES)					",
	""
	">> ALERT : HIGHWAY 42 BLOCKED.RECOMMENDED DETOUR VIA SKYLINE LANE.",
	},
	{
	">> ALL MODULES GREEN.NAVIGATION LOCKED.",
	">> SYSTEM MESSAGE : ICE DELIVERED COLD, HEARTS WARMED. GOOD LUCK OUT THERE, RIDER.			                      ",
	">> ENGAGE ENGINE IN 3............ 2............ 1............                               ",
	">> [NEON TRAIL ACTIVATED]                                                                   ",
	}
	};
}

// �O���錾

//===============================================
// �R���X�g���N�^
//===============================================
//CPlayer::CPlayer()
//{
//	// �l���N���A����
//	m_nCounterAnim = 0;
//	m_nPatternAnim = 0;
//}

//===============================================
// �R���X�g���N�^(�I�[�o�[���[�h)
//===============================================
CPlayer::CPlayer()
{
	// �l���N���A����
	m_Info.pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_Info.posOld = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_Info.rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_Info.move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_Info.pRoad = nullptr;
	m_fRotMove = 0.0f;
	m_fRotDiff = 0.0f;
	m_fRotDest = 0.0f;
	m_fBrake = 0.0f;
	m_fEngine = 0.0f;
	m_fTurnSpeed = 0.0f;
	m_fHandle = 0.0f;
	m_fLife = LIFE;
	m_fLifeOrigin = m_fLife;
	m_fCamera = CAMERA_NORMAL;
	m_type = TYPE_NONE;
	m_nId = -1;
	m_fNitroCool = 0.0f;
	m_Info.fSlideMove = 0.0f;
	m_pObj = nullptr;
	m_pPrev = nullptr;
	m_pNext = nullptr;
	m_pDamageEffect = nullptr;
	m_pSound = nullptr;
	m_pBaggage = nullptr;	
	m_pPredRoute = nullptr;
	m_fbrakePitch = 0.0f;
	m_fbrakeVolume = 0.0f;
	m_nNumDeliveryStatus = 0;
	m_pAfterburner = CEffekseer::GetInstance()->Create("data\\EFFEKSEER\\afterburner.efkefc", VECTOR3_ZERO, VECTOR3_ZERO, VECTOR3_ZERO, 45.0f, false, false);
	m_pTailLamp = CEffekseer::GetInstance()->Create("data\\EFFEKSEER\\taillamp.efkefc", VECTOR3_ZERO, VECTOR3_ZERO, VECTOR3_ZERO, 45.0f, false, false);
	m_pBackdust = CEffekseer::GetInstance()->Create("data\\EFFEKSEER\\backdust.efkefc", VECTOR3_ZERO, VECTOR3_ZERO, VECTOR3_ZERO, 45.0f, false, false);
	m_pCollSound = CMasterSound::CObjectSound::Create("data\\SE\\collision.wav", 0);
	m_pCollSound->Stop();
	CPlayerManager::GetInstance()->ListIn(this);
}

//===============================================
// �f�X�g���N�^
//===============================================
CPlayer::~CPlayer()
{
	
}

//===============================================
// ����������
//===============================================
HRESULT CPlayer::Init(void)
{
	// ���̐���
	m_Info.state = STATE_APPEAR;
	m_type = TYPE_NONE;


	
	return S_OK;
}

//===============================================
// ����������(�I�[�o�[���[�h)
//===============================================
HRESULT CPlayer::Init(const char *pBodyName, const char *pLegName)
{
	m_pObj = CObjectX::Create(D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), "data\\MODEL\\flyingscooter.x");
	m_pObj->SetType(CObject::TYPE_PLAYER);
	m_pObj->SetRotateType(CObjectX::TYPE_QUATERNION);
	SetMatrix();
	m_pSound = CMasterSound::CObjectSound::Create("data\\SE\\idol.wav", -1);
	m_pSound->SetVolume(0.0f);
	m_pSoundBrake = CMasterSound::CObjectSound::Create("data\\SE\\flight.wav", -1);
	m_pSoundBrake->SetVolume(0.0f);
	pRadio = CRadio::Create();
	m_pNavi = CNavi::Create();
	CContainer::Create();
	m_pPredRoute = CPredRoute::Create(this);
	m_pFont[0] = CScrollText2D::Create("data\\FONT\\x12y16pxMaruMonica.ttf", false, D3DXVECTOR3(400.0f, 200.0f, 0.0f),0.0025f,20.0f, 20.0f, XALIGN_LEFT, YALIGN_TOP);
	m_pFont[1] = CScrollText2D::Create("data\\FONT\\x12y16pxMaruMonica.ttf", false, D3DXVECTOR3(500.0f, 150.0f, 0.0f), 0.0025f, 15.0f, 15.0f, XALIGN_LEFT, YALIGN_TOP);
	m_pFont[2] = CScrollText2D::Create("data\\FONT\\x12y16pxMaruMonica.ttf", false, D3DXVECTOR3(50.0f, 50.0f, 0.0f), 0.001f, 15.0f, 15.0f, XALIGN_LEFT, YALIGN_TOP);
	m_pFont[3] = CScrollText2D::Create("data\\FONT\\x12y16pxMaruMonica.ttf", false, D3DXVECTOR3(300.0f, 300.0f, 0.0f), 0.025f, 20.0f, 20.0f, XALIGN_LEFT, YALIGN_TOP,VECTOR3_ZERO,D3DXCOLOR(0.0f,1.0f,1.0f,1.0f));
	for (int i = 0; i < NUM_TXT; i++)
	{
		for (int j = 0; j < START_TEXT[i].size(); j++)
		{
			m_pFont[i]->PushBackString(START_TEXT[i][j]);
		}
	}
	return S_OK;
}

//===============================================
// �I������
//===============================================
void CPlayer::Uninit(void)
{	
	SAFE_UNINIT(m_pObj);
	SAFE_UNINIT(m_pBaggage);
	SAFE_DELETE(m_pTailLamp);
	SAFE_DELETE(m_pBackdust);
	SAFE_DELETE(m_pAfterburner);
	SAFE_DELETE(m_pDamageEffect);
	SAFE_UNINIT_DELETE(m_pSound);
	SAFE_UNINIT_DELETE(m_pSoundBrake);
	SAFE_UNINIT_DELETE(pRadio);
	SAFE_UNINIT_DELETE(m_pCollSound);
	CPlayerManager::GetInstance()->ListOut(this);

	// �p��
	Release();
}

//===============================================
// �X�V����
//===============================================
void CPlayer::Update(void)
{	
	DEBUGKEY();
	for (int i = 0; i < NUM_TXT; i++)
	{
		if (CManager::GetInstance()->GetFade()->GetState() == CFade::STATE_NONE && !m_pFont[i]->GetEnd())
		{
			if (i>0)
			{
				if (!m_pFont[i - 1]->GetEnd())continue;
			}
			m_pFont[i]->SetEnableScroll(true);
		}
		else
		{
			if (m_pFont[i]->GetNumString() > 0 && m_pFont[i]->GetEnd())
			{
				m_pFont[i]->DeleteString(0);
			}
		}
	}

	// �O��̍��W���擾
	m_Info.posOld = GetPosition();

	StateSet();	
	pRadio->Update();
	if (m_type == TYPE_ACTIVE)
	{
		// �v���C���[����
		Controller();

		// �Ŋ��̓�����
		//SearchRoad();

		// �����蔻��
		Collision();

		// �I�����C�����M
		CManager::GetInstance()->GetScene()->SendPosition(m_Info.pos);
		CManager::GetInstance()->GetScene()->SendRotation(m_Info.rot);
	}
	
	// �}�g���b�N�X
	SetMatrix();

	if (m_pObj != nullptr)
	{
		D3DXVECTOR3 rot = GetRotation();
		D3DXVECTOR3 pos = GetPosition();
		pos.y += 100.0f;
		rot.y -= D3DX_PI * 0.5f;
		rot.z += m_fTurnSpeed * 15.0f;
		m_pObj->SetPosition(pos);
		m_pObj->SetRotation(rot);
		m_pObj->SetShadowHeight(GetPosition().y);
		// �G�t�F�N�g
		{
			m_pTailLamp->m_pos = pos;
			m_pTailLamp->m_rot = rot;
			m_pBackdust->m_pos = GetPosition();
			m_pBackdust->m_rot = m_pObj->GetRotation();
			m_pBackdust->m_Scale =VECTOR3_ONE* m_fEngine * 300.0f;
			
			m_pAfterburner->m_pos = GetPosition();;
		
			m_pAfterburner->m_Scale = VECTOR3_ONE * m_fEngine * m_fBrake * 150.0f;
		
		}
		if (m_pDamageEffect != nullptr)
		{
			m_pDamageEffect->m_pos = pos;
			m_pDamageEffect->m_rot = rot;
		}
	}

	// �ו�������
	if (m_pBaggage != nullptr)
	{
		D3DXVECTOR3 rot = GetRotation();
		rot.y -= D3DX_PI * 0.5f;
		CCamera* pCamera = CCameraManager::GetInstance()->GetTop();
		pCamera->Pursue(GetPosition(), rot, m_fCamera);
	}

	// �J�����̋����ݒ�
	if (m_fLife<=0)
	{
		// ���S�J����
		m_Info.move *= 0.7f;
		m_fCamera += (CAMERA_DETAH - m_fCamera) * 0.02f;
	}
	else
	{
		// ���x�ɂ���ĕ␳�J����
		float engine = m_fEngine;
		if (engine < CAMERA_ENGINEMULTI) { engine = 0.0f; }
		else { engine -= CAMERA_ENGINEMULTI; }
		CDebugProc::GetInstance()->Print("engine [ %f ]", engine);
		m_fCamera = CAMERA_NORMAL + ENGINE_ADDCAMERA * engine;
	}

	if (CBaggage::GetThrowList()->GetNum() == 0 && m_pBaggage == nullptr)
	{
		m_pBaggage = CBaggage::Create(GetPosition());
	}

	if (m_pBaggage != nullptr)
	{
		D3DXVECTOR3 pos = GetPosition();

		pos.y += 100.0f;
		m_pBaggage->SetPosition(pos);
		m_pBaggage->GetObj()->SetShadowHeight(GetPosition().y);
	}

	// �f�o�b�O�\��
	CDebugProc::GetInstance()->Print("�v���C���[ :");
	CDebugProc::GetInstance()->Print("���W: [ %f, %f, %f ]", m_Info.pos.x, m_Info.pos.y, m_Info.pos.z);
	CDebugProc::GetInstance()->Print(" : ����: [ %f, %f, %f ]\n", m_pObj->GetRotation().x, m_pObj->GetRotation().y, m_pObj->GetRotation().z);
}

//===============================================
// ����
//===============================================
CPlayer *CPlayer::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move, const char *pBodyName, const char *pLegName)
{
	CPlayer *pPlayer = nullptr;

	// �I�u�W�F�N�g2D�̐���
	pPlayer = DEBUG_NEW CPlayer;

	if (nullptr != pPlayer)
	{// �����ł����ꍇ
		// ����������
		pPlayer->Init(pBodyName, pLegName);

		// ���W�ݒ�
		pPlayer->SetPosition(pos);

		// �����ݒ�
		pPlayer->SetRotation(rot);

		pPlayer->m_fRotDest = rot.y;

		// �ړ��ʐݒ�
		pPlayer->SetMove(move);
	}
	else
	{// �����Ɏ��s�����ꍇ
		return nullptr;
	}

	return pPlayer;
}

//===============================================
// ���쏈��
//===============================================
void CPlayer::Controller(void)
{
	D3DXVECTOR3 rot = GetRotation();	// �������擾
	m_fRotMove = rot.y;	//���݂̌������擾

	// �ړ�
	Move();

	// ��]
	Rotate();

	// �����␳
	Adjust();
}

//===============================================
// �ړ�
//===============================================
void CPlayer::Move(void)
{
	CInputKeyboard *pInputKey = CInputKeyboard::GetInstance();	// �L�[�{�[�h�̃|�C���^
	CInputPad *pInputPad = CInputPad::GetInstance();
	float fThrottle = 0.0f;
	m_fBrake = 0.0f;
	if (pInputKey->GetPress(DIK_W))
	{
		fThrottle = 0.8f;
	}
	else
	{
		fThrottle = (float)pInputPad->GetRightTriggerPress(0) / 255;
	}

	if (pInputKey->GetPress(DIK_S))
	{
		m_fBrake = 1.0f;
	}
	else
	{
		m_fBrake = (float)pInputPad->GetLeftTriggerPress(0) / 255;
	
	}
	Nitro(); 
	Engine(fThrottle);
	// ���͑��u�m�F
	if (nullptr == pInputKey)
	{
		return;
	}

	// �f���^�^�C���擾
	float deltatime = CManager::GetInstance()->GetDeltaTime()->GetDeltaTime();
	
	m_Info.pos += m_Info.move * FRAME_RATE_SCALER * deltatime;
	float fHandle = m_fHandle;
	if (fHandle < 0.0f)
	{
		fHandle *= -1;
	}
	float fEngine = m_fEngine;
	if (fEngine > 1.0f)
	{
		fEngine = 1.0f;
	}
	float fIner = INER + (fEngine * m_fBrake * fHandle) * (DRIFT_INER - INER);
	m_fbrakeVolume += (1.0f - m_fbrakeVolume) * (m_fEngine * m_fBrake) * BRAKE_INER;
	m_fbrakePitch += (1.0f - m_fbrakePitch) * (m_fEngine * m_fBrake * fHandle) * BRAKE_INER;
	m_fbrakeVolume -= m_fbrakeVolume* BRAKE_INER;
	m_fbrakePitch -= m_fbrakePitch *BRAKE_INER;
	m_pSoundBrake->SetVolume(m_fbrakeVolume * 0.8f);
	m_pSoundBrake->SetPitch(0.5f + m_fbrakePitch);
	float moveY = m_Info.move.y;
	m_Info.move *= fIner;//�ړ��ʂ̌���
	m_Info.move.y = moveY;
	m_Info.move.y += GRAVITY * deltatime;

}
//===============================================
//�ȈՃG���W���V�~��
//===============================================
void  CPlayer::Engine(float fThrottle)
{
	float fAccel = (1.0f - m_fEngine) * fThrottle * ENGINE_INER;
	m_fEngine += fAccel;
	if (fThrottle <= 0)
	{
		m_fEngine -= ENGINE_BRAKE;
		if (m_fEngine <= 0)
		{
			m_fEngine = 0;
		}
	}
	//��]�����特�ʂƃs�b�`�𑀍�
	m_pSound->SetPitch(0.5f + m_fEngine*1.5f);
	m_pSound->SetVolume(0.5f + fAccel*100.0f + m_fEngine);

	m_pSoundBrake->SetVolume(m_fBrake * m_fEngine * 0.3f);
	//����
	m_Info.move.z += MOVE * sinf(-m_Info.rot.y) * m_fEngine;
	m_Info.move.x += MOVE * cosf(-m_Info.rot.y) * m_fEngine;
	//�u���[�L
	m_Info.move.z -= MOVE * sinf(-m_Info.rot.y) * m_fEngine * m_fBrake * BRAKE;
	m_Info.move.x -= MOVE * cosf(-m_Info.rot.y) * m_fEngine * m_fBrake * BRAKE;
	CManager::GetInstance()->GetDebugProc()->Print("fThrottle:%f\nm_fEngine%f\n", fThrottle, m_fEngine);
}
//===============================================
// ����
//===============================================
void CPlayer::Rotate(void)
{
	float slowmulti = CManager::GetInstance()->GetDeltaTime()->GetSlow();
	CInputKeyboard* pInputKey = CInputKeyboard::GetInstance();	// �L�[�{�[�h�̃|�C���^
	CInputPad* pInputPad = CInputPad::GetInstance();
	float diff = 0.0f;

	if (pInputKey->GetPress(DIK_D))
	{
		diff = 1.0f;
	}
	else if (pInputKey->GetPress(DIK_A))
	{
		diff = -1.0f;
	}
	else
	{
		diff = pInputPad->GetLStick(0, 0.1f).x;
	}

	float dest = (diff - m_fHandle) * slowmulti;
	m_fHandle += dest * slowmulti;
	
	m_fTurnSpeed += TURN * m_fHandle * (1.0f + m_fBrake * m_fEngine * DRIFT);
	m_fTurnSpeed*= TUURN_INER;
	// ���͑��u�m�F
	if (nullptr == pInputKey) 
	{
		return;
	}

	// m_fRotDiff�̒l�ς��Ă����Ε␳������
}

//===============================================
// ����
//===============================================
void CPlayer::Adjust(void)
{

	m_Info.rot.y += m_fTurnSpeed;

	while (1)
	{
		if (m_Info.rot.y > D3DX_PI || m_Info.rot.y < -D3DX_PI)
		{//-3.14�`3.14�͈̔͊O�̏ꍇ
			if (m_Info.rot.y > D3DX_PI)
			{
				m_Info.rot.y += (-D3DX_PI * 2);
			}
			else if (m_Info.rot.y < -D3DX_PI)
			{
				m_Info.rot.y += (D3DX_PI * 2);
			}
		}
		else
		{
			break;
		}
	}
}

//===============================================
// �Ŋ��̓�����
//===============================================
void CPlayer::SearchRoad()
{
	CRoadManager* pRoadManager = CRoadManager::GetInstance();
	auto list = pRoadManager->GetList();
	CRoad* pRoad = list->Get(0);
	CRoad* pRoadClose = list->Get(0);
	

	if (pRoad == nullptr) 
	{
		return;
	}

	float length = D3DXVec3Length(&(pRoadClose->GetPosition() - m_Info.pos));
	float lengthClose = 0.0f;

	for (int i = 0; i < list->GetNum() - 1; i++)
	{
		pRoad = list->Get(i);

		// �������菈��
		lengthClose = D3DXVec3Length(&(pRoad->GetPosition() - m_Info.pos));

		if (length > lengthClose)
		{
			length = lengthClose;
			pRoadClose = pRoad;
		}
	}

	m_Info.pRoad = pRoadClose;
}

//===============================================
// �����蔻�菈��
//===============================================
bool CPlayer::Collision(void)
{
	auto mgr = CObjectX::GetList();
	for (int i = 0; i < mgr->GetNum(); i++)
	{// �g�p����Ă��Ȃ���Ԃ܂�

		CObjectX* pObjectX = mgr->Get(i);	// �擪���擾

		// �Փ˔�������Ȃ�
		if(!pObjectX->GetEnableCollision()){
			continue;
		}

		D3DXVECTOR3 posObjectX = pObjectX->GetPosition();
		D3DXVECTOR3 rotObjectX = pObjectX->GetRotation();
		D3DXVECTOR3 sizeMax = pObjectX->GetVtxMax();
		D3DXVECTOR3 sizeMin = pObjectX->GetVtxMin();
		D3DXVECTOR3 pVecCollision;

		bool bCollision = collision::ReflectPointToOBB(&pVecCollision, &m_Info.pos, &m_Info.move, m_Info.posOld, posObjectX, rotObjectX, (sizeMax - sizeMin) * 0.5f, REF_INER);

		if (bCollision)
		{
			CEffekseer::GetInstance()->Create("data\\EFFEKSEER\\spark.efkefc", (m_Info.pos + m_Info.move), VECTOR3_ZERO, VECTOR3_ZERO, 300.0f);
			m_pCollSound->SetVolume(m_fEngine*2.0f);
			m_pCollSound->Play();
			pObjectX->SetHit(true);
			D3DXVECTOR3 vecMoveNor = m_Info.move;
			D3DXVec3Normalize(&vecMoveNor, &m_Info.move);
			D3DXVec3Normalize(&pVecCollision, &pVecCollision);

			float engineLow = fabsf(D3DXVec3Dot(&pVecCollision, &vecMoveNor) * 0.1f);
			m_fEngine *= 0.9f + engineLow;

			return true;
		}
	}

	m_Info.pRoad = nullptr;

	// ���Ƃ̔���
	auto listRoad = CRoadManager::GetInstance()->GetList();
	for (int i = 0; i < listRoad->GetNum(); i++)
	{// �g�p����Ă��Ȃ���Ԃ܂�

		// ���m�F
		CRoad* pRoad = listRoad->Get(i);	// �擪���擾
		if (pRoad == nullptr) { continue; }

		D3DXVECTOR3* pVtx = pRoad->GetVtxPos();
		D3DXVECTOR3 pos = pRoad->GetPosition();
		float height = m_Info.pos.y - 0.1f;;
		D3DXVECTOR3 vec1 = pVtx[1] - pVtx[0], vec2 = pVtx[2] - pVtx[0];
		D3DXVECTOR3 nor0, nor1;

		D3DXVec3Cross(&nor0, &vec1, &vec2);
		D3DXVec3Normalize(&nor0, &nor0);	// �x�N�g���𐳋K������

		vec1 = pVtx[2] - pVtx[3];
		vec2 = pVtx[1] - pVtx[3];
		D3DXVec3Cross(&nor1, &vec1, &vec2);
		D3DXVec3Normalize(&nor1, &nor1);	// �x�N�g���𐳋K������

		// ����
		collision::IsOnSquarePolygon(pos + pVtx[0], pos + pVtx[1], pos + pVtx[2], pos + pVtx[3],
			nor0, nor1, m_Info.pos, m_Info.posOld, height);

		if (height >= m_Info.pos.y)
		{
			m_Info.pos.y = height;
			m_Info.move.y = 0.0f;
			m_Info.pRoad = pRoad;
		}
	}

	auto listGimmick = CGimmick::GetList();
	for (int i = 0; i < listGimmick->GetNum(); i++)
	{// �g�p����Ă��Ȃ���Ԃ܂�

		// ���m�F
		CGimmick* pGimmick = listGimmick->Get(i);	// �擪���擾
		if (pGimmick == nullptr) { continue; }

		if (pGimmick->GetType() == CGimmick::TYPE_BRIDGE)
		{
			pGimmick->GetPos();
		}
	}
	return false;
}

//===============================================
// �_���[�W
//===============================================
void CPlayer::Damage(float fDamage)
{
	m_fLife -= fDamage;
	if (m_fLife <= 0.0f)
	{
		SAFE_DELETE(m_pDamageEffect);
		m_pDamageEffect = CEffekseer::GetInstance()->Create("data\\EFFEKSEER\\explosion.efkefc", VECTOR3_ZERO, VECTOR3_ZERO, VECTOR3_ZERO, 120.0f, false, false);
	}
	else if (m_fLife <= LIFE * 0.5f)
	{
		SAFE_DELETE(m_pDamageEffect);
		m_pDamageEffect = CEffekseer::GetInstance()->Create("data\\EFFEKSEER\\moderately_damage.efkefc", VECTOR3_ZERO, VECTOR3_ZERO, VECTOR3_ZERO, 60.0f, false, false);
	}
	else if (m_fLife <= LIFE * 0.8f)
	{
		SAFE_DELETE(m_pDamageEffect);
		m_pDamageEffect = CEffekseer::GetInstance()->Create("data\\EFFEKSEER\\minor_damage.efkefc", VECTOR3_ZERO, VECTOR3_ZERO, VECTOR3_ZERO, 60.0f, false, false);
	}
}
//===============================================
// ����
//===============================================
void CPlayer::Nitro()
{
	CInputKeyboard* pInputKey = CInputKeyboard::GetInstance();	// �L�[�{�[�h�̃|�C���^
	CInputPad* pInputPad = CInputPad::GetInstance();
	if (m_fNitroCool==0.0f&& (pInputKey->GetTrigger(DIK_SPACE)|| pInputPad->GetTrigger(CInputPad::BUTTON_B,0)))
	{
 		Damage(LIFE * 0.1f);
		if (m_fLife > 0.0f)
		{
			CManager::GetInstance()->GetRenderer()->SetEnableDrawMultiScreen(0.3f, 1.035f, 2.0f);
		}
		m_Info.state = STATE::STATE_NITRO;
		m_Info.fStateCounter = NITRO_COUNTER;
		m_fNitroCool = NITRO_COOL;
	}
	if (m_Info.state == STATE::STATE_NITRO)
	{
		m_fEngine = 1.5f;
	}
	else
	{
		m_fNitroCool--;
		if (m_fNitroCool == NITRO_COOL - 1.0f)
		{
			CManager::GetInstance()->GetRenderer()->SetEnableDrawMultiScreen(0.0f, 1.0f, 320.0f);
		}
		if (m_fNitroCool < 0.0f)
		{
			m_fNitroCool = 0.0f;
		}
	}
	
}
void CPlayer::GetBestPath()
{
	Clist<CRoad*>* List = CRoadManager::GetInstance()->GetList();
	float fDis = FLT_MAX;
	CRoad* pStart = nullptr;
	for (int i = 0; i < List->GetNum(); i++)
	{
		float F = GetDistance(List->Get(i)->GetPosition(), GetPosition());
		if (F<fDis)
		{
			fDis = F;
			pStart = List->Get(i);
		}
	}
	
}
//===============================================
// �f�o�b�O�L�[
//===============================================
#if _DEBUG
void CPlayer::DEBUGKEY(void)
{
	CInputKeyboard* pInputKey = CInputKeyboard::GetInstance();	// �L�[�{�[�h�̃|�C���^
	if (pInputKey->GetTrigger(DIK_K))
	{
		Damage(40.0f);
	}
	if (pInputKey->GetTrigger(DIK_B))
	{
		GetBestPath();
	}
}
#else
void CPlayer::DEBUGKEY(void){}
#endif
//===============================================
// ��ԊǗ�
//===============================================
void CPlayer::StateSet(void)
{
	float fSlawMul = CDeltaTime::GetInstance()->GetSlow();
	switch (m_Info.state)
	{
	case STATE_APPEAR:
	{
		m_Info.fStateCounter -= fSlawMul;

		if (m_Info.fStateCounter <= 0.0f)
		{
			m_Info.fStateCounter = 0.0f;
			m_Info.state = STATE_NORMAL;
		}
	}
		break;

	case STATE_NORMAL:
	{

	}
		break;
	case STATE_NITRO:
	{
		/*if (m_Info.fStateCounter == NITRO_COUNTER - fSlawMul)
		{
			CManager::GetInstance()->GetRenderer()->SetEnableDrawMultiScreen(0.0f, 1.0f, 180.0f);
		}*/

		m_Info.fStateCounter -= fSlawMul;

		if (m_Info.fStateCounter <= 0.0f)
		{
			m_Info.fStateCounter = 0.0f;
			m_Info.state = STATE_NORMAL;
		}
	}
	break;

	case STATE_DAMAGE:
	{
		m_Info.fStateCounter -= fSlawMul;

		if (m_Info.fStateCounter <= 0.0f)
		{
			m_Info.fStateCounter = DAMAGE_APPEAR;
			m_Info.state = STATE_APPEAR;
		}
	}
		break;

	case STATE_DEATH:
	{
		m_Info.fStateCounter -= fSlawMul;

		if (m_Info.fStateCounter <= 0.0f)
		{
			m_Info.fStateCounter = DAMAGE_APPEAR;
			m_Info.state = STATE_SPAWN;
		}
	}
		break;

	case STATE_SPAWN:
	{
		m_Info.fStateCounter -= fSlawMul;

		if (m_Info.fStateCounter <= 0.0f)
		{
			m_Info.fStateCounter = SPAWN_INTERVAL;
			m_Info.state = STATE_APPEAR;
		}
	}
		break;

	}
}

//===============================================
// �̗͐ݒ�
//===============================================
void CPlayer::SetType(TYPE type)
{
	m_type = type;
}

//===============================================
// �}�g���b�N�X�ݒ�
//===============================================
void CPlayer::SetMatrix(void)
{
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();	//�f�o�C�X�ւ̃|�C���^���擾
	D3DXMATRIX mtxRot, mtxTrans;	//�v�Z�p�}�g���b�N�X

	//���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&m_Info.mtxWorld);

	//�����𔽉f
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_Info.rot.y, m_Info.rot.x, m_Info.rot.z);
	D3DXMatrixMultiply(&m_Info.mtxWorld, &m_Info.mtxWorld, &mtxRot);

	//�ʒu�𔽉f
	D3DXMatrixTranslation(&mtxTrans, m_Info.pos.x, m_Info.pos.y, m_Info.pos.z);
	D3DXMatrixMultiply(&m_Info.mtxWorld, &m_Info.mtxWorld, &mtxTrans);

	//���[���h�}�g���b�N�X�̐ݒ�
	pDevice->SetTransform(D3DTS_WORLD, &m_Info.mtxWorld);
}

//===============================================
// �w�胂�[�V�����ɐݒ�
//===============================================
void CPlayer::SetMotion(int nMotion)
{
	
	
}

//===============================================
// �v���C���[�̕`��ݒ�
//===============================================
void CPlayer::SetDraw(bool bDraw)
{
	
}

//===============================================
// �ו��𓊂���
//===============================================
CBaggage* CPlayer::ThrowBaggage(D3DXVECTOR3* pTarget)
{
	if (pTarget == nullptr) { return nullptr; }

	/*CCamera* pCamera = CCameraManager::GetInstance()->GetTop();
	float rotY = atan2f(m_Info.pos.z - pTarget->z, m_Info.pos.x - pTarget->x);
	pCamera->GetAction()->Set(pCamera, D3DXVECTOR3(0.0f, rotY, D3DX_PI * 0.35f), 600.0f, 0.5f, 1.0f, CCameraAction::MOVE_POSV, true);*/

	D3DXVECTOR3 pos = GetPosition();

	pos.y += 100.0f;

	// �g�p����Ă��Ȃ�
	if (CBaggage::GetList()->GetNum() == 0 || m_pBaggage == nullptr)
	{
		m_pBaggage = CBaggage::Create(pos);
	}

	CManager::GetInstance()->GetRenderer()->SetEnableDrawMultiScreen(0.65f, 0.95f, 1.0f);

	// �ו��𓊂���
	CBaggage* pBag = m_pBaggage;
	m_pBaggage->Set(pos, pTarget, 0.75f);
	m_pBaggage = nullptr;

	return pBag;
}
