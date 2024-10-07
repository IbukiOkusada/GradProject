//==========================================================
//
// 入力デバイス処理 [input.h]
// Author Ibuki Okusada
//
//==========================================================
#ifndef _INPUT_H_		// このマクロが定義されていない場合
#define _INPUT_H_		// 二重インクルード防止用マクロを定義

#include "main.h"

// 前方宣言
class CInputKeyboard;
class CInputMouse;
class CInputPad;

//==========================================================
//基底クラスの定義(入力デバイス)
//==========================================================
class CInput
{
public:	// 誰でもアクセス可能
	
	CInput();	// コンストラクタ
	virtual ~CInput();	// デストラクタ

	// メンバ関数
	virtual HRESULT Init(HINSTANCE hInstance, HWND hWnd);
	virtual void Uninit(void);
	virtual void Update(void);
	void Release();
	static CInput* Create(HINSTANCE hInstance, HWND hWnd);

protected:	// 派生クラスからもアクセス可能
	static LPDIRECTINPUT8 m_pInput;	//DirectInputオブジェクトへのポインタ
	LPDIRECTINPUTDEVICE8 m_pDevice;	//入力デバイスへのポインタ

private:	// 自分だけアクセス可能

	// メンバ変数
	CInputKeyboard *m_pKeyboard;	// キーボード
	CInputMouse* m_pMouse;			// マウス
	CInputPad* m_pPad;				// パッド
	static CInput* m_pInstance;		// 自身のポインタ
	
};

#endif
