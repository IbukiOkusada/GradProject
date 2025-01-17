//==========================================================
//
// エディットゴール [edit_robot.h]
// Author : Ryosuke Oohara
//
//==========================================================
#ifndef _EDIT_ROBOT_H_		// このマクロが定義されていない場合
#define _EDIT_ROBOT_H_		// 二重インクルード防止用マクロを定義

#include "edit.h"

// 前方宣言
class CGoal;
class CEdit_Handle;
class CRobot;
class CEffect3D;

//==========================================================
// ロボット配置エディタのクラス定義
//==========================================================
class CEdit_Robot : public CEdit
{
public:	// 誰でもアクセス可能

	CEdit_Robot();	// コンストラクタ(オーバーロード)
	~CEdit_Robot();	// デストラクタ

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
	void Rotate();		// 回転
	void RotateReset();	// 回転リセット
	void ChangeDistance();  // 距離
	void Save();		// 保存
	void Create();		// 生成
	void ModeChange();  // ハンドルの状態変化
	bool CursorCollision(CRobot* pRobot);
	bool TriangleCollision(const D3DXVECTOR3& rayOrigin, const D3DXVECTOR3& rayDir,
		const D3DXVECTOR3& pos0, const D3DXVECTOR3& pos1, const D3DXVECTOR3& pos2);

	// メンバ変数
	CRobot* m_pSelect;  // 選択したロボット
	CEdit_Handle* m_pHandle;	// 矢印
	D3DXVECTOR3 m_startRotate;
	CEffect3D* m_pStartPosEff;
	CEffect3D* m_pEndPosEff;
	float m_fStartDistance;  // 基準の距離
	float m_fMouseWheel;	// マウスホイールの移動量
	float m_fMoveLength;	// 移動距離
};

#endif