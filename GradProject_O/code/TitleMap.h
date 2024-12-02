//<=================================================
//タイトルでのマップ処理(TitleMap.h)
//
//Author : Kazuki Watanabe
//<=================================================
#ifndef _TITLEMAP_H_
#define _TITLEMAP_H_

#include "list_manager.h"
#include "task.h"
#include "list.h"
#include "map_obstacle.h"

//<=================================================
//タイトルマネージャー継承
//<=================================================
class CTitleMap : public CListManager
{
public:

	CTitleMap() {}
	~CTitleMap() {}

	// リスト管理メンバ関数
	HRESULT Init(void) { return S_OK; }
	void Uninit(void);
	void Load(void);

	//インスタンス取得
	static CTitleMap* GetInstance(void) 
	{
		if (!m_pInstance)
		{
			m_pInstance = DEBUG_NEW CTitleMap;
		}

		return m_pInstance;
	}
	//破棄
	static void Release(void) 
	{
		if (m_pInstance)
		{
			m_pInstance->Uninit();
		}
	}
	std::vector<std::string>& GetFileNameList() { return m_LoadFileName; }

private:	// 自分だけがアクセス可能

	// メンバ関数
	void LoadRoad(const std::string& filename);
	void LoadObstacle(const std::string& filename);
	void LoadModelName(const std::string& filename);

	// メンバ変数
	std::vector<std::string> m_LoadFileName;
	static CTitleMap* m_pInstance;
};
//<=================================================
//タイトル用の障害物(元の障害物クラスを継承)
//<=================================================
class CTitleObstacle : public CMapObstacle
{
public:

	CTitleObstacle(const SInfo& info = SInfo());	// コンストラクタ(オーバーロード)
	~CTitleObstacle();	// デストラクタ

	// メンバ関数
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;

	static CTitleObstacle* Create(const SInfo& info);

private:

	//メンバ変数
	CObjectX* m_pObj;	// 障害物
	SInfo m_Info;		// 基本情報
	static Clist<CTitleObstacle*>* m_pList;	// リスト

};
#endif
