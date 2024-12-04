//<========================================================
//�^�C�g���ł̃A�C�X���� [TitleBaggage.h]
//
//Author : Kazuki Watanabe
//<========================================================
#include "TitleBaggage.h"
#include "model.h"
#include "character.h"
#include "road_manager.h"
#include "road.h"
#include "goal_manager.h"

//<=========================================================
//������
//<=========================================================
HRESULT CTitleBaggage::Init(void)
{
	CBaggage::Init();
	return S_OK;
}
//<=========================================================
//��������
//<=========================================================
CTitleBaggage* CTitleBaggage::Create(const D3DXVECTOR3& pos)
{
	//�������m�ۂ��邽�߂̕ϐ�
	CTitleBaggage* pTitleBaggage = DEBUG_NEW CTitleBaggage;

	//�������m�ۂ���Ă�����
	if (pTitleBaggage)
	{
		//�ݒ�
		pTitleBaggage->Init();
	}
	else { return nullptr; }

	return pTitleBaggage;
}
//<=========================================================
//�S�[���̏�����
//<=========================================================
HRESULT CTitleGoal::Init(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot)
{
	constexpr char* MOTION_HUMAN = "data\\TXT\\motion_kidsboy.txt";	//�S�[���ƂȂ�l�Ԃ̃��[�V����
	const D3DXVECTOR3 SCALE = D3DXVECTOR3(2.0f, 2.0f, 2.0f);		//�X�P�[��

	//������
	m_pPeople = nullptr;
	m_rPos = VECTOR3_ZERO;

	//�l�Ԃ̐ݒ�
	m_pPeople = CCharacter::Create(pos, rot,MOTION_HUMAN);
	m_pPeople->SetScale(SCALE);

	//�ʒu�ݒ�
	m_rPos = pos;

	return S_OK;
}
//<=========================================================
//�S�[���̏I������
//<=========================================================
void CTitleGoal::Uninit(void)
{
	//�I������
	SAFE_UNINIT(m_pPeople);
	Release();
}
//<=========================================================
//�X�V����
//<=========================================================
void CTitleGoal::Update(void)
{
	//�l�Ԃ̍X�V
	if (m_pPeople){m_pPeople->Update();}
}
//<=========================================================
//�S�[���̐�������
//<=========================================================
CTitleGoal* CTitleGoal::Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot)
{
	CTitleGoal* pTitleGoal = DEBUG_NEW CTitleGoal;

	if (pTitleGoal) { pTitleGoal->Init(pos, rot); }
	else { return nullptr; }

	return pTitleGoal;
}