#include "TitleMap.h"
#include "road.h"
#include "map_obstacle.h"
#include "manager.h"
#include "road_manager.h"
#include "gimmick.h"

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

// �ÓI�����o�ϐ��錾
CTitleMap* CTitleMap::m_pInstance = nullptr;

//<==========================================================
//�R���X�g���N�^
//<==========================================================
CTitleMap::CTitleMap()
{
	//���̃t�@�C���l�[�����N���A����
	m_LoadFileName.clear();

}
//<==========================================================
//�f�X�g���N�^
//<==========================================================
CTitleMap::~CTitleMap()
{

}
//==========================================================
// ����������
//==========================================================
HRESULT CTitleMap::Init(void)
{
	return S_OK;
}

//==========================================================
// �I������
//==========================================================
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
//===============================================
// �t�@�C���ǂݍ���
//===============================================
void CTitleMap::Load(void)
{
	CScene::MODE mode = CManager::GetInstance()->GetMode();

	// ���f���t�@�C�����ǂݍ���
	LoadModelName("data\\TXT\\model_info.txt");

	// ���ǂݍ���
	LoadRoad("data\\FILE\\map\\road.bin");

	// ��Q���ǂݍ���
	LoadObstacle("data\\FILE\\map\\obstacle.bin");

	// ���A��
	CRoadManager::GetInstance()->AllConnect();
}

//===============================================
// �t�@�C���ǂݍ���
//===============================================
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

//===============================================
// �t�@�C���ǂݍ���
//===============================================
void CTitleMap::LoadObstacle(const std::string& filename)
{
	CMapObstacle* C = nullptr;

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
	std::vector<CMapObstacle::SInfo> roaddata(size);
	File.read(reinterpret_cast<char*>(roaddata.data()), size * sizeof(CMapObstacle::SInfo));

	for (const auto& it : roaddata)
	{
		CMapObstacle::Create(it);
	}

	// �t�@�C�������
	File.close();
}

//===============================================
// ���f�����t�@�C���ǂݍ���
//===============================================
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