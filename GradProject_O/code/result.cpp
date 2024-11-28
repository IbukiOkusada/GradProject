//===============================================
//
// ���U���g��ʂ̊Ǘ����� [result.cpp]
// Author : Ibuki Okusada
//
//===============================================
#include "result.h"
#include "object2D.h"
#include "texture.h"
#include "manager.h"
#include "main.h"
#include "input.h"
#include "fade.h"
#include "ranking.h"
#include "sound.h"
#include "result.h"
#include "camera.h"
#include "ranking.h"
#include "object_manager.h"
#include "time.h"
#include "input_gamepad.h"
#include "input_keyboard.h"
#include "timer.h"
#include "player_manager.h"
#include "player.h"
#include "objectX.h"
#include "map_manager.h"


//==========================================================
// �萔��`
//==========================================================
namespace NUMBER
{
	const float HEIGHT = 60.0f;		// ����
	const float WIDTH = 30.0f;		// ����
	const float INTERVAL = 60.0f;	// �Ԋu
	const int TIME_OBJ = 3;			// �Ԋu
	const int LIFE_OBJ = 3;			// �Ԋu
	const D3DXVECTOR3 POS = D3DXVECTOR3(SCREEN_WIDTH * 0.5f, 50.0f, 0.0f);
}

// �ÓI�����o�ϐ�
int CResult::m_nScore = 0;
int CResult::m_nDeli = 0;
float CResult::m_fTime = 0.0f;
float CResult::m_fLife = 0.0f;

//===============================================
// �R���X�g���N�^
//===============================================
CResult::CResult()
{
	m_pMeshSky = NULL;
	m_pTime = NULL;
	m_pObjClear = nullptr;
	m_TimeObj[0] = {};
	m_LifeObj[0] = {};
}

//===============================================
// �f�X�g���N�^
//===============================================
CResult::~CResult()
{

}

//===============================================
// ����������
//===============================================
HRESULT CResult::Init(void)
{

	//�J����������
	{
		CManager::GetInstance()->GetCamera()->SetLength(300.0f);
		CManager::GetInstance()->GetCamera()->SetRotation(D3DXVECTOR3(0.0f, 1.0f, 1.0f));
		CManager::GetInstance()->GetCamera()->SetPositionR(D3DXVECTOR3(1000.0f, 1000.0f, 1000.0f));
		D3DVIEWPORT9 viewport;

		//�v���C���[�Ǐ]�J�����̉�ʈʒu�ݒ�
		viewport.X = 0;
		viewport.Y = 0;
		viewport.Width = (DWORD)(SCREEN_WIDTH * 1.0f);
		viewport.Height = (DWORD)(SCREEN_HEIGHT * 1.0f);
		viewport.MinZ = 0.0f;
		viewport.MaxZ = 1.0f;
		CManager::GetInstance()->GetCamera()->SetViewPort(viewport);
	}

	// �}�b�v�ǂݍ���
	CMapManager::GetInstance()->Load();

	m_pObj = CObjectX::Create(D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), "data\\MODEL\\flyingscooter.x");
	m_pObj->SetType(CObject::TYPE_PLAYER);
	m_pObj->SetRotateType(CObjectX::TYPE_QUATERNION);


	m_nDeli = CManager::GetInstance()->GetDeliveryStatus();
	m_fTime = CTimer::GetTime();
	m_fLife = CManager::GetInstance()->GetLife();


	m_pDeliObject2D = CNumber::Create(D3DXVECTOR3(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.2f, 0.0f),
		NUMBER::WIDTH,
		NUMBER::HEIGHT);
	m_pDeliObject2D->SetIdx(m_nDeli);

	for (int Cnt = 0; Cnt < 3; Cnt++)
	{
		m_pTimeObject2D[Cnt] = CNumber::Create(D3DXVECTOR3(SCREEN_WIDTH * 0.5f + NUMBER::INTERVAL * Cnt, SCREEN_HEIGHT * 0.4f, 0.0f),
			NUMBER::WIDTH,
			NUMBER::HEIGHT);
		Calculation(&m_TimeObj[Cnt], m_fTime, Cnt, NUMBER::TIME_OBJ);
		m_pTimeObject2D[Cnt]->SetIdx(m_TimeObj[Cnt]);
	}

	for (int Cnt = 0; Cnt < 3; Cnt++)
	{
		m_pLifeObject2D[Cnt] = CNumber::Create(D3DXVECTOR3(SCREEN_WIDTH * 0.5f + NUMBER::INTERVAL * Cnt, SCREEN_HEIGHT * 0.6f, 0.0f),
			NUMBER::WIDTH,
			NUMBER::HEIGHT);
		Calculation(&m_LifeObj[Cnt], m_fLife, Cnt, NUMBER::LIFE_OBJ);
		m_pLifeObject2D[Cnt]->SetIdx(m_LifeObj[Cnt]);
	}

	//CManager::GetInstance()->GetSound()->Play(CSound::LABEL_BGM_RESULT);

	return S_OK;
}

//===============================================
// �I������
//===============================================
void CResult::Uninit(void)
{
	if (m_pDeliObject2D != NULL)
	{
		m_pDeliObject2D->Uninit();
		m_pDeliObject2D = NULL;
	}

	for (int Cnt = 0; Cnt < 3; Cnt++)
	{
		if (m_pTimeObject2D[Cnt] != NULL)
		{
			m_pTimeObject2D[Cnt]->Uninit();
			m_pTimeObject2D[Cnt] = NULL;
		}
	}

	for (int Cnt = 0; Cnt < 3; Cnt++)
	{
		if (m_pLifeObject2D[Cnt] != NULL)
		{
			m_pLifeObject2D[Cnt]->Uninit();
			m_pLifeObject2D[Cnt] = NULL;
		}
	}

	CRanking::SetScore(m_nScore);
	m_nScore = 0;
	CManager::GetInstance()->GetCamera()->SetActive(true);
	CManager::GetInstance()->GetSound()->Stop();
}

//===============================================
// �X�V����
//===============================================
void CResult::Update(void)
{
	CInputKeyboard* pKey = CInputKeyboard::GetInstance();
	CInputPad* pPad = CInputPad::GetInstance();

	if (pPad->GetTrigger(CInputPad::BUTTON_A, 0) ||
		pPad->GetTrigger(CInputPad::BUTTON_START, 0) ||
		pKey->GetTrigger(DIK_RETURN))
	{
		CManager::GetInstance()->GetFade()->Set(CScene::MODE_RANKING);
	}

	CScene::Update();
}

//===============================================
// �`�揈��
//===============================================
void CResult::Draw(void)
{
	CScene::Draw();
}

//===============================================
// �i���o�[�\���̂��߂̌v�Z
//===============================================
void CResult::Calculation(int* Obj, float Score, int Cnt, int ObjMax)
{
	int nCal = 1;
	bool bFirst = false;

	if (Cnt == 0)
	{
		Cnt++;
		bFirst = true;
	}

	for(int nCnt = Cnt; nCnt < ObjMax; nCnt++)
	{
		nCal = nCal * 10;
	}

	if (bFirst == true)
	{
		Cnt--;
	}

	switch (Cnt)
	{
	case 0:
		*Obj = ((int)Score) / nCal;
		break;

	case NUMBER::LIFE_OBJ - 1:
		*Obj = ((int)Score) % nCal;
		break;

	default:
		*Obj = ((int)Score) % nCal / (nCal / 10);
		break;
	}
}