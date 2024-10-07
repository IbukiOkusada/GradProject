//===============================================
//
// フェードの処理全般 [fade.h]
// Author : Ibuki Okusada
//
//===============================================
#ifndef _FADE_H_
#define _FADE_H_

#include "main.h"	//main.hで定義しているものが必要なためinclude
#include "manager.h"
#include "object2D.h"
#include "task.h"

//===============================================
// フェードクラスの定義(派生クラス)
//===============================================
class CFade
{
public:	// 誰でもアクセス可能な定義

	// 状態列挙型
	enum STATE
	{
		STATE_NONE = 0,	//何もしていない状態
		STATE_IN,		//フェードイン状態
		STATE_OUT,		//フェードアウト状態
		STATE_MAX
	};

public:	// 誰でもアクセス可能

	CFade();	// コンストラクタ
	~CFade();	// デストラクタ

	// メンバ関数
	HRESULT Init(void);
	HRESULT Init(CScene::MODE modeNext);
	void Uninit(void);
	void Update(void);
	void Set(CScene::MODE modeNext);
	void SetState(STATE state);
	STATE GetState(void) { return m_state; }
	static CFade *Create(CScene::MODE modeNext);

private:	// 自分だけがアクセス可能な定義

	// メンバ変数
	STATE m_state;				// 状態
	CScene::MODE m_modeNext;	// 次の画面(モード)
	D3DXCOLOR m_Col;			// フェードカラー
	CObject2D *m_pObject;		// 描画オブジェクト
};

#endif
