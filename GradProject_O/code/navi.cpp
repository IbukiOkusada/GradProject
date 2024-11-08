//==========================================================
//
// �i�r [navi.cpp]
// Author : �O�엳�V��
//
//==========================================================
#include "navi.h"
#include "player.h"
#include "player_manager.h"
#include "manager.h"
#include "object.h"
#include "object_manager.h"
// �}�N����`
namespace
{
 	const D3DVIEWPORT9 VIEWPORT = (D3DVIEWPORT9{ (DWORD)(SCREEN_WIDTH * 0.7f), (DWORD)(SCREEN_WIDTH * 0.7f), 100,100,1.0,1000});
}
//==========================================================
// �R���X�g���N�^
//==========================================================
CNavi::CNavi()
{
	// �l�̃N���A
}

//==========================================================
// �f�X�g���N�^
//==========================================================
CNavi::~CNavi()
{

}

//==========================================================
// ����������
//==========================================================
HRESULT CNavi::Init(void)
{
	m_pCamera = DEBUG_NEW CMultiCamera;
	m_pCamera->Init();
	m_pCamera->SetViewPort(VIEWPORT);
	return S_OK;
}

//==========================================================
// �I������
//==========================================================
void CNavi::Uninit(void)
{
	SAFE_UNINIT_DELETE(m_pCamera);
	Release();
}

//==========================================================
// �X�V����
//==========================================================
void CNavi::Update(void)
{
	CPlayer* pPlayer = CPlayerManager::GetInstance()->GetTop();
	D3DXVECTOR3 pos = pPlayer->GetPosition();
	m_pCamera->SetPositionR(pos);
	pos.y += 1000.0f;
	m_pCamera->SetPositionV(pos);

}

//==========================================================
// ����
//==========================================================
CNavi* CNavi::Create(void)
{
	CNavi* pNavi = nullptr;

	pNavi = DEBUG_NEW CNavi;

	if (pNavi != nullptr)
	{
		// ����������
		pNavi->Init();
	}

	return pNavi;
}
