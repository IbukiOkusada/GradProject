//<=================================================
//�^�C�g���ł̃}�b�v����(TitleMap.cpp)
//
//Author : Kazuki Watanabe
//<=================================================
#include "TitleMap.h"
#include "road.h"
#include "map_obstacle.h"
#include "manager.h"
#include "road_manager.h"
#include "gimmick.h"
#include "objectX.h"

//<*********************************************************
//���O�錾
//<*********************************************************
namespace
{
	const std::string ROADFILENAME[CScene::MODE_MAX] = {	// ���t�@�C����
		"data\\FILE\\map\\road.bin",
		"data\\FILE\\map\\road.bin",
		"data\\FILE\\map\\road.bin",
		"data\\FILE\\map\\road.bin",
		"data\\FILE\\map\\road.bin",
	};

	const std::string OBSTACLEFILENAME[CScene::MODE_MAX] = {	// ��Q���t�@�C����
		"data\\FILE\\map\\obstacle.bin",
		"data\\FILE\\map\\obstacle.bin",
		"data\\FILE\\map\\obstacle.bin",
		"data\\FILE\\map\\obstacle.bin",
		"data\\FILE\\map\\obstacle.bin",
	};

	const std::string MODELNAMEFILE[CScene::MODE_MAX] = {	// ���f�����t�@�C��
		"data\\TXT\\model_info.txt",
		"data\\TXT\\model_info.txt",
		"data\\TXT\\model_info.txt",
		"data\\TXT\\model_info.txt",
		"data\\TXT\\model_info.txt",
	};
}

//<*****************************************
//�ÓI�����o�ϐ��錾
//<*****************************************
CTitleMap* CTitleMap::m_pInstance = nullptr;				//�C���X�^���X
Clist<CTitleObstacle*>* CTitleObstacle::m_pList = nullptr;	//���X�g(�^�C�g���p��Q��)
//<==========================================================
//�^�C�g���}�b�v�̏I������
//<==========================================================
void CTitleMap::Uninit(void)
{
	// �e�N���X�̏I������
	CListManager::Uninit();

	// �C���X�^���X�̔p��
	if (m_pInstance != nullptr)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}
//<==========================================================
//�^�C�g���}�b�v�̃t�@�C���ǂݍ���
//<==========================================================
void CTitleMap::Load(void)
{
	//���[�h
	CScene::MODE mode = CManager::GetInstance()->GetMode();

	//���������Ă���
	m_LoadFileName.clear();

	//���f���t�@�C�����ǂݍ���
	LoadModelName(MODELNAMEFILE[mode]);

	//���ǂݍ���
	LoadRoad(ROADFILENAME[mode]);

	//��Q���ǂݍ���
	LoadObstacle(OBSTACLEFILENAME[mode]);

	//���A��
	CRoadManager::GetInstance()->AllConnect();
}

//<==========================================================
//�^�C�g���}�b�v�̓��t�@�C���ǂݍ���
//<==========================================================
void CTitleMap::LoadRoad(const std::string& filename)
{
	// �t�@�C�����J��
	std::ifstream File(filename, std::ios::binary);
	if (!File.is_open()) {
		// ��O����
		return;
	}

	// �T�C�Y�ǂݍ���
	int size = 0;
	File.read(reinterpret_cast<char*>(&size), sizeof(size));

	// �f�[�^�ǂݍ���
	std::vector<CRoad::SInfo> roaddata(size);
	File.read(reinterpret_cast<char*>(roaddata.data()), size * sizeof(CRoad::SInfo));

	for (const auto& it : roaddata)
	{
		CRoad::Create(it.pos, it.rot, it.sizeOrigin);
	}

	// �t�@�C�������
	File.close();
}

//<==========================================================
//�^�C�g���}�b�v�̏�Q���t�@�C���ǂݍ���
//<==========================================================
void CTitleMap::LoadObstacle(const std::string& filename)
{
	// �t�@�C�����J��
	std::ifstream File(filename, std::ios::binary);
	if (!File.is_open()) {
		// ��O����
		return;
	}

	// �T�C�Y�ǂݍ���
	int size = 0;
	File.read(reinterpret_cast<char*>(&size), sizeof(size));

	// �f�[�^�ǂݍ���
	std::vector<CTitleObstacle::SInfo> roaddata(size);
	File.read(reinterpret_cast<char*>(roaddata.data()), size * sizeof(CTitleObstacle::SInfo));

	for (const auto& it : roaddata)
	{
		CTitleObstacle::Create(it);
	}

	// �t�@�C�������
	File.close();
}

//<==========================================================
//�^�C�g���}�b�v�̃��f�����ǂݍ���
//<==========================================================
void CTitleMap::LoadModelName(const std::string& filename)
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
			std::istringstream lineStr(line);

			// ���n��
			lineStr >>
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
//<==========================================================
//�^�C�g���p��Q���̃R���X�g���N�^
//<==========================================================
CTitleObstacle::CTitleObstacle(const SInfo& info)
{
	//���g�����X�g�ɓo�^
	GetInstance()->Regist(this);
	m_Info = info;
	m_pObj = nullptr;
}

//<==========================================================
//�^�C�g���p��Q���̃f�X�g���N�^
//<==========================================================
CTitleObstacle::~CTitleObstacle()
{
	//���g�����X�g����폜
	GetInstance()->Delete(this);
	if (GetInstance()->Empty())
	{//���X�g�̒��g����Ȃ烊�X�g��j��
		ListRelease();
	}
}

//<==========================================================
//�^�C�g���p��Q���̏���������(�����Ȃ�)
//<==========================================================
HRESULT CTitleObstacle::Init(void)
{
	std::vector<std::string> str = CTitleMap::GetInstance()->GetFileNameList();
	m_pObj = CObjectX::Create(m_Info.pos, m_Info.rot, str[m_Info.fileidx].c_str());

	return S_OK;
}
//<==========================================================
//�^�C�g���p��Q���̏I������
//<==========================================================
void CTitleObstacle::Uninit(void)
{
	if (m_pObj != nullptr)
	{
		m_pObj->Uninit();
		m_pObj = nullptr;
	}

	Release();
}

//<==========================================================
//�^�C�g���p��Q���̍X�V����(�w��Ȃ�)
//<==========================================================
void CTitleObstacle::Update(void)
{

	//�I�u�W�F�N�g�̍X�V
	if (m_pObj)
	{
		m_pObj->SetPosition(m_Info.pos);
		m_pObj->SetRotation(m_Info.rot);
		m_pObj->SetScale(m_Info.scale);
	}
}

//<==========================================================
//�^�C�g���p��Q���̐�������(�w��Ȃ�)
//<==========================================================
CTitleObstacle* CTitleObstacle::Create(const SInfo& info)
{
	//�������m�ۗp�̃I�u�W�F�N�g
	CTitleObstacle* pGoal = DEBUG_NEW CTitleObstacle(info);

	//���g�����
	if (pGoal)
	{
		//����`�Ə���������
		pGoal->m_Info = info;
		pGoal->Init();
	}
	//�Ȃ�������
	else
	{
		return nullptr;
	}

	return pGoal;
}
