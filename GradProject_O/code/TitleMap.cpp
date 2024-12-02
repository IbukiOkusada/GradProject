//<=================================================
//タイトルでのマップ処理(TitleMap.cpp)
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
//名前宣言
//<*********************************************************
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

//<*****************************************
//静的メンバ変数宣言
//<*****************************************
CTitleMap* CTitleMap::m_pInstance = nullptr;				//インスタンス
Clist<CTitleObstacle*>* CTitleObstacle::m_pList = nullptr;	//リスト(タイトル用障害物)
//<==========================================================
//タイトルマップの終了処理
//<==========================================================
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
//<==========================================================
//タイトルマップのファイル読み込み
//<==========================================================
void CTitleMap::Load(void)
{
	//モード
	CScene::MODE mode = CManager::GetInstance()->GetMode();

	//初期化しておく
	m_LoadFileName.clear();

	//モデルファイル名読み込み
	LoadModelName(MODELNAMEFILE[mode]);

	//道読み込み
	LoadRoad(ROADFILENAME[mode]);

	//障害物読み込み
	LoadObstacle(OBSTACLEFILENAME[mode]);

	//道連結
	CRoadManager::GetInstance()->AllConnect();
}

//<==========================================================
//タイトルマップの道ファイル読み込み
//<==========================================================
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

//<==========================================================
//タイトルマップの障害物ファイル読み込み
//<==========================================================
void CTitleMap::LoadObstacle(const std::string& filename)
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
	std::vector<CTitleObstacle::SInfo> roaddata(size);
	File.read(reinterpret_cast<char*>(roaddata.data()), size * sizeof(CTitleObstacle::SInfo));

	for (const auto& it : roaddata)
	{
		CTitleObstacle::Create(it);
	}

	// ファイルを閉じる
	File.close();
}

//<==========================================================
//タイトルマップのモデル名読み込み
//<==========================================================
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
//<==========================================================
//タイトル用障害物のコンストラクタ
//<==========================================================
CTitleObstacle::CTitleObstacle(const SInfo& info)
{
	//自身をリストに登録
	GetInstance()->Regist(this);
	m_Info = info;
	m_pObj = nullptr;
}

//<==========================================================
//タイトル用障害物のデストラクタ
//<==========================================================
CTitleObstacle::~CTitleObstacle()
{
	//自身をリストから削除
	GetInstance()->Delete(this);
	if (GetInstance()->Empty())
	{//リストの中身が空ならリストを破棄
		ListRelease();
	}
}

//<==========================================================
//タイトル用障害物の初期化処理(引数なし)
//<==========================================================
HRESULT CTitleObstacle::Init(void)
{
	std::vector<std::string> str = CTitleMap::GetInstance()->GetFileNameList();
	m_pObj = CObjectX::Create(m_Info.pos, m_Info.rot, str[m_Info.fileidx].c_str());

	return S_OK;
}
//<==========================================================
//タイトル用障害物の終了処理
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
//タイトル用障害物の更新処理(指定なし)
//<==========================================================
void CTitleObstacle::Update(void)
{

	//オブジェクトの更新
	if (m_pObj)
	{
		m_pObj->SetPosition(m_Info.pos);
		m_pObj->SetRotation(m_Info.rot);
		m_pObj->SetScale(m_Info.scale);
	}
}

//<==========================================================
//タイトル用障害物の生成処理(指定なし)
//<==========================================================
CTitleObstacle* CTitleObstacle::Create(const SInfo& info)
{
	//メモリ確保用のオブジェクト
	CTitleObstacle* pGoal = DEBUG_NEW CTitleObstacle(info);

	//中身あれば
	if (pGoal)
	{
		//情報定義と初期化処理
		pGoal->m_Info = info;
		pGoal->Init();
	}
	//なかったら
	else
	{
		return nullptr;
	}

	return pGoal;
}
