//==========================================================
//
// 車マネージャー [car_manager.h]
// Author : Riku Nakamura
//
//==========================================================
#ifndef _CARMANAGER_H_		// このマクロが定義されていない場合
#define _CARMANAGER_H_		// 二重インクルード防止用マクロを定義

// 前方宣言
class CCar;

//==========================================================
// サンプルのクラス定義
//==========================================================
class CCarManager
{
private:

	CCarManager();		// コンストラクタ
	~CCarManager();	// デストラクタ

public:	// 誰でもアクセス可能

	// メンバ関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	static CCarManager* GetInstance(void);
	static void Release(void);
	CCar* GetTop(void) { return m_pTop; }
	CCar* GetCur(void) { return m_pCur; }
	void ListIn(CCar* pCar);
	void ListOut(CCar* pCar);
	int GetNum(void) { return m_nNum; }
	bool Hit(D3DXVECTOR3& pos, const float fRange, const float fHeight, const int nDamage);

private:	// 自分だけがアクセス可能

	// メンバ関数
	// 
	// メンバ変数
	CCar* m_pTop;	// 先頭
	CCar* m_pCur;	// 最後尾
	int m_nNum;
	static CCarManager* m_pInstance;	// インスタンス
};

#endif
