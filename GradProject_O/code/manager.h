//===============================================
//
// マネージャーの処理 [manager.h]
// Author : Ibuki Okusada
//
//===============================================
#ifndef _MANAGER_H_		// このマクロが定義されていない場合
#define _MANAGER_H_		// 二重インクルード防止用マクロを定義

#include "main.h"

// 前方宣言
class CRenderer;
class CInputKeyboard;
class CInputMouse;
class CInputPad;
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
class CEditor;
class CEnemyRoute;
class CCarManager;
class CEnemyManager;
class CMiniMap;

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
		MODE_TUTORIAL,	// チュートリアル画面
		MODE_GAME,		// ゲーム画面
		MODE_RESULT,	// リザルト画面
		MODE_RANKING,	// ランキング画面
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
	virtual CEditor *GetEditor(void) { return NULL; }
	virtual CTime* GetTime(void) { return nullptr; }
	virtual void SendPosition(D3DXVECTOR3&) {  }
	virtual void SendRotation(D3DXVECTOR3&) {  }
	virtual void SendDamage(int) {  }
	virtual void SendLife(int) {  }
	void SetMode(MODE mode) { m_mode = mode; }
	MODE GetMode(void) { return m_mode; }

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
	CInputKeyboard *GetInputKeyboard(void);
	CInputMouse *GetInputMouse(void);
	CInputPad *GetInputPad(void);
	CDebugProc *GetDebugProc(void);
	CSound *GetSound(void);
	CMultiCamera *GetCamera(void);
	CLight *GetLight(void);
	CTexture *GetTexture(void);
	CXFile *GetModelFile(void);
	CSlow *GetSlow(void);
	CFade *GetFade(void);
	void SetMode(CScene::MODE mode);
	CScene::MODE GetMode(void) { return m_pScene->GetMode(); }
	CScene *GetScene(void) { return m_pScene; }
	static CManager *GetInstance(void);
	static void Release(void);
	void ManagerRelease(void);

private:	// 自分だけがアクセス可能

	// メンバ関数
	void DataReset(void);

	// 静的メンバ変数
	CRenderer *m_pRenderer;	// レンダラーのポインタ
	CInputKeyboard *m_pInputKeyboard;	// キーボードのポインタ
	CInputMouse *m_pInputMouse;	// マウスのポインタ
	CInputPad *m_pInputPad;	// パッドのポインタ
	CDebugProc *m_pDebugProc;	// デバッグ表示のポインタ
	CMultiCamera *m_pCamera;		// カメラのポインタ
	CLight *m_pLight;		// ライトのポインタ
	CTexture *m_pTexture;	// テクスチャのポインタ
	CXFile *m_pModelFile;	// Xファイルのポインタ
	CSound *m_pSound;		// サウンドのポインタ
	CSlow *m_pSlow;			// スローのポインタ
	CScene *m_pScene;		// シーンのポインタ
	CFade *m_pFade;			// フェードのポインタ
	static CManager *m_pManager;	// マネージャーのポインタ
};


#endif

