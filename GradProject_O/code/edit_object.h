//==========================================================
//
// エディットオブジェクト [edit_object.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _EDIT_OBJ_H_		// このマクロが定義されていない場合
#define _EDIT_OBJ_H_		// 二重インクルード防止用マクロを定義

#include "edit.h"

class CMapObstacle;
class CEdit_Arrow;

//==========================================================
// サンプルのクラス定義
//==========================================================
class CEdit_Obj : public CEdit
{
public:	// 誰でもアクセス可能

	CEdit_Obj();	// コンストラクタ(オーバーロード)
	~CEdit_Obj();	// デストラクタ

	// メンバ関数
	virtual HRESULT Init(void) override;
	virtual void Uninit(void);
	virtual void Update(void) override;

	// メンバ関数(取得)

	// メンバ関数(設定)

private:	// 自分だけがアクセス可能

	// メンバ関数
	void ClickCheck();	// 選択確認
	void Delete();		// 削除
	void Move();		// 移動
	void Save();		// 保存
	void Create();		// 生成
	bool CursorCollision(CMapObstacle* pObj);
	void ModelChange();	// モデル変更

	// メンバ変数
	CEdit_Arrow* m_pArrow;	// 矢印
	float m_fMouseWheel;	// マウスホイールの移動量
	CMapObstacle* m_pSelect;	// 選択中オブジェクト
};

#endif

