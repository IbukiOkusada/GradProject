//===============================================
//
// �G�t�F�N�g�̏��� [effect.cpp]
// Author : Ibuki Okusada
//
//===============================================
#include "effect3D.h"
#include "manager.h"
#include "renderer.h"
#include "input.h"
#include "debugproc.h"
#include "texture.h"
#include "deltatime.h"
#include "game.h"
#include "meshfield.h"
#include "billboard.h"
#include "deltatime.h"

//===============================================
// �������O���
//===============================================
namespace {
	const float DELTATIME_MULTI = (100.0f);
	const float LIFE = (30.0f); // ����
	const float GUNCHARGE = (0.1f);

	const D3DXCOLOR COLINFO[CEffect3D::TYPE_MAX] = {	// ��ޕʏ����F�̐ݒ�
		{1.0f, 1.0f, 1.0f, 1.0f},
		{ 1.0f, 1.0f, 0.0f, 1.0f },
	};

	const float RADIUSINFO[CEffect3D::TYPE_MAX] = {	// ��ޕʔ��a�̐ݒ�
		100.0f,
		100.0f,
	};

	const CTexture::TYPE TEXIDX[CEffect3D::TYPE::TYPE_MAX] = {
		CTexture::TYPE_EFFECT,
		CTexture::TYPE_EFFECT,
		CTexture::TYPE_WATER,
		CTexture::TYPE_EFFECT,
	};
}

//===============================================
// �R���X�g���N�^
//===============================================
CEffect3D::CEffect3D()
{
	// �l�̃N���A
	m_Info.col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	m_Info.fLife = 0;
	m_Info.move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_Info.fRadius = 0.0f;
	m_Info.Type = TYPE_NONE;
}

//===============================================
// �f�X�g���N�^
//===============================================
CEffect3D::~CEffect3D()
{

}

//===============================================
// ����������
//===============================================
HRESULT CEffect3D::Init(void)
{
	// �I�u�W�F�N�g�̏���������
	m_pObjectBilBoard = CObjectBillboard::Create(m_Info.pos, 6);
	m_pObjectBilBoard->BindTexture(CManager::GetInstance()->GetTexture()->Regist(CManager::GetInstance()->GetTexture()->GetFileName(SetTex(m_Info.Type))));
	m_pObjectBilBoard->SetAlphaText(true);
	m_pObjectBilBoard->SetZTest(true);
	m_pObjectBilBoard->SetLighting(true);
	m_pObjectBilBoard->SetFusion(CObjectBillboard::FUSION_ADD);

	m_Info.fLife = LIFE;	// �̗͂̏�����

	return S_OK;
}

//===============================================
// �I������
//===============================================
void CEffect3D::Uninit(void)
{
	// �I�u�W�F�N�g�̏I������
	if (m_pObjectBilBoard != nullptr) {
		m_pObjectBilBoard->Uninit();
		m_pObjectBilBoard = nullptr;
	}

	Release();
}

//===============================================
// �X�V����
//===============================================
void CEffect3D::Update(void)
{
	float slow = CDeltaTime::GetInstance()->GetSlow();
	m_Info.fLife -= slow;
	D3DXVECTOR3 nor = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 posOld = m_Info.pos;
	//float fHeight = CMeshField::GetHeight(m_Info.pos, posOld);

	if (m_Info.fLife < 0)
	{// �������Ȃ��Ȃ����ꍇ

		// �I������
		Uninit();
		return;
	}

	D3DXVECTOR3 pos = GetPosition();	// ���W

	pos += m_Info.move * slow;

	// ���W
	SetPosition(pos);

	switch (m_Info.Type)
	{
	case TYPE_NONE:
		m_Info.col.a -= 0.05f * slow;
		m_Info.fRadius += 0.1f * slow;
		break;

	case TYPE_SMAKE:	// ��

		m_Info.col.a -= 0.05f * slow;
		m_Info.fRadius += 0.1f * slow;

		break;

	case TYPE_SPLASH:	// ��

		m_Info.col.a -= 0.01f * slow;
		m_Info.move.y += -1.0f;

		break;

	case TYPE_LASER:	// ��

		m_Info.col.a -= 0.01f * slow;

		break;
	}

	if (m_Info.col.a < 0.0f || m_Info.fRadius < 0.0f)
	{// �l���Ђ�����Ԃ���

		
		Uninit();
		return;
	}

	InfoSet();
}

//===============================================
// ����
//===============================================
CEffect3D *CEffect3D::Create(D3DXVECTOR3 pos, TYPE type)
{
	CEffect3D *pEffect3D = NULL;

	// �G�t�F�N�g�̐���
	pEffect3D = DEBUG_NEW CEffect3D;

	if (pEffect3D != NULL)
	{// �����ł����ꍇ

		// ���W�ݒ�
		pEffect3D->SetPosition(pos);

		// ��ނ̐ݒ�
		pEffect3D->SetType(type);

		// ���a�ݒ�
		pEffect3D->RadiusSet();

		// �F�̐ݒ�
		pEffect3D->ColorSet();

		pEffect3D->InfoSet();

		// ����������
		pEffect3D->Init();

		// �e�N�X�`���̊��蓖��
	}
	else
	{// �����Ɏ��s�����ꍇ
		return NULL;
	}

	return pEffect3D;
}

