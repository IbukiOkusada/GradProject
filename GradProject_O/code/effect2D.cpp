//===============================================
//
// �G�t�F�N�g�̏��� [effect.cpp]
// Author : Ibuki Okusada
//
//===============================================
#include "effect2D.h"
#include "manager.h"
#include "renderer.h"
#include "input.h"
#include "debugproc.h"
#include "texture.h"
#include "slow.h"
#include "game.h"
#include "meshfield.h"
#include "billboard.h"

//===============================================
// �������O���
//===============================================
namespace {
	const float LIFE = (30.0f); // ����
	const float GUNCHARGE = (0.1f);

	const D3DXCOLOR COLINFO[CEffect2D::TYPE_MAX] = {	// ��ޕʏ����F�̐ݒ�
		{1.0f, 1.0f, 1.0f, 1.0f},
		{ 1.0f, 1.0f, 0.0f, 1.0f },
	};

	const float RADIUSINFO[CEffect2D::TYPE_MAX] = {	// ��ޕʔ��a�̐ݒ�
		100.0f,
		100.0f,
	};
}

//===============================================
// �R���X�g���N�^
//===============================================
CEffect2D::CEffect2D()
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
CEffect2D::~CEffect2D()
{

}

//===============================================
// ����������
//===============================================
HRESULT CEffect2D::Init(void)
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
void CEffect2D::Uninit(void)
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
void CEffect2D::Update(void)
{
	m_Info.fLife -= CManager::GetInstance()->GetSlow()->Get();
	D3DXVECTOR3 nor = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 posOld = m_Info.pos;
	float fHeight = CMeshField::GetHeight(m_Info.pos, posOld);

	if (m_Info.fLife < 0)
	{// �������Ȃ��Ȃ����ꍇ

		// �I������
		Uninit();
		return;
	}

	D3DXVECTOR3 pos = GetPosition();	// ���W

	pos += m_Info.move * CManager::GetInstance()->GetSlow()->Get();

	// ���W
	SetPosition(pos);

	switch (m_Info.Type)
	{
	case TYPE_NONE:
		m_Info.col.a -= 0.05f * CManager::GetInstance()->GetSlow()->Get();
		m_Info.fRadius += 0.1f * CManager::GetInstance()->GetSlow()->Get();
		break;

	case TYPE_SMAKE:	// ��

		m_Info.col.a -= 0.05f * CManager::GetInstance()->GetSlow()->Get();
		m_Info.fRadius += 0.1f * CManager::GetInstance()->GetSlow()->Get();

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
CEffect2D* CEffect2D::Create(D3DXVECTOR3 pos, TYPE type)
{
	CEffect2D* pEffect2D = NULL;
	CTexture* pTexture = CManager::GetInstance()->GetTexture();	// �e�N�X�`���ւ̃|�C���^

	// �G�t�F�N�g�̐���
	pEffect2D = DEBUG_NEW CEffect2D;

	if (pEffect2D != NULL)
	{// �����ł����ꍇ

		// ���W�ݒ�
		pEffect2D->SetPosition(pos);

		// ��ނ̐ݒ�
		pEffect2D->SetType(type);

		// ���a�ݒ�
		pEffect2D->RadiusSet();

		// �F�̐ݒ�
		pEffect2D->ColorSet();

		pEffect2D->InfoSet();

		// ����������
		pEffect2D->Init();

		// �e�N�X�`���̊��蓖��
	}
	else
	{// �����Ɏ��s�����ꍇ
		return NULL;
	}

	return pEffect2D;
}

//===============================================
// ����
//===============================================
CEffect2D* CEffect2D::Create(D3DXVECTOR3 pos, D3DXVECTOR3 move, D3DXCOLOR col, float fRadius, float fLife, TYPE type)
{
	CEffect2D* pEffect2D = NULL;
	CTexture* pTexture = CManager::GetInstance()->GetTexture();	// �e�N�X�`���ւ̃|�C���^

	// �G�t�F�N�g�̐���
	pEffect2D = DEBUG_NEW CEffect2D();

	if (pEffect2D != NULL)
	{// �����ł����ꍇ

		// ���W�ݒ�
		pEffect2D->SetPosition(pos);

		// �G�t�F�N�g�̐ݒ�
		pEffect2D->SetType(type);

		// ���a�ݒ�
		pEffect2D->m_Info.fRadius = fRadius;

		// �ړ��ʐݒ�
		pEffect2D->SetMove(move);

		// �T�C�Y�̐ݒ�
		//pEffect2D->SetSize(pEffect2D->m_Info.fRadius, pEffect2D->m_Info.fRadius);

		// �F�̐ݒ�
		pEffect2D->m_Info.col = col;

		// ����������
		pEffect2D->Init();

		// �����ݒ�
		pEffect2D->m_Info.fLife = fLife;

		pEffect2D->InfoSet();

		// �`����@�ݒ�
		pEffect2D->DrawSet();
	}
	else
	{// �����Ɏ��s�����ꍇ
		return NULL;
	}

	return pEffect2D;
}

//===============================================
// �F�ݒ�
//===============================================
void CEffect2D::ColorSet(void)
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
void CEffect2D::RadiusSet(void)
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
void CEffect2D::SetMove(D3DXVECTOR3 move)
{
	m_Info.move = move;
}

//===============================================
// ����{�ݒ�
//===============================================
void CEffect2D::InfoSet(void)
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
CTexture::TYPE CEffect2D::SetTex(TYPE type)
{
	switch (type)
	{
	case TYPE_NONE:
	{

	}
	break;

	case TYPE_SMAKE:
	{
		return CTexture::TYPE_EFFECT;

	}
	break;
	}

	return CTexture::TYPE();
}

//===============================================
// �`��ݒ�
//===============================================
void CEffect2D::DrawSet(void)
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

	}
}

//===============================================
// ���a�擾
//===============================================
float CEffect2D::GetRange(void) const
{
	if (m_pObjectBilBoard == nullptr) {
		return 0.0f;
	}

	return m_pObjectBilBoard->GetWidth();
}

//===============================================
// �F�擾
//===============================================
D3DXCOLOR CEffect2D::GetCol(void) const
{
	if (m_pObjectBilBoard == nullptr) {
		return D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);
	}

	return m_pObjectBilBoard->GetCol();
}