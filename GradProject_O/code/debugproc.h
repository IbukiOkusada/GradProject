//==========================================================
//
// デバッグ表示処理 [debugproc.h]
// Author Ibuki Okusada
//
//==========================================================
#ifndef _DEBUGPROC_H_		//このマクロが定義されていない場合
#define _DEBUGPROC_H_

#include "main.h"
#include "manager.h"

//**********************************************************
//クラスの定義
//**********************************************************
class CDebugProc
{
private:

	CDebugProc();	// コンストラクタ
	~CDebugProc();	// デストラクタ

public:	// 誰でもアクセス可能

	void Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void Print(const char *fmt, ...);
	static CDebugProc* Create();
	static CDebugProc* GetInstance() { return m_pInstance; }

private:	// 自分だけがアクセス可能
	static LPD3DXFONT m_pFont;	//フォントへのポインタ
	char* m_pStr;					// デバッグ表示用文字列の格納用
	bool m_bDisp;		//デバッグ表示のON/OFF
	static const char *m_apMode[CScene::MODE_MAX];
	static CDebugProc* m_pInstance;
};

#endif