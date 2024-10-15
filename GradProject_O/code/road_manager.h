//==========================================================
//
// 道マネージャー [road_manager.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _ROADMANAGER_H_		// このマクロが定義されていない場合
#define _ROADMANAGER_H_		// 二重インクルード防止用マクロを定義

// 前方宣言
class CRoad;

//==========================================================
// サンプルのクラス定義
//==========================================================
class CRoadManager
{
private:

	CRoadManager();		// コンストラクタ
	~CRoadManager();	// デストラクタ

public:	// 誰でもアクセス可能

	// メンバ関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	static CRoadManager* GetInstance(void);
	static void Release(void);
	CRoad* GetTop(void) { return m_pTop; }
	CRoad* GetCur(void) { return m_pCur; }
	void ListIn(CRoad* pRoad);
	void ListOut(CRoad* pRoad);
	int GetNum(void) { return m_nNum; }
	bool Hit(D3DXVECTOR3& pos, const float fRange, const float fHeight, const int nDamage);

	// 道連結関数
	void AllConnect(void);

private:	// 自分だけがアクセス可能

	// メンバ関数
	void SideConnectCheck(CRoad* pRoad, CRoad* pCheckRoad);		// 横連結判定
	void VerticalConnectCheck(CRoad* pRoad, CRoad* pCheckRoad);	// 縦連結判定

	// メンバ変数
	CRoad* m_pTop;	// 先頭
	CRoad* m_pCur;	// 最後尾
	int m_nNum;
	static CRoadManager* m_pInstance;	// インスタンス
};

#endif

