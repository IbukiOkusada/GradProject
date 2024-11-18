//==========================================================
//
// プレイヤーの処理 [player.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _PLAYER_H_		// このマクロが定義されていない場合
#define _PLAYER_H_		// 二重インクルード防止用マクロを定義

#include "main.h"
#include "object.h"
#include "objectX.h"
#include "task.h"
#include "effekseerControl.h"
#include "convenience.h"
#include "objectsound.h"
#include "radio.h"
#include "navi.h"
#include "road.h"
using namespace std;
// 前方宣言
class CWaist;
class CCharacter;
class CObjectX;
class CRoad;
class CBaggage;
class CMultiCamera;

// マクロ定義
#define MAX_ITEM  (1280)  // 所持できるアイテムの最大数

//==========================================================
// プレイヤーのクラス定義(派生クラス)
//==========================================================
class CPlayer : public CTask
{
public:

	// 操作種類列挙型
	enum TYPE
	{
		TYPE_NONE,		// 操作不可能
		TYPE_SEND,		// データ送信
		TYPE_ACTIVE,	// 操作可能
		TYPE_MAX
	};



private:	// 自分だけがアクセス可能な定義

	// 状態列挙型
	enum STATE
	{
		STATE_APPEAR = 0,	// 出現状態
		STATE_NORMAL,		// 通常状態
		STATE_NITRO	,		// ブースト状態
		STATE_DAMAGE,		// ダメージ状態
		STATE_DEATH,		// 死亡状態
		STATE_SPAWN,		// 復活中状態
		STATE_MAX
	};

	// 情報構造体
	struct SInfo
	{
		D3DXVECTOR3 pos;		// 位置
		D3DXVECTOR3 rot;		// 向き
		D3DXVECTOR3 move;		// 移動量
		float fSlideMove;		// スライディング開始移動量
		D3DXVECTOR3 posOld;		// 設定位置
		D3DXMATRIX mtxWorld;	// ワールドマトリックス
		STATE state;			// 状態
		float fStateCounter;	// 状態管理カウンター
		CRoad* pRoad;			// 最寄りの道
	};

public:	// 誰でもアクセス可能

	CPlayer();	// コンストラクタ
	~CPlayer();	// デストラクタ

	// メンバ関数
	HRESULT Init(void);
	HRESULT Init(const char *pBodyName, const char *pLegName);	// オーバーロード
	void Uninit(void);
	void Update(void);
	static CPlayer *Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, const D3DXVECTOR3 move,
		const char *pBodyName, const char *pLegName);

	// メンバ関数(設定)
	void SetMove(const D3DXVECTOR3 move) { m_Info.move = move; }
	void SetPosition(const D3DXVECTOR3 pos) { m_Info.pos = pos; }
	void SetRotation(const D3DXVECTOR3 rot) { m_Info.rot = rot; }
	void SetType(TYPE type);
	void BindId(int nId) { m_nId = nId; }
	void SetMotion(int nMotion);
	void SetDraw(bool bDraw);
	void SetNext(CPlayer* pNext) { m_pNext = pNext; }
	void SetPrev(CPlayer* pPrev) { m_pPrev = pPrev; }
	void SetRotDiff(float fDiff) { m_fRotDest = fDiff; }
	CBaggage* ThrowBaggage(D3DXVECTOR3* pTarget);
	void Damage(float fDamage);

	void SetNumDeliveryStatus(int nNum) { m_nNumDeliveryStatus = nNum; }
	void AddDeliveryCount(void) { m_nNumDeliveryStatus++; }
	
	// メンバ関数(取得)
	D3DXVECTOR3 GetMove(void) { return m_Info.move; }
	D3DXVECTOR3 GetPosition(void) { return m_Info.pos; }
	D3DXVECTOR3 GetRotation(void) { return m_Info.rot; }
	D3DXVECTOR3 GetOldPosition(void) { return m_Info.posOld; }
	CRoad* GetRoad(void) { return m_Info.pRoad; }
	CPlayer* GetNext(void) { return m_pNext; }
	CObjectX* GetObj() { return m_pObj; }
	CPlayer* GetPrev(void) { return m_pPrev; }
	int GetModelIndex(void) { return m_pObj->GetIdx(); }
	float GetEngine(void) { return m_fEngine; }
	int GetNumDeliverStatus(void) { return m_nNumDeliveryStatus; }
	float GetLifeOrigin() { return m_fLifeOrigin; }

protected:	// 自分だけがアクセス可能

	// メンバ関数
	void SetMatrix(void);
	void StateSet(void);
	void Controller(void);
	void Move(void);
	void Rotate(void);
	void Adjust(void);
	bool Collision(void);
	void Engine(float fThrottle);
	void SearchRoad(void);
	void Nitro();
	void GetBestPath();
	void DEBUGKEY();
	// メンバ変数
	CPlayer *m_pPrev;			// 前のオブジェクトへのポインタ
	CPlayer *m_pNext;			// 次のオブジェクトへのポインタ
	SInfo m_Info;				// 自分自身の情報
	float m_fRotMove;			// 現在の角度
	float m_fRotDiff;			// 目的の角度
	float m_fRotDest;			// 角度計算
	float m_fSpeed;				//速度(割合)
	float m_fBrake;
	float m_fEngine;
	float m_fTurnSpeed;
	float m_fHandle;
	float m_fLife;
	float m_fLifeOrigin;
	float m_fCamera;
	float m_fSlowRate = 1.0f;
	float m_fNitroCool;
	int m_nId;					// ID
	TYPE m_type;			// 種類
	CObjectX* m_pObj;		// 描画オブジェクト
	CBaggage* m_pBaggage;	// 荷物
	CNavi* m_pNavi;			// ナビ
	CEffekseer::CEffectData * m_pTailLamp;
	CEffekseer::CEffectData* m_pBackdust;
	CEffekseer::CEffectData* m_pAfterburner;
	CEffekseer::CEffectData* m_pDamageEffect;
	CMasterSound::CObjectSound* m_pSound;
	CMasterSound::CObjectSound* m_pSoundBrake;
	std::vector<CRoad::SSearch*> m_pPath;
	float m_fbrakeVolume;
	float m_fbrakePitch;
	CRadio* pRadio;
	CMasterSound::CObjectSound* m_pCollSound;
	int m_nNumDeliveryStatus;  // 配達した数
};


#endif
