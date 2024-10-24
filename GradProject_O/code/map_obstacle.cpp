//==========================================================
//
// �}�b�v����Q���̏��� [map_obstacle.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "map_obstacle.h"
#include "objectX.h"

// �ÓI�����o�ϐ��錾
std::vector<std::string> CMapObstacle::m_LoadFileName = {};
Clist<CMapObstacle*>* CMapObstacle::m_pList = nullptr;

//==========================================================
// �R���X�g���N�^
//==========================================================
CMapObstacle::CMapObstacle(const SInfo& info)
{
	//���g�����X�g�ɓo�^
	GetInstance()->Regist(this);
	m_Info = info;
}

//==========================================================
// �f�X�g���N�^
//==========================================================
CMapObstacle::~CMapObstacle()
{
	//���g�����X�g����폜
	GetInstance()->Delete(this);
	if (GetInstance()->Empty())
	{//���X�g�̒��g����Ȃ烊�X�g��j��
		ListRelease();
	}
}

//==========================================================
// ����������
//==========================================================
HRESULT CMapObstacle::Init(void)
{
	m_pObj = CObjectX::Create(m_Info.pos, m_Info.rot, m_LoadFileName[m_Info.fileidx].c_str());
	
	return S_OK;
}

//==========================================================
// �I������
//==========================================================
void CMapObstacle::Uninit(void)
{
	if (m_pObj != nullptr)
	{
		m_pObj->Uninit();
		m_pObj = nullptr;
	}

	Release();
}

//==========================================================
// �X�V����
//==========================================================
void CMapObstacle::Update(void)
{

	// �I�u�W�F�N�g�̍X�V
	if (m_pObj != nullptr)
	{
		m_pObj->SetPosition(m_Info.pos);
		m_pObj->SetRotation(m_Info.rot);
	}
}

//==========================================================
// ����
//==========================================================
CMapObstacle* CMapObstacle::Create(const SInfo& info)
{
	CMapObstacle* pGoal = nullptr;

	pGoal = DEBUG_NEW CMapObstacle(info);

	if (pGoal != nullptr)
	{
		pGoal->m_Info = info;
		// ����������
		pGoal->Init();
	}

	return pGoal;
}

//==========================================================
// ���W�ݒ�
//==========================================================
void CMapObstacle::SetPosition(const D3DXVECTOR3& pos)
{
	m_Info.pos = pos;

	// �I�u�W�F�N�g�̍X�V
	if (m_pObj != nullptr)
	{
		m_pObj->SetPosition(m_Info.pos);
	}
}

//==========================================================
// �����ݒ�
//==========================================================
void CMapObstacle::SetRotation(const D3DXVECTOR3& rot)
{
	m_Info.rot = rot;

	// �I�u�W�F�N�g�̍X�V
	if (m_pObj != nullptr)
	{
		m_pObj->SetRotation(m_Info.rot);
	}
}

//==========================================================
// ���f���ݒ�
//==========================================================
void CMapObstacle::BindModel(const int& idx)
{
	if (idx < 0 || idx >= m_LoadFileName.size()) { m_Info.fileidx = 0;}

	m_Info.fileidx = idx;

	if (m_pObj == nullptr) { return; }

	m_pObj->BindFile(m_LoadFileName[idx].c_str());
}

//==========================================================
// ���f���t�@�C���ǂݍ���
//==========================================================
void CMapObstacle::FileLoad(const std::string& filename)
{
	// �t�@�C�����J��
	std::ifstream File(filename);
	if (!File.is_open()) {
		return;
	}

	// �R�����g�p
	std::string hoge;

	// ���f����
	std::string modelname;

	// �f�[�^�ǂݍ���
	std::string line;
	while (std::getline(File, line))
	{
		// �R�����g�̓X�L�b�v
		if (line.empty() ||
			line[0] == '#')
		{
			continue;
		}

		// �X�g���[���쐬
		std::istringstream lineStream(line);

		if (line.find("MODEL_FILENAME") != std::string::npos)
		{// TEXT_FILENAME�Ń��f�����ǂݍ���

			// �X�g���[���쐬
			std::istringstream lineStream(line);

			// ���n��
			lineStream >>
				hoge >>
				hoge >>			// ��
				modelname;// ���f���t�@�C����

			// ���ǂݍ���
			m_LoadFileName.push_back(modelname);
			continue;
		}

		if (line.find("END_SCRIPT") != std::string::npos)
		{
			break;
		}
	}

	// �t�@�C�������
	File.close();
}