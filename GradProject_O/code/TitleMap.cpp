#include "TitleMap.h"
#include "road.h"
#include "map_obstacle.h"
#include "manager.h"
#include "road_manager.h"
#include "gimmick.h"

namespace
{
	const std::string ROADFILENAME[CScene::MODE_MAX] = {	// 道ファイル名
		"data\\FILE\\map\\road.bin",
		"data\\FILE\\map\\road.bin",
		"data\\FILE\\map\\road.bin",
		"data\\FILE\\map\\road.bin",
		"data\\FILE\\map\\road.bin",
	};

	const std::string OBSTACLEFILENAME[CScene::MODE_MAX] = {	// 障害物ファイル名
		"data\\FILE\\map\\obstacle.bin",
		"data\\FILE\\map\\obstacle.bin",
		"data\\FILE\\map\\obstacle.bin",
		"data\\FILE\\map\\obstacle.bin",
		"data\\FILE\\map\\obstacle.bin",
	};

	const std::string MODELNAMEFILE[CScene::MODE_MAX] = {	// モデル名ファイル
		"data\\TXT\\model_info.txt",
		"data\\TXT\\model_info.txt",
		"data\\TXT\\model_info.txt",
		"data\\TXT\\model_info.txt",
		"data\\TXT\\model_info.txt",
	};
}

// 静的メンバ変数宣言
CTitleMap* CTitleMap::m_pInstance = nullptr;

//<==========================================================
//コンストラクタ
//<==========================================================
CTitleMap::CTitleMap()
{
	//道のファイルネームをクリアする
	m_LoadFileName.clear();

}
//<==========================================================
//デストラクタ
//<==========================================================
CTitleMap::~CTitleMap()
{

}
//==========================================================
// 初期化処理
//==========================================================
HRESULT CTitleMap::Init(void)
{
	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CTitleMap::Uninit(void)
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
// ファイル読み込み
//===============================================
void CTitleMap::Load(void)
{
	CScene::MODE mode = CManager::GetInstance()->GetMode();

	// モデルファイル名読み込み
	LoadModelName("data\\TXT\\model_info.txt");

	// 道読み込み
	LoadRoad("data\\FILE\\map\\road.bin");

	// 障害物読み込み
	LoadObstacle("data\\FILE\\map\\obstacle.bin");

	// 道連結
	CRoadManager::GetInstance()->AllConnect();
}

//===============================================
// ファイル読み込み
//===============================================
void CTitleMap::LoadRoad(const std::string& filename)
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
void CTitleMap::LoadObstacle(const std::string& filename)
{
	CMapObstacle* C = nullptr;

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
		CMapObstacle::Create(it);
	}

	// ファイルを閉じる
	File.close();
}

//===============================================
// モデル名ファイル読み込み
//===============================================
void CTitleMap::LoadModelName(const std::string& filename)
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