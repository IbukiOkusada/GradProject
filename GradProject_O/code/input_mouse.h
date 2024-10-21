//==========================================================
//
// 入力デバイス処理(マウス) [input_mouse.h]
// Author Ibuki Okusada
//
//==========================================================
#ifndef _INPUT_MOUSE_H_		// このマクロが定義されていない場合
#define _INPUT_MOUSE_H_		// 二重インクルード防止用マクロを定義

#include "input.h"

//==========================================================
//派生クラスの定義(マウス)
//==========================================================
class CInputMouse : public CInput
{
public:	// 誰でもアクセス可能な定義

	//マウス
	enum BUTTON
	{
		BUTTON_LBUTTON = 0,	// 左クリック
		BUTTON_RBUTTON,		// 右クリック
		BUTTON_WHEEL,			// ホイール
		BUTTON_MAX
	};

	// レイ情報構造体
	struct SRayInfo
	{
		D3DXVECTOR3 origin;	// 開始点
		D3DXVECTOR3 end;	// 終点
		D3DXVECTOR3 vec;	// 方向
		D3DXVECTOR3 vecold;	// 前回の方向

		SRayInfo() : origin(D3DXVECTOR3(0.0f, 0.0f, 0.0f)), end(D3DXVECTOR3(0.0f, 0.0f, 0.0f)), 
			vec(D3DXVECTOR3(0.0f, 0.0f, 0.0f)), vecold(D3DXVECTOR3(0.0f, 0.0f, 0.0f)) {}
	};

public:	// 誰でもアクセス可能
	CInputMouse();	// コンストラクタ
	~CInputMouse();	// デストラクタ

	// メンバ関数
	HRESULT Init(HINSTANCE hInstance, HWND hWnd);
	void Uninit(void);
	void Update(void);
	bool GetPress(int nKey);
	bool GetTrigger(int nKey);
	bool GetRelease(int nKey);
	bool GetRepeat(int nKey);
	D3DXVECTOR3 GetCousorMove();
	SRayInfo GetRayInfo() { return m_RayInfo; }
	static CInputMouse* Create(HINSTANCE hInstance, HWND hWnd);
	static CInputMouse* GetInstance() { return m_pInstance; }

private:	// 自分だけアクセス可能

	// メンバ変数
	DIMOUSESTATE2 m_State;	//プレス情報
	DIMOUSESTATE2 m_Trigger;	//トリガー情報
	DIMOUSESTATE2 m_Release;	//リリース情報
	DIMOUSESTATE2 m_Repeat;	//リピート情報
	POINT m_Point;			//カーソルポインター
	SRayInfo m_RayInfo;
	static CInputMouse* m_pInstance;
};

#endif
