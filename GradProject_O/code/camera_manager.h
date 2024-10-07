//==========================================================
//
// カメラマネージャー [camera_manager.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _CAMERAMANAGER_H_		// このマクロが定義されていない場合
#define _CAMERAMANAGER_H_		// 二重インクルード防止用マクロを定義

// 前方宣言
class CCamera;

//==========================================================
// サンプルのクラス定義
//==========================================================
class CCameraManager
{
private:

	CCameraManager();		// コンストラクタ
	~CCameraManager();	// デストラクタ

public:	// 誰でもアクセス可能

	// メンバ関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	static CCameraManager *GetInstance(void);
	static void Release(void);
	CCamera *GetTop(void) { return m_pTop; }
	CCamera *GetCur(void) { return m_pCur; }
	void ListIn(CCamera *pCamera);
	void ListOut(CCamera *pCamera);
	int GetNum(void) { return m_nNum; }

private:	// 自分だけがアクセス可能

	// メンバ関数

	// メンバ変数
	CCamera *m_pTop;	// 先頭
	CCamera *m_pCur;	// 最後尾
	int m_nNum;
	static CCameraManager *m_pInstance;	// インスタンス
};

#endif
