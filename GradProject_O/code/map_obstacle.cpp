//==========================================================
//
// マップ内障害物の処理 [map_obstacle.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "map_obstacle.h"
#include "objectX.h"

// 静的メンバ変数宣言
std::vector<std::string> CMapObstacle::m_LoadFileName = {};
Clist<CMapObstacle*>* CMapObstacle::m_pList = nullptr;

//==========================================================
// コンストラクタ
//==========================================================
CMapObstacle::CMapObstacle(const SInfo& info)
{
	//自身をリストに登録
	GetInstance()->Regist(this);
	m_Info = info;
}

//==========================================================
// デストラクタ
//==========================================================
CMapObstacle::~CMapObstacle()
{
	//自身をリストから削除
	GetInstance()->Delete(this);
	if (GetInstance()->Empty())
	{//リストの中身が空ならリストを破棄
		ListRelease();
	}
}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CMapObstacle::Init(void)
{
	m_pObj = CObjectX::Create(m_Info.pos, m_Info.rot, m_LoadFileName[m_Info.fileidx].c_str());
	
	return S_OK;
}

//==========================================================
// 終了処理
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
// 更新処理
//==========================================================
void CMapObstacle::Update(void)
{

	// オブジェクトの更新
	if (m_pObj != nullptr)
	{
		m_pObj->SetPosition(m_Info.pos);
		m_pObj->SetRotation(m_Info.rot);
	}
}

//==========================================================
// 生成
//==========================================================
CMapObstacle* CMapObstacle::Create(const SInfo& info)
{
	CMapObstacle* pGoal = nullptr;

	pGoal = DEBUG_NEW CMapObstacle(info);

	if (pGoal != nullptr)
	{
		pGoal->m_Info = info;
		// 初期化処理
		pGoal->Init();
	}

	return pGoal;
}

//==========================================================
// 座標設定
//==========================================================
void CMapObstacle::SetPosition(const D3DXVECTOR3& pos)
{
	m_Info.pos = pos;

	// オブジェクトの更新
	if (m_pObj != nullptr)
	{
		m_pObj->SetPosition(m_Info.pos);
	}
}

//==========================================================
// 向き設定
//==========================================================
void CMapObstacle::SetRotation(const D3DXVECTOR3& rot)
{
	m_Info.rot = rot;

	// オブジェクトの更新
	if (m_pObj != nullptr)
	{
		m_pObj->SetRotation(m_Info.rot);
	}
}

//==========================================================
// モデル設定
//==========================================================
void CMapObstacle::BindModel(const int& idx)
{
	if (idx < 0 || idx >= m_LoadFileName.size()) { m_Info.fileidx = 0;}

	m_Info.fileidx = idx;

	if (m_pObj == nullptr) { return; }

	m_pObj->BindFile(m_LoadFileName[idx].c_str());
}

//==========================================================
// モデルファイル読み込み
//==========================================================
void CMapObstacle::FileLoad(const std::string& filename)
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
			std::istringstream lineStream(line);

			// 情報渡す
			lineStream >>
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