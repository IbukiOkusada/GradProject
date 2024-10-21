//==========================================================
//
// エディター管理の処理 [edit_manager.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _EDIT_MANAGER_H_	// このマクロが定義されていない場合
#define _EDIT_MANAGER_H_	// 二重インクルード防止用マクロを定義

#include "edit.h"

//==========================================================
// タスクマネージャーのクラス定義
//==========================================================
class CEditManager
{
private:
	CEditManager();		// コンストラクタ
	~CEditManager();	// デストラクタ

public:
	// リスト管理メンバ関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);

	// シングルトン
	static CEditManager* Create(void);
	static CEditManager* GetInstance(void);
	static void Release(void);

private:	// 自分だけがアクセス可能

	// メンバ関数
	void ChangeEdit(CEdit* pEdit);

	// メンバ変数
	bool m_bFlag;	// 終了フラグ
	CEdit::TYPE m_SelectType;
	CEdit* m_pEdit;	// エディター用ポインタ
	static CEditManager* m_pInstance;
};

#endif
