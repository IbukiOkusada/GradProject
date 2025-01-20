//==========================================================
//
// タスク管理の処理 [task_manager.cpp]
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
	const std::string ROADFILENAME[CScene::MODE_MAX] = {	// 道ファイル名
		"data\\FILE\\map\\road.bin",
		"data\\FILE\\map\\tutorial_road.bin",
		"data\\FILE\\map\\road.bin",
		"data\\FILE\\map\\road.bin",
		"data\\FILE\\map\\road.bin",
	};

	const std::string OBSTACLEFILENAME[CScene::MODE_MAX] = {	// 障害物ファイル名
		"data\\FILE\\map\\obstacle.bin",
		"data\\FILE\\map\\tutorial_obstacle.bin",
		"data\\FILE\\map\\obstacle.bin",
		"data\\FILE\\map\\obstacle.bin",
		"data\\FILE\\map\\obstacle.bin",
	};

	const std::string GIMMICKFILENAME[CScene::MODE_MAX] = {	// ギミックファイル名
		"",
		"data\\FILE\\map\\tutorial_gimmick.bin",
		"data\\FILE\\map\\gimmick.bin",
		"data\\FILE\\map\\gimmick.bin",
		"data\\FILE\\map\\gimmick.bin",
	};

	const std::string GOALFILENAME[CScene::MODE_MAX] = {	// ゴールファイル名
		"",
		"data\\FILE\\map\\tutorial_goal.bin",
		"data\\FILE\\map\\goal.bin",
		"data\\FILE\\map\\goal.bin",
		"data\\FILE\\map\\goal.bin",
	};

	const std::string MODELNAMEFILE[CScene::MODE_MAX] = {	// モデル名ファイル
		"data\\TXT\\model_info.txt",
		"data\\TXT\\model_info.txt",
		"data\\TXT\\model_info.txt",
		"data\\TXT\\model_info.txt",
		"data\\TXT\\model_info.txt",
	};

	const std::string ROBOTFILENAME[CScene::MODE_MAX] = {  // ロボットファイル
		"",
		"",
		"data\\FILE\\map\\robot.bin",
		"",
	};

	const std::string EFFECTNAMEFILE[CScene::MODE_MAX] = {	// モデル名ファイル
		"data\\FILE\\map\\effect.bin",
		"data\\FILE\\map\\effect.bin",
		"data\\FILE\\map\\effect.bin",
		"data\\FILE\\map\\effect.bin",
		"data\\FILE\\map\\effect.bin",
	};
}

// 静的メンバ変数宣言
CMapManager* CMapManager::m_pInstance = nullptr;

//==========================================================
// コンストラクタ
//==========================================================
CMapManager::CMapManager()
{
	m_LoadFileName.clear();
}

