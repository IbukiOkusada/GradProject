//==========================================================
//
// �X�s�[�h���[�^�[�̏��� [speedmeter.cpp]
// Author : Ryosuke Ohara
//
//==========================================================
#include "speedmeter.h"
#include "object2D.h"
#include "player.h"
#include "player_manager.h"
#include "texture.h"
#include "manager.h"
#include "deltatime.h"
#include "debugproc.h"

// �ÓI�����o�ϐ��錾
CSpeedMeter* CSpeedMeter::m_pInstance = nullptr;

//==========================================================
// �萔��`
//==========================================================
namespace FRAME
{
	const float HEIGHT = 100.0f;  // ����
	const float WIDTH = 100.0f;   // ����
	const D3DXVECTOR3 POS = D3DXVECTOR3(SCREEN_WIDTH * 0.8f, SCREEN_HEIGHT * 0.8f, 0.0f);
	const char* TEX_PATH = "data\\TEXTURE\\speed_limit.png";
}

namespace NEEDLE
{
	const float HEIGHT = 70.0f;  // ����
	const float WIDTH = 20.0f;   // ����
	const float START = -D3DX_PI * 0.7f;
	const float LIMIT = D3DX_PI * 0.7f;
	const float DEST = (LIMIT - START);
	const D3DXVECTOR3 POS = D3DXVECTOR3(SCREEN_WIDTH * 0.8f, SCREEN_HEIGHT * 0.8f, 0.0f);
	const D3DXVECTOR3 ROT = D3DXVECTOR3(0.0f, 0.0f, D3DX_PI * 0.7f);
	const char* TEX_PATH = "data\\TEXTURE\\bill_wall.jpg";
}

//==========================================================
// �R���X�g���N�^
//==========================================================
CSpeedMeter::CSpeedMeter()
{
	m_pFrame = nullptr;
	m_pNeedle = nullptr;

	m_pInstance = this;
}

//==========================================================
// �f�X�g���N�^
//==========================================================
CSpeedMeter::~CSpeedMeter()
{
}

//==========================================================
// ��������
//==========================================================
CSpeedMeter* CSpeedMeter::Create(void)
{
	CSpeedMeter* pSpeedMeter = DEBUG_NEW CSpeedMeter;

	if (pSpeedMeter != nullptr)
	{
		pSpeedMeter->Init();
	}

	return pSpeedMeter;
}

//==========================================================
// ����������
//==========================================================
HRESULT CSpeedMeter::Init(void)
{
	if (m_pFrame == nullptr)
	{
		m_pFrame = CObject2D::Create();
		m_pFrame->SetPosition(FRAME::POS);
		m_pFrame->SetSize(FRAME::WIDTH, FRAME::HEIGHT);
		m_pFrame->BindTexture(CManager::GetInstance()->GetTexture()->Regist(FRAME::TEX_PATH));
		m_pFrame->SetDraw();
	}

	if (m_pNeedle == nullptr)
	{
		m_pNeedle = CObject2D::Create();
		m_pNeedle->SetPosition(NEEDLE::POS);
		m_pNeedle->SetRotation(NEEDLE::ROT);
		m_pNeedle->SetSize(NEEDLE::WIDTH, NEEDLE::HEIGHT);
		m_pNeedle->SetVtx(NEEDLE::WIDTH, NEEDLE::HEIGHT);
		m_pNeedle->BindTexture(CManager::GetInstance()->GetTexture()->Regist(NEEDLE::TEX_PATH));
		m_pNeedle->SetDraw();
	}

	return S_OK;
}

//==========================================================
// �I������
//==========================================================
void CSpeedMeter::Uninit(void)
{
	// ���[�^�[�̘g�̔j��
	if (m_pFrame != nullptr)
	{
		m_pFrame->Uninit();
		m_pFrame = nullptr;
	}

	// ���[�^�[�̐j�̔j��
	if (m_pNeedle != nullptr)
	{
		m_pNeedle->Uninit();
		m_pNeedle = nullptr;
	}

	// ���g�̔j��
	if (m_pInstance != nullptr)
	{
		m_pInstance = nullptr;
	}

	Release();
}

//==========================================================
// �X�V����
//==========================================================
void CSpeedMeter::Update(void)
{
	CPlayer* pPlayer = CPlayerManager::GetInstance()->GetTop();

	float fEngineRate = (pPlayer->GetEngine() / 1.0f);

	/*if (NEEDLE::ROT.z <= fEngineRate)
		fEngineRate = NEEDLE::ROT.z;*/

	if (m_pNeedle != nullptr)
	{
		float fRot = NEEDLE::START + fEngineRate * NEEDLE::DEST;

		m_pNeedle->SetRotation({ 0.0f , 0.0f,  -fRot });

		m_pNeedle->SetVtx(NEEDLE::WIDTH, NEEDLE::HEIGHT);
	}

	float t = CDeltaTime::GetInstance()->GetDestTime();

	CManager::GetInstance()->GetDebugProc()->Print("[ ���� : %f ]\n", t);
}

//==========================================================
// �`�揈��
//==========================================================
void CSpeedMeter::Draw(void)
{

}