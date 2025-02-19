//==========================================================
//
// カメラの処理全般 [camera.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _CAMERA_H_		 // このマクロが定義されていない場合
#define _CAMERA_H_		 // 二重インクルード防止用マクロを定義

#include "main.h"		//main.hで定義しているものが必要なためinclude

// マクロ定義
#define MAX_CAMERA_ROTZ (D3DX_PI * 0.999f)	// カメラ最大角度
#define MIN_CAMERA_ROTZ (D3DX_PI * 0.01f)	// カメラ最大角度

class CCameraAction;

//**********************************************************
// カメラクラスの定義
//**********************************************************
class CCamera
{
public:	// 誰でもアクセス可能な定義

	enum MODE
	{
		MODE_NORMAL = 0,	// 通常
		MODE_FREE,			// 自由
		MODE_MAX
	};

	enum DRAWSTATE
	{
		NORMAL,
		PLAYER_ONLY,
		MAX
	};

public:	// 誰でもアクセス可能
	CCamera();	// コンストラクタ
	virtual ~CCamera();	// デストラクタ

	// メンバ関数
	virtual HRESULT Init(void);
	virtual void Uninit(void);
	virtual void Update(void);
	virtual void SetCamera(void);
	void Pursue(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, const float fLength);
	void Setting(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot);
	void SetRot(const D3DXVECTOR3 rot);
	void SetMode(MODE mode) { m_mode = mode; }
	MODE GetMode() { return m_mode; }
	void SetDrawState(DRAWSTATE state) { m_DrawState = state; }
	void SetOldRot(D3DXVECTOR3 rot) { m_SlowOldRot = rot; }
	void MoveV(void);
	void SetV(void);
	void SetR(void);
	void MouseCamera(void);
	void MoveVR(void);
	void SetPositionR(D3DXVECTOR3 pos);
	void SetPositionV(D3DXVECTOR3 pos);
	void Edit(void);

	//当たり判定
	void CollisionObj(void);

	// タイトル用
	void TitleRotateCamera(void);

	// スタートの扉全部開いたとき
	void AllOpenCamera(int nCount);

	// メンバ関数(取得)
	D3DXMATRIX GetMtxView(void) { return m_mtxView; }
	D3DXMATRIX* GetViewMtx(void) { return &m_mtxView; }
	D3DXMATRIX GetMtxProjection(void) { return m_mtxProjection; }
	D3DXMATRIX* GetProjectionMtx(void) { return &m_mtxProjection; }
	D3DXVECTOR3 GetRotation(void) { return m_rot; }
	D3DXVECTOR3 GetPositionV(void) { return m_posV; }
	D3DXVECTOR3 GetPositionR(void) { return m_posR; }
	D3DXVECTOR3 GetOldPositionV(void) { return m_OldposV; }
	D3DXVECTOR3 GetOldPositionR(void) { return m_OldposR; }
	D3DXVECTOR3 GetVectorU(void) { return m_vecU; }
	void SetRotation(D3DXVECTOR3 rot);
	void InerRot(const D3DXVECTOR3& rot, const float fMul);
	void SetLength(float fLength) { m_fLength = fLength; }
	CCamera *GetNext(void) { return m_pNext; }
	CCamera *GetPrev(void) { return m_pPrev; }
	DRAWSTATE GetDrawState(void) { return m_DrawState; }
	bool IsDebug() { return m_bDebug; }
	bool GetDraw(void) const { return m_bDraw; }
	float GetLength(void) { return m_fLength; }
	CCameraAction* GetAction() { return m_pAction; }

	// メンバ関数(設定)
	void SetMtxView(D3DXMATRIX mtxView) { m_mtxView = mtxView; }
	void SetMtxProjection(D3DXMATRIX mtxProjection) { m_mtxProjection = mtxProjection; }
	void SetNext(CCamera *pNext) { m_pNext = pNext; }
	void SetPrev(CCamera *pPrev) { m_pPrev = pPrev; }
	void SetDraw(const bool bDraw) { m_bDraw = bDraw; }
	void SetActive(const bool bActive) { m_bActive = bActive; }
	void BindId(int nId) { m_nId = nId; }

private:	// 自分だけがアクセス可能

	// メンバ関数
	void MoveR(void);
	void Slow(void);
	void Zoom(void);
	void FstPerson(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, const float fLength);

	// メンバ変数
	D3DXMATRIX m_mtxView;		// ビューマトリックス
	D3DXMATRIX m_mtxProjection;	// プロジェクションマトリックス
	D3DXVECTOR3 m_move;		// 移動量
	D3DXVECTOR3 m_posV;		// 視点
	D3DXVECTOR3 m_posR;		// 注視点
	D3DXVECTOR3 m_OldposV;	// 前の視点
	D3DXVECTOR3 m_OldposR;	// 前の注視点
	D3DXVECTOR3 m_vecU;		// 上方向ベクトル
	D3DXVECTOR3 m_rot;		// 向き
	D3DXVECTOR3 m_Oldrot;   // 前の向き
	D3DXVECTOR3 m_SlowOldRot;	// スロー前の向き
	D3DXVECTOR3 m_GoalPos;  // ゴールの位置
	CCameraAction* m_pAction;	// アクション用ポインタ
	MODE m_mode;				// モード
	DRAWSTATE m_DrawState;
	float m_fMulScore;		// スコア倍率
	float m_fLength;			// 視点と注視点の距離
	float m_fZoom;				// ズーム率
	float m_fDestZoom;			// 目標ズーム率
	CCamera *m_pNext;			// 次
	CCamera *m_pPrev;			// 前
	int m_nId;				// 使用コントローラー番号
	int m_nZoomCount;           // スタートドアを見ている時間
	bool m_bDraw;				// 描画
	bool m_bActive;			// 操作可能かどうか
	bool m_bDebug;
};

//**********************************************************
// 複数カメラクラスの定義
//**********************************************************
class CMultiCamera : public CCamera
{
public:	// 誰でもアクセス可能
	CMultiCamera();	// コンストラクタ
	~CMultiCamera();	// デストラクタ

	// メンバ関数
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void SetCamera(void) override;
	void SetViewPort(D3DVIEWPORT9 viewport) { m_viewport = viewport; }
	D3DVIEWPORT9 *GetViewPort(void) { return &m_viewport; }

private:	// 自分だけがアクセス可能

	// メンバ変数
	D3DVIEWPORT9 m_viewport;		//ビューポート
};

#endif