//==========================================================
// デストラクタ
//==========================================================
CMapManager::~CMapManager()
{

}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CMapManager::Init(void)
{
	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CMapManager::Uninit(void)
{
	// 親クラスの終了処理
	CListManager::Uninit();

	// インスタンスの廃棄
	if (m_pInstance != nullptr)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

//===============================================
// インスタンスの取得
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
// インスタンスの廃棄
//===============================================
void CMapManager::Release(void)
{
	if (m_pInstance != nullptr)
	{
		m_pInstance->Uninit();
	}
}

//===============================================
// ファイル読み込み
//===============================================
void CMapManager::Load(void)
{
	CScene::MODE mode = CManager::GetInstance()->GetMode();

	// モデルファイル名読み込み
	LoadModelName(MODELNAMEFILE[mode]);

	// 道読み込み
	LoadRoad(ROADFILENAME[mode]);

	// ギミック読み込み
	LoadGimmick(GIMMICKFILENAME[mode]);

	// 障害物読み込み
	LoadObstacle(OBSTACLEFILENAME[mode]);

	// ゴール読み込み
	LoadGoal(GOALFILENAME[mode]);

	// ロボット読み込み
	LoadRobot(ROBOTFILENAME[mode]);

	// ゴール読み込み
	LoadEffect(EFFECTNAMEFILE[mode]);

	// 道連結
	CRoadManager::GetInstance()->AllConnect();
}

//===============================================
// ファイル読み込み
//===============================================
void CMapManager::LoadRoad(const std::string& filename)
{
	// ファイルを開く
	std::ifstream File(filename, std::ios::binary);
	if (!File.is_open()) {
		// 例外処理
		return;
	}

	// サイズ読み込み
	int size = 0;
	File.read(reinterpret_cast<char*>(&size), sizeof(size));

	// データ読み込み
	std::vector<CRoad::SInfo> roaddata(size);
	File.read(reinterpret_cast<char*>(roaddata.data()), size * sizeof(CRoad::SInfo));

	for (const auto& it : roaddata)
	{
		CRoad::Create(it.pos, it.rot, it.sizeOrigin);
	}

	// ファイルを閉じる
	File.close();
}

//===============================================
// ファイル読み込み
//===============================================
void CMapManager::LoadObstacle(const std::string& filename)
{
	// ファイルを開く
	std::ifstream File(filename, std::ios::binary);
	if (!File.is_open()) {
		// 例外処理
		return;
	}

	// サイズ読み込み
	int size = 0;
	File.read(reinterpret_cast<char*>(&size), sizeof(size));

	// データ読み込み
	std::vector<CMapObstacle::SInfo> roaddata(size);
	File.read(reinterpret_cast<char*>(roaddata.data()), size * sizeof(CMapObstacle::SInfo));

	
	for (const auto& it : roaddata)
	{
		if (it.fileidx >= m_LoadFileName.size()) { return; }
		CMapObstacle::Create(it);
	}

	// ファイルを閉じる
	File.close();
}

//===============================================
// ファイル読み込み
//===============================================
void CMapManager::LoadGoal(const std::string& filename)
{
	// ファイルを開く
	std::ifstream File(filename, std::ios::binary);
	if (!File.is_open()) {
		// 例外処理
		return;
	}

	// サイズ読み込み
	int size = 0;
	File.read(reinterpret_cast<char*>(&size), sizeof(size));

	// データ読み込み
	std::vector<CGoal::SInfo> roaddata(size);
	File.read(reinterpret_cast<char*>(roaddata.data()), size * sizeof(CGoal::SInfo));

	// ファイルを閉じる
	File.close();

	// ゴールマネージャーで管理
	CGoalManager::Create();
	CGoalManager::GetInstance()->SetInfoList(roaddata);
}

//===============================================
// モデル名ファイル読み込み
//===============================================
void CMapManager::LoadRobot(const std::string& filename)
{
	// ファイルを開く
	std::ifstream File(filename, std::ios::binary);
	if (!File.is_open()) {
		// 例外処理
		return;
	}

	// サイズ読み込み
	int size = 0;
	File.read(reinterpret_cast<char*>(&size), sizeof(size));

	// データ読み込み
	std::vector<CRobot::SInfo> roaddata(size);
	File.read(reinterpret_cast<char*>(roaddata.data()), size * sizeof(CRobot::SInfo));

	for (const auto& it : roaddata)
	{
		CRobot::Create(it.pos, it.rot, it.fDistance);
	}

	// ファイルを閉じる
	File.close();
}

//===============================================
// モデル名ファイル読み込み
//===============================================
void CMapManager::LoadModelName(const std::string& filename)
{
	// ファイルを開く
	std::ifstream File(filename);
	if (!File.is_open()) {
		return;
	}

	// コメント用
	std::string hoge;

	// モデル名
	std::string modelname;

	// データ読み込み
	std::string line;

	while (std::getline(File, line))
	{
		// コメントはスキップ
		if (line.empty() ||
			line[0] == '#')
		{
			continue;
		}

		// ストリーム作成
		std::istringstream lineStream(line);

		if (line.find("MODEL_FILENAME") != std::string::npos)
		{// TEXT_FILENAMEでモデル名読み込み

			// ストリーム作成
			std::istringstream lineStr(line);

			// 情報渡す
			lineStr >>
				hoge >>
				hoge >>			// ＝
				modelname;// モデルファイル名

			// 情報読み込み
			m_LoadFileName.push_back(modelname);
			continue;
		}

		if (line.find("END_SCRIPT") != std::string::npos)
		{
			break;
		}
	}

	// ファイルを閉じる
	File.close();
}

//===============================================
// ギミック読み込み
//===============================================
void CMapManager::LoadGimmick(const std::string& filename)
{
	// ファイルを開く
	std::ifstream File(filename, std::ios::binary);
	if (!File.is_open()) {
		// 例外処理
		return;
	}

	// サイズ読み込み
	int size = 0;
	File.read(reinterpret_cast<char*>(&size), sizeof(size));

	// データ読み込み
	std::vector<CGimmick::SInfo> roaddata(size);
	File.read(reinterpret_cast<char*>(roaddata.data()), size * sizeof(CGimmick::SInfo));

	for (const auto& it : roaddata)
	{
		CGimmick::Create(it.pos, it.rot, it.scale, it.type);
	}

	// ファイルを閉じる
	File.close();
}

//===============================================
// ギミック読み込み
//===============================================
void CMapManager::LoadEffect(const std::string& filename)
{
	// ファイルを開く
	std::ifstream File(filename, std::ios::binary);
	if (!File.is_open()) {
		// 例外処理
		return;
	}

	// サイズ読み込み
	int size = 0;
	File.read(reinterpret_cast<char*>(&size), sizeof(size));

	// データ読み込み
	std::vector<CEffectEffekseer::SInfo> roaddata(size);
	File.read(reinterpret_cast<char*>(roaddata.data()), size * sizeof(CEffectEffekseer::SInfo));

	for (const auto& it : roaddata)
	{
		CEffectEffekseer::Create(it.pos, it.rot, it.move, it.fScale, it.Type);
	}

	// ファイルを閉じる
	File.close();
}