//===============================================
// ����
//===============================================
CEffect3D *CEffect3D::Create(D3DXVECTOR3 pos, D3DXVECTOR3 move, D3DXCOLOR col, float fRadius, float fLife, TYPE type)
{
	CEffect3D *pEffect3D = NULL;

	// �G�t�F�N�g�̐���
	pEffect3D = DEBUG_NEW CEffect3D();

	if (pEffect3D != NULL)
	{// �����ł����ꍇ

		// ���W�ݒ�
		pEffect3D->SetPosition(pos);

		// �G�t�F�N�g�̐ݒ�
		pEffect3D->SetType(type);

		// ���a�ݒ�
		pEffect3D->m_Info.fRadius = fRadius;

		// �ړ��ʐݒ�
		pEffect3D->SetMove(move);

		// �T�C�Y�̐ݒ�
		//pEffect3D->SetSize(pEffect3D->m_Info.fRadius, pEffect3D->m_Info.fRadius);

		// �F�̐ݒ�
		pEffect3D->m_Info.col = col;

		// ����������
		pEffect3D->Init();

		// �����ݒ�
		pEffect3D->m_Info.fLife = fLife;

		pEffect3D->InfoSet();

		// �`����@�ݒ�
		pEffect3D->DrawSet();
	}
	else
	{// �����Ɏ��s�����ꍇ
		return NULL;
	}

	return pEffect3D;
}

//===============================================
// �F�ݒ�
//===============================================
void CEffect3D::ColorSet(void)
{
	// ��ނ��ƂɐF�̐ݒ�
	m_Info.col = COLINFO[m_Info.Type];
	

	if (m_pObjectBilBoard == nullptr) {
		return;
	}

	m_pObjectBilBoard->SetCol(m_Info.col);
}

//===============================================
// �T�C�Y�̐ݒ�
//===============================================
void CEffect3D::RadiusSet(void)
{
	// ���a�̐ݒ�
	m_Info.fRadius = RADIUSINFO[m_Info.Type];

	if (m_pObjectBilBoard == nullptr) {
		return;
	}

	m_pObjectBilBoard->SetSize(m_Info.fRadius, m_Info.fRadius);
}

//===============================================
// �ړ��ʂ̐ݒ�
//===============================================
void CEffect3D::SetMove(D3DXVECTOR3 move)
{
	m_Info.move = move;
}

//===============================================
// ����{�ݒ�
//===============================================
void CEffect3D::InfoSet(void)
{
	if (m_pObjectBilBoard == nullptr) {
		return;
	}

	m_pObjectBilBoard->SetPosition(m_Info.pos);
	m_pObjectBilBoard->SetCol(m_Info.col);
	m_pObjectBilBoard->SetSize(m_Info.fRadius, m_Info.fRadius);
}

//===============================================
// �^�C�v�ʃe�N�X�`��
//===============================================
CTexture::TYPE CEffect3D::SetTex(TYPE type)
{
	return TEXIDX[type];
}

//===============================================
// �`��ݒ�
//===============================================
void CEffect3D::DrawSet(void)
{
	if (m_pObjectBilBoard == nullptr) {
		return;
	}

	switch (m_Info.Type)
	{
	case TYPE_NONE:
	{

	}
	break;

	case TYPE_SMAKE:
	{
		m_pObjectBilBoard->SetAlphaText(true);
		m_pObjectBilBoard->SetZTest(true);
		m_pObjectBilBoard->SetLighting(true);
		m_pObjectBilBoard->SetFusion(CObjectBillboard::FUSION_ADD);
	}
	break;

	case TYPE_SPLASH:
	{
		m_pObjectBilBoard->SetAlphaText(true);
		m_pObjectBilBoard->SetZTest(false);
		m_pObjectBilBoard->SetLighting(true);
		m_pObjectBilBoard->SetFusion(CObjectBillboard::FUSION_ADD);
	}
	break;

	case TYPE_LASER:
	{
		m_pObjectBilBoard->SetAlphaText(true);
		m_pObjectBilBoard->SetZTest(false);
		m_pObjectBilBoard->SetLighting(true);
		m_pObjectBilBoard->SetFusion(CObjectBillboard::FUSION_ADD);
	}
	break;

	}
}

//===============================================
// ���a�擾
//===============================================
float CEffect3D::GetRange(void) const
{
	if (m_pObjectBilBoard == nullptr) {
		return 0.0f;
	}

	return m_pObjectBilBoard->GetWidth();
}

//===============================================
// �F�擾
//===============================================
D3DXCOLOR CEffect3D::GetCol(void) const
{
	if (m_pObjectBilBoard == nullptr) {
		return D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);
	}

	return m_pObjectBilBoard->GetCol();
}