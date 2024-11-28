//==========================================================
//
// マップ管理の処理 [map_manager.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _MAP_MANAGER_H_	// このマクロが定義されていない場合
#define _MAP_MANAGER_H_	// 二重インクルード防止用マクロを定義

#include "list_manager.h"

// 前方宣言
class CMap;

//==========================================================
// タスクマネージャーのクラス定義
//==========================================================
class CMapManager : public CListManager
{
private:
	CMapManager();	// コンストラクタ
	~CMapManager();	// デストラクタ

public:
	// リスト管理メンバ関数
	HRESULT Init(void);
	void Uninit(void);
	void Load(void);

	// シングルトン
	static CMapManager* GetInstance(void);
	static void Release(void);
	std::vector<std::string>& GetFileNameList() { return m_LoadFileName; }

private:	// 自分だけがアクセス可能

	// メンバ関数
	void LoadRoad(const std::string& filename);
	void LoadObstacle(const std::string& filename);
	void LoadModelName(const std::string& filename);
	void LoadGimmick(const std::string& filename);
	void LoadGoal(const std::string& filename);

	// メンバ変数
	std::vector<std::string> m_LoadFileName;
	static CMapManager* m_pInstance;
};

#endif
