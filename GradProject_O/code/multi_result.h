//===============================================
//
// マルチリザルト画面の管理処理 [multi_result.h]
// Author : Ibuki Okusada
//
//===============================================
#ifndef _MULTI_RESULT_H_		// このマクロが定義されていない場合
#define _MULTI_RESULT_H_		// 二重インクルード防止用マクロを定義

#include "manager.h"

// 前方宣言
class CMultiResultManager;
class CMultiResultState;
class CScrollText2D;
class CPlayer;

//===============================================
// リザルトクラスの定義(派生クラス)
//===============================================
class CMultiResult : public CScene
{
public:

	// プレイヤー情報
	struct SPlayerInfo
	{
		bool bActive;			// 使用されているか否か
		int nScore;				// スコア
		int nId;				// ID
		CScrollText2D* pString;	// 文字
		CPlayer* pPlayer;		// プレイヤー

		// コンストラクタ
		SPlayerInfo(bool _active = false,int _score = 0, int _id = 0, CScrollText2D* _string = nullptr, CPlayer* _player = nullptr) :
			bActive(_active), nScore(_score), nId(_id), pString(_string), pPlayer(_player) {}
	};

public:

	// メンバ関数
	CMultiResult();	// コンストラクタ
	~CMultiResult();	// デストラクタ

	// メンバ関数
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;

	// 設定系メンバ関数
	void ChangeState(CMultiResultState* pNext);
	void SetNowScr(int nNum) { m_nNowScrPlayer = nNum; }

	// 取得系メンバ関数
	SPlayerInfo* GetInfo() { return m_pInfo; }
	CScrollText2D* GetEndStr() { return m_pEndStr; }
	CScrollText2D* GetTitleStr() { return m_pTitleStr; }
	CMultiResultManager* GetMgr() { return m_pMgr; }
	int GetNowScr() { return m_nNowScrPlayer; }

private:

	// メンバ関数
	void Sort();
	void StrCheck();
	void EndStr();
	void InitCameraSet();

	// メンバ変数
	CMultiResultManager* m_pMgr;	// マネージャー
	CMultiResultState* m_pState;	// 状態
	CScrollText2D* m_pEndStr;		// 最終文字
	CScrollText2D* m_pTitleStr;		// 最終文字
	SPlayerInfo* m_pInfo;			// 情報
	int m_nNowScrPlayer;

};

#endif
