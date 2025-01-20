//==========================================================
//
// ポーズ [pause.h]
// Author : Riku Nakamura
//
//==========================================================
#ifndef _SAMPLETASK_H_		// このマクロが定義されていない場合
#define _SAMPLETASK_H_		// 二重インクルード防止用マクロを定義

#include "task.h"
#include "object2D.h"

//==========================================================
// サンプルのクラス定義
//==========================================================
class CPause : public CTask
{

public:	// 誰でもアクセス可能

	// AIタイプ列挙型
	enum TYPE
	{
		TYPE_QUIT = 0,	// 終了
		TYPE_RETRY,			// やり直し
		TYPE_CANCEL,			// 再開
		TYPE_MAX
	};

	CPause();	// コンストラクタ(オーバーロード)
	~CPause();	// デストラクタ

	// メンバ関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	static CPause* Create(void);

	bool GetPause(void) { return m_bPause; }

	void ChangePause(void);

	// メンバ関数(取得)

	// メンバ関数(設定)

private:	// 自分だけがアクセス可能

	// メンバ関数
	
	// メンバ変数
	CObject2D* m_pPauseFrame;
	CObject2D* m_pPauseButton[TYPE_MAX];

	int m_nNumSelect;
	float m_fTimerColor;
	bool m_bPause;
};

#endif
