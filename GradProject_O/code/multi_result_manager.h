//==========================================================
//
// マルチリザルトマネージャー [multi_result_manager.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _MULTI_RESULT_MANAGER_H_		// このマクロが定義されていない場合
#define _MULTI_RESULT_MANAGER_H_		// 二重インクルード防止用マクロを定義

#include "list_manager.h"
#include "network.h"

//==========================================================
// マネージャーの派生クラス定義
//==========================================================
class CMultiResultManager : public CListManager
{
public:

	// 受け取り情報
	struct SInfo
	{
		bool bActive;	// 使用されているか
		int nNumDelv;	// 配達数
		int nId;		// ID

		// コンストラクタ
		SInfo() : bActive(false), nNumDelv(0), nId(-1) {}
	};

private:

	CMultiResultManager();	// コンストラクタ
	~CMultiResultManager();	// デストラクタ

public:	// 誰でもアクセス可能

	// メンバ関数
	HRESULT Init(void);
	void Uninit(void) override;
	void Update(void);

	// 取得系
	SInfo* GetInfo() { return &m_aInfo[0]; }
	int GetNumPlayer() { return m_nNumPlayer; }
	int GetMyId() { return m_nMyId; }

	// 設定系
	void DataSet(void);

	// インスタンス関連
	static CMultiResultManager* GetInstance(void);
	static void Release(void);

private:	// 自分だけがアクセス可能

	// メンバ関数

	// メンバ変数
	int m_nMyId;
	int m_nNumPlayer;
	SInfo m_aInfo[NetWork::MAX_CONNECT];

	// 静的メンバ変数
	static CMultiResultManager* m_pInstance;	// インスタンス
};

#endif