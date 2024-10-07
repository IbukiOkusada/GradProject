//==========================================================
//
// プレイヤーマネージャー [player_manager.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _PLAYERMANAGER_H_		// このマクロが定義されていない場合
#define _PLAYERMANAGER_H_		// 二重インクルード防止用マクロを定義

// 前方宣言
class CPlayer;

//==========================================================
// サンプルのクラス定義
//==========================================================
class CPlayerManager
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
	CPlayer* GetTop(void) { return m_pTop; }
	CPlayer* GetCur(void) { return m_pCur; }
	void ListIn(CPlayer* pPlayer);
	void ListOut(CPlayer* pPlayer);
	int GetNum(void) { return m_nNum; }
	bool Hit(D3DXVECTOR3& pos, const float fRange, const float fHeight, const int nDamage);

private:	// 自分だけがアクセス可能

	// メンバ関数
	// 
	// メンバ変数
	CPlayer* m_pTop;	// 先頭
	CPlayer* m_pCur;	// 最後尾
	int m_nNum;
	static CPlayerManager* m_pInstance;	// インスタンス
};

#endif
