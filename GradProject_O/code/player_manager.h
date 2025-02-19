//==========================================================
//
// プレイヤーマネージャー [player_manager.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _PLAYERMANAGER_H_		// このマクロが定義されていない場合
#define _PLAYERMANAGER_H_		// 二重インクルード防止用マクロを定義

#include "list_manager.h"
#include "map_list.h"

// ネットワーク
#include "network.h"

// 前方宣言
class CPlayer;

//==========================================================
// サンプルのクラス定義
//==========================================================
class CPlayerManager : public CListManager
{
private:

	CPlayerManager();		// コンストラクタ
	~CPlayerManager();	// デストラクタ

public:	// 誰でもアクセス可能

	// メンバ関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	static CPlayerManager* GetInstance(void);
	static void Release(void);
	int GetNum() { return m_List.GetNum(); }
	CPlayer* GetPlayer(int nIdx = CNetWork::GetInstance()->GetIdx());
	Cmaplist<CPlayer*>* GetList() { return &m_List; }
	bool ListIn(CPlayer* pPlayer);
	bool ListOut(CPlayer* pPlayer);
	bool Hit(D3DXVECTOR3& pos, const float fRange, const float fHeight, const int nDamage);

private:	// 自分だけがアクセス可能

	// メンバ関数
	
	// メンバ変数
	Cmaplist<CPlayer*> m_List;
	static CPlayerManager* m_pInstance;	// インスタンス
};

#endif
