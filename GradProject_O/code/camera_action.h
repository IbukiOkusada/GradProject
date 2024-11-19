//==========================================================
//
// カメラアクションヘッダー [camera_action.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _CAMERAACTION_H_		// このマクロが定義されていない場合
#define _CAMERAACTION_H_		// 二重インクルード防止用マクロを定義

// 前方宣言
class CCamera;

//==========================================================
// カメラアクションのクラス定義
//==========================================================
class CCameraAction
{
public:

	// 移動列挙型
	enum MOVE
	{
		MOVE_POSV = 0,
		MOVE_POSR,
		MOVE_MAX
	};

private:

	// 開始地点構造体
	struct SStartInfo
	{
		D3DXVECTOR3 posV;	// 視点
		D3DXVECTOR3 posR;	// 注視点
		D3DXVECTOR3 rot;	// 向き
		float fLength;	// 距離

		SStartInfo() : posV(VECTOR3_ZERO), posR(VECTOR3_ZERO), rot(VECTOR3_ZERO), fLength(0.0f) {}
	};

	// 目標地点構造体
	struct STargetInfo
	{
		D3DXVECTOR3 rot;	// 向き
		float fLength;		// 距離

		STargetInfo() : rot(VECTOR3_ZERO), fLength(0.0f) {}
	};

	// 時間管理構造体
	struct STime
	{
		float fNow;		// 現在
		float fEnd;		// 終了
		float fStopNow;	// 停止時間現在
		float fStop;	// 終了後停止時間

		STime() : fNow(0.0f), fEnd(0.0f), fStopNow(0.0f), fStop(0.0f) {}
	};

public:	// 誰でもアクセス可能

	CCameraAction();	// コンストラクタ
	~CCameraAction();	// デストラクタ

	// メンバ関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(CCamera* pCamera);

	/**
	@brief	カメラの移動地点設定
	@param	pCamera		[in]	変更したいカメラ
	@param	rot			[in]	移動後の向き
	@param	fLength		[in]	移動後の距離
	@param	fTime		[in]	移動完了までの時間
	@param	fStopTime	[in]	移動完了までの時間
	@param	MOVE		[in]	移動する視点
	@param	bNext		[in]	次もモーションさせる予定があるか
	@return	void
	 */
	void Set(CCamera* pCamera, const D3DXVECTOR3& rot, const float& fLength, const float fTime, const float fStopTime, const MOVE& move, const bool& bNext = false);

	// メンバ関数(取得)
	bool IsFinish() { return m_bFinish; }
	bool IsNext() { return m_bNext; }
	bool IsPause() { return m_bPause; }

	// メンバ関数(設定)
	void SetFinish(bool bFinish) { m_bFinish = bFinish; }
	void SetNext(bool bNext) { m_bNext = bNext; }
	void SetPause(bool bPause) { m_bPause = bPause; }

private:	// 自分だけがアクセス可能

	// メンバ関数
	void Adjust(D3DXVECTOR3& rot);

	// メンバ変数
	SStartInfo m_startInfo;		// 開始地点情報
	STargetInfo m_targetInfo;	// ターゲット情報
	STime m_time;				// 時間
	bool m_bFinish;				// 終了
	bool m_bNext;				// 次の動作があるかどうか
	bool m_bPause;				// ポーズ
	MOVE m_move;
};

#endif

