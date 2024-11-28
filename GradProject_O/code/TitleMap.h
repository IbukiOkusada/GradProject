//<=================================================
//タイトルでのマップ処理
//
//Author : Kazuki Watanabe
//<=================================================
#ifndef _TITLEMAP_H_
#define _TITLEMAP_H_

#include "list_manager.h"
#include "task.h"

//<=================================================
//タイトルマネージャー継承
//<=================================================
class CTitleMap : public CListManager
{
public:

	CTitleMap();
	~CTitleMap();

	// リスト管理メンバ関数
	HRESULT Init(void);
	void Uninit(void);
	void Load(void);

	// シングルトン
	static CTitleMap* GetInstance(void) 
	{
		if (m_pInstance == nullptr)
		{
			m_pInstance = DEBUG_NEW CTitleMap;
		}
		return m_pInstance;
	}
	static void Release(void) 
	{
		if (m_pInstance != nullptr)
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
#endif
