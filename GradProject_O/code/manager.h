//===============================================
//
// マネージャーの処理 [manager.h]
// Author : Ibuki Okusada
//
//===============================================
#ifndef _MANAGER_H_		// このマクロが定義されていない場合
#define _MANAGER_H_		// 二重インクルード防止用マクロを定義

#include "main.h"
#include "fxmanager.h"
#include "objectsound.h"

// 前方宣言
class CRenderer;
class CInput;
class CDebugProc;
class CSound;
class CCamera;
class CLight;
class CTexture;
class CXFile;
class CFileLoad;
class CSlow;
class CFade;
class CMultiCamera;
class CMapCamera;
class CTime;
class CPlayer;
class CFileLoad;
class CEnemyRoute;
class CCarManager;
class CEnemyManager;
class CMiniMap;
class CDeltaTime;
class CFont;
class CNetWork;

//===============================================
// シーンクラスの定義
//===============================================
class CScene
{
public:

	// モード列挙型
	enum MODE
	{
		MODE_TITLE = 0,	// タイトル画面
		MODE_ENTRY,     // エントリー画面
		MODE_GAME,		// ゲーム画面
		MODE_RESULT,	// リザルト画面
		MODE_MULTI_RESULT,	// マルチリザルト画面
		MODE_MAX
	};

	CScene();
	~CScene();

	// メンバ関数
	static CScene *Create(MODE mode);
	virtual HRESULT Init(void);
	virtual void Uninit(void);
	virtual void Update(void);
	virtual void Draw(void);
	virtual CMapCamera *GetMapCamera(void) { return NULL; }
	virtual CPlayer *GetPlayer(void) { return NULL; }
	virtual CFileLoad *GetFileLoad(void) { return NULL; }
	virtual CTime* GetTime(void) { return nullptr; }
	void SetMode(MODE mode) { m_mode = mode; }
	MODE GetMode(void) { return m_mode; }
	virtual void SetID(const int id) {};
	virtual void ChangeFlag(bool value) {};
	virtual void EndTutorial(void) {};
	virtual void ReadyUp(const int id) {};

private:
	MODE m_mode;	// モード
};

//===============================================
// マネージャークラスの定義
//===============================================
class CManager
{
public:	// 誰でもアクセス可能

	CManager();	// コンストラクタ
	~CManager();	// デストラクタ

	// メンバ関数
	HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	// 静的メンバ関数
	CRenderer *GetRenderer(void);
	CDebugProc *GetDebugProc(void);
	CSound *GetSound(void);
	CMultiCamera *GetCamera(void);
	CLight *GetLight(void);
	CTexture *GetTexture(void);
	CXFile *GetModelFile(void);
	CSlow *GetSlow(void);
	CFade *GetFade(void);
	CFont *GetFont(void);
	CDeltaTime *GetDeltaTime(void);
	CMasterSound::CObjectSound* GetScrTxt2DSe() { return m_pScrTxt2DSe; }
	CMasterSound::CObjectSound* GetScrStr2DSe() { return m_pScrStr2DSe; }
	
	void SetMode(CScene::MODE mode);
	CScene::MODE GetMode(void) { return m_pScene->GetMode(); }
	CScene *GetScene(void) { return m_pScene; }
	static CManager *GetInstance(void);
	static void Release(void);

	void SetDeliveryStatus(int nScore) { m_nDeliveryStatus = nScore; }
	int GetDeliveryStatus() { return m_nDeliveryStatus; }
	void SetLife(float nLife) { m_fLife = nLife; }
	float GetLife() { return m_fLife; }
	void SetSuccess(bool bSuccess) { m_bSuccess = bSuccess; }
	bool GetSuccess() { return m_bSuccess; }

private:	// 自分だけがアクセス可能

	// メンバ関数
	void DataReset(void);

	// 静的メンバ変数
	CRenderer* m_pRenderer;	// レンダラーのポインタ
	CInput* m_pInput;		// 入力のポイント
	CDebugProc* m_pDebugProc;	// デバッグ表示のポインタ
	CMultiCamera* m_pCamera;		// カメラのポインタ
	CLight* m_pLight;		// ライトのポインタ
	CTexture* m_pTexture;	// テクスチャのポインタ
	CXFile* m_pModelFile;	// Xファイルのポインタ
	CSound* m_pSound;		// サウンドのポインタ
	CSlow* m_pSlow;			// スローのポインタ
	CScene* m_pScene;		// シーンのポインタ
	CFade* m_pFade;			// フェードのポインタ
	CDeltaTime* m_pDeltaTime;  // タイマーのポインタ
	CFont* m_pFont;
	CNetWork* m_pNetWork;
	CMasterSound::CObjectSound* m_pScrTxt2DSe;
	CMasterSound::CObjectSound* m_pScrStr2DSe;

	static CManager *m_pManager;	// マネージャーのポインタ

	int m_nDeliveryStatus;
	float m_fLife;
	bool m_bSuccess;
};


#endif

