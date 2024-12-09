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
class CEdit_Handle;

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
	void Scale();		// スケール
	void Rotate();		// 回転
	void RotateReset();	// 回転リセット
	void Save();		// 保存
	void Create();		// 生成
	void ModeChange();	// 変更
	bool CursorCollision(CMapObstacle* pObj);
	void ModelChange();	// モデル変更

	// メンバ変数
	D3DXVECTOR3 m_startScale;
	D3DXVECTOR3 m_startRotate;
	CEdit_Handle* m_pHandle;	// 矢印
	float m_fMouseWheel;	// マウスホイールの移動量
	CMapObstacle* m_pSelect;	// 選択中オブジェクト
};

#endif

