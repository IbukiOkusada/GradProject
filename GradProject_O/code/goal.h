//==========================================================
//
// ゴール [goal.h]
// Author : 丹野竜之介
//
//==========================================================
#ifndef _GOAL_H_		// このマクロが定義されていない場合
#define _GOAL_H_		// 二重インクルード防止用マクロを定義

#include "task.h"
#include "list.h"
#include "effekseerControl.h"
#include "road.h"
#include "scrollText2D.h"

//#include "task.h"	// これでファイルインクルードできます
//前方宣言
class CMeshCylinder;
class CCharacter;
class CBaggage;
class CObjectX;

//==========================================================
// ゴールのクラス定義
//==========================================================
class CGoal : public CTask
{
public:

	// 基本情報構造体
	struct SInfo
	{
		D3DXVECTOR3 pos;				// 座標
		float fRange;					// ゴールの範囲
		float fLimit;					// 速度制限

		// コンストラクタ
		SInfo() : pos(VECTOR3_ZERO), fRange(0.0f), fLimit(0.0f){}
	};

private:

	// 人管理用構造体
	struct SPeople
	{
		CCharacter* pChara;		// 人
		D3DXVECTOR3 setpos;		// 相対座標

		// コンストラクタ
		SPeople() : pChara(nullptr), setpos(VECTOR3_ZERO) {}
	};

public:	// 誰でもアクセス可能

	CGoal(int nId);	// コンストラクタ(オーバーロード)
	~CGoal();	// デストラクタ

	// メンバ関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	static CGoal* Create(const D3DXVECTOR3& pos,float fRange,float fLimit, int nId);

	void SetEnd(int nId);
	void RecvEnd(int nId) { m_bRecvEnd = true; m_RecvId = nId; }
	
	// 設定
	void SetPos(const D3DXVECTOR3& pos);

	// 取得
	SInfo* GetInfo() { return &m_Info; }
	CRoad* GetRoad() { return m_pRoad; }
	D3DXVECTOR3 GetPos() { return m_Info.pos; }
	bool GetEnd() { return m_bEnd; }
	CEffekseer::CEffectData* GetEffect() { return pEffect; }
	float GetRange() { return m_Info.fRange; }
	int GetId() { return m_nId; }

private:	// 自分だけがアクセス可能

	// メンバ関数
	bool CheckRange(int nId);
	bool CheckSpeed(int nId);
	void ScreenEffect();

	// メンバ変数
	int m_nId;
	bool m_bEnd;	// 終了地点
	bool m_bRecvEnd;	// 通信用終了判定
	int m_RecvId;		// 通信用通過プレイヤーID 
	SInfo m_Info;
	CRoad * m_pRoad;
	CBaggage* m_pBaggage;
	CEffekseer::CEffectData* pEffect;
	SPeople m_People;	// 人の情報
	CScrollText2D* m_pFont;	// テキスト
};

#endif