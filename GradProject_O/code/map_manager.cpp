//==========================================================
//
// �^�X�N�Ǘ��̏��� [task_manager.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "map_manager.h"
#include "road.h"
#include "map_obstacle.h"
#include "manager.h"
#include "road_manager.h"
#include "gimmick.h"
#include "goal_manager.h"
#include "goal.h"
#include "robot.h"
#include "effect_effekseer.h"

namespace
{
	const std::string ROADFILENAME[CScene::MODE_MAX] = {	// ���t�@�C����
		"data\\FILE\\map\\road.bin",
		"data\\FILE\\map\\tutorial_road.bin",
		"data\\FILE\\map\\road.bin",
		"data\\FILE\\map\\road.bin",
		"data\\FILE\\map\\road.bin",
	};

	const std::string OBSTACLEFILENAME[CScene::MODE_MAX] = {	// ��Q���t�@�C����
		"data\\FILE\\map\\obstacle.bin",
		"data\\FILE\\map\\tutorial_obstacle.bin",
		"data\\FILE\\map\\obstacle.bin",
		"data\\FILE\\map\\obstacle.bin",
		"data\\FILE\\map\\obstacle.bin",
	};

	const std::string GIMMICKFILENAME[CScene::MODE_MAX] = {	// �M�~�b�N�t�@�C����
		"",
		"data\\FILE\\map\\tutorial_gimmick.bin",
		"data\\FILE\\map\\gimmick.bin",
		"data\\FILE\\map\\gimmick.bin",
		"data\\FILE\\map\\gimmick.bin",
	};

	const std::string GOALFILENAME[CScene::MODE_MAX] = {	// �S�[���t�@�C����
		"",
		"data\\FILE\\map\\tutorial_goal.bin",
		"data\\FILE\\map\\goal.bin",
		"data\\FILE\\map\\goal.bin",
		"data\\FILE\\map\\goal.bin",
	};

	const std::string MODELNAMEFILE[CScene::MODE_MAX] = {	// ���f�����t�@�C��
		"data\\TXT\\model_info.txt",
		"data\\TXT\\model_info.txt",
		"data\\TXT\\model_info.txt",
		"data\\TXT\\model_info.txt",
		"data\\TXT\\model_info.txt",
	};

	const std::string ROBOTFILENAME[CScene::MODE_MAX] = {  // ���{�b�g�t�@�C��
		"",
		"",
		"data\\FILE\\map\\robot.bin",
		"",
	};

	const std::string EFFECTNAMEFILE[CScene::MODE_MAX] = {	// ���f�����t�@�C��
		"data\\FILE\\map\\effect.bin",
		"data\\FILE\\map\\effect.bin",
		"data\\FILE\\map\\effect.bin",
		"data\\FILE\\map\\effect.bin",
		"data\\FILE\\map\\effect.bin",
	};
}

// �ÓI�����o�ϐ��錾
CMapManager* CMapManager::m_pInstance = nullptr;

//==========================================================
// �R���X�g���N�^
//==========================================================
CMapManager::CMapManager()
{
	m_LoadFileName.clear();
}

//==========================================================
// �f�X�g���N�^
//==========================================================
CMapManager::~CMapManager()
{

}

//==========================================================
// ����������
//==========================================================
HRESULT CMapManager::Init(void)
{
	return S_OK;
}

//==========================================================
// �I������
//==========================================================
void CMapManager::Uninit(void)
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
// �C���X�^���X�̎擾
//===============================================
CMapManager* CMapManager::GetInstance()
{
	if (m_pInstance == nullptr)
	{
		m_pInstance = DEBUG_NEW CMapManager;
	}

	return m_pInstance;
}

//===============================================
// �C���X�^���X�̔p��
//===============================================
void CMapManager::Release(void)
{
	if (m_pInstance != nullptr)
	{
		m_pInstance->Uninit();
	}
}

//===============================================
// �t�@�C���ǂݍ���
//===============================================
void CMapManager::Load(void)
{
	CScene::MODE mode = CManager::GetInstance()->GetMode();

	// ���f���t�@�C�����ǂݍ���
	LoadModelName(MODELNAMEFILE[mode]);

	// ���ǂݍ���
	LoadRoad(ROADFILENAME[mode]);

	// �M�~�b�N�ǂݍ���
	LoadGimmick(GIMMICKFILENAME[mode]);

	// ��Q���ǂݍ���
	LoadObstacle(OBSTACLEFILENAME[mode]);

	// �S�[���ǂݍ���
	LoadGoal(GOALFILENAME[mode]);

	// ���{�b�g�ǂݍ���
	LoadRobot(ROBOTFILENAME[mode]);

	// �S�[���ǂݍ���
	LoadEffect(EFFECTNAMEFILE[mode]);

	// ���A��
	CRoadManager::GetInstance()->AllConnect();
}

//===============================================
// �t�@�C���ǂݍ���
//===============================================
void CMapManager::LoadRoad(const std::string& filename)
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
void CMapManager::LoadObstacle(const std::string& filename)
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
	std::vector<CMapObstacle::SInfo> roaddata(size);
	File.read(reinterpret_cast<char*>(roaddata.data()), size * sizeof(CMapObstacle::SInfo));

	
	for (const auto& it : roaddata)
	{
		if (it.fileidx >= m_LoadFileName.size()) { return; }
		CMapObstacle::Create(it);
	}

	// �t�@�C�������
	File.close();
}

//===============================================
// �t�@�C���ǂݍ���
//===============================================
void CMapManager::LoadGoal(const std::string& filename)
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
	std::vector<CGoal::SInfo> roaddata(size);
	File.read(reinterpret_cast<char*>(roaddata.data()), size * sizeof(CGoal::SInfo));

	// �t�@�C�������
	File.close();

	// �S�[���}�l�[�W���[�ŊǗ�
	CGoalManager::Create();
	CGoalManager::GetInstance()->SetInfoList(roaddata);
}

//===============================================
// ���f�����t�@�C���ǂݍ���
//===============================================
void CMapManager::LoadRobot(const std::string& filename)
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
	std::vector<CRobot::SInfo> roaddata(size);
	File.read(reinterpret_cast<char*>(roaddata.data()), size * sizeof(CRobot::SInfo));

	for (const auto& it : roaddata)
	{
		CRobot::Create(it.pos, it.rot, it.fDistance);
	}

	// �t�@�C�������
	File.close();
}

//===============================================
// ���f�����t�@�C���ǂݍ���
//===============================================
void CMapManager::LoadModelName(const std::string& filename)
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

//===============================================
// �M�~�b�N�ǂݍ���
//===============================================
void CMapManager::LoadGimmick(const std::string& filename)
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
	std::vector<CGimmick::SInfo> roaddata(size);
	File.read(reinterpret_cast<char*>(roaddata.data()), size * sizeof(CGimmick::SInfo));

	for (const auto& it : roaddata)
	{
		CGimmick::Create(it.pos, it.rot, it.scale, it.type);
	}

	// �t�@�C�������
	File.close();
}

//===============================================
// �M�~�b�N�ǂݍ���
//===============================================
void CMapManager::LoadEffect(const std::string& filename)
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
	std::vector<CEffectEffekseer::SInfo> roaddata(size);
	File.read(reinterpret_cast<char*>(roaddata.data()), size * sizeof(CEffectEffekseer::SInfo));

	for (const auto& it : roaddata)
	{
		CEffectEffekseer::Create(it.pos, it.rot, it.move, it.fScale, it.Type);
	}

	// �t�@�C�������
	File.close();
}