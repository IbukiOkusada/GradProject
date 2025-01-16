//===============================================
//
// �G�t�F�N�g�̏��� [effect.cpp]
// Author : Riku Nakamura
//
//===============================================
#include "effect_effekseer.h"
#include "debugproc.h"

//===============================================
// �������O���
//===============================================
namespace {
	const std::string EFFECTNAMEPATH[CEffectEffekseer::TYPE_MAX] = {	// �G�t�F�N�g�̃p�X
		"data\\EFFEKSEER\\patrollamp.efkefc",
		"data\\EFFEKSEER\\explosion.efkefc",
		"data\\EFFEKSEER\\splash.efkefc",
		"data\\EFFEKSEER\\trail.efkefc",
	};
}

//===============================================
// �R���X�g���N�^
//===============================================
CEffectEffekseer::CEffectEffekseer()
{
	m_Info = SInfo();
}

//===============================================
// �f�X�g���N�^
//===============================================
CEffectEffekseer::~CEffectEffekseer()
{

}

//===============================================
// ����������
//===============================================
HRESULT CEffectEffekseer::Init(void)
{
	m_pEffekseer = CEffekseer::GetInstance()->Create(EFFECTNAMEPATH[m_Info.Type],
		m_Info.pos, m_Info.rot, m_Info.move, m_Info.fScale, false, false);

	return S_OK;
}

//===============================================
// �I������
//===============================================
void CEffectEffekseer::Uninit(void)
{
	SAFE_DELETE(m_pEffekseer)

	Release();
}

//===============================================
// �X�V����
//===============================================
void CEffectEffekseer::Update(void)
{
	m_pEffekseer->m_pos = m_Info.pos;
	m_pEffekseer->m_rot = m_Info.rot;
	m_pEffekseer->m_move = m_Info.move;
	m_pEffekseer->m_Scale.X = m_Info.fScale;
	m_pEffekseer->m_Scale.Y = m_Info.fScale;
	m_pEffekseer->m_Scale.Z = m_Info.fScale;
}

//===============================================
// ����
//===============================================
CEffectEffekseer *CEffectEffekseer::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move, float fScale, TYPE type)
{
	CEffectEffekseer *pEffectEffekseer = nullptr;
	CTexture *pTexture = CManager::GetInstance()->GetTexture();	// �e�N�X�`���ւ̃|�C���^

	// �G�t�F�N�g�̐���
	pEffectEffekseer = DEBUG_NEW CEffectEffekseer;

	if (pEffectEffekseer != NULL)
	{// �����ł����ꍇ

		// ���W�ݒ�
		pEffectEffekseer->SetPosition(pos);

		// �p�x�ݒ�
		pEffectEffekseer->SetRotation(rot);

		// �p�x�ݒ�
		pEffectEffekseer->SetMove(move);

		// �p�x�ݒ�
		pEffectEffekseer->SetScale(fScale);

		// ��ނ̐ݒ�
		pEffectEffekseer->SetType(type);

		// ����������
		pEffectEffekseer->Init();
	}
	else
	{// �����Ɏ��s�����ꍇ
		return NULL;
	}

	return pEffectEffekseer;
}