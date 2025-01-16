//==========================================================
//
// プレイヤーの処理 [player.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _PLAYER_H_		// このマクロが定義されていない場合
#define _PLAYER_H_		// 二重インクルード防止用マクロを定義

#include "main.h"
#include "task.h"
#include "effekseerControl.h"
#include "objectsound.h"
#include "road.h"
#include "shaderlight.h"
using namespace std;

// 前方宣言
class CWaist;
class CCharacter;
class CObjectX;
class CBaggage;
class CMultiCamera;
class CPredRoute;
class CContainer;
class CNavi;
class CScrollText2D;
class CRadio;

// マクロ定義
#define MAX_ITEM  (1280)  // 所持できるアイテムの最大数
#define NUM_TXT  (4)  
//==========================================================
// プレイヤーのクラス定義(派生クラス)
//==========================================================
class CPlayer : public CTask
{
public:

	// 操作種類列挙型
	enum TYPE
	{
		TYPE_SEND = 0,			// データ送信
		TYPE_RECV,				// データ受信他操作
		TYPE_ACTIVE,			// 操作可能
		TYPE_GAMESTARTOK,		// 準備完了
		TYPE_TUTOLERIAL_ACTIVE,  // チュートリアル時
		TYPE_NONE,
		TYPE_MAX
	};

	// 状態列挙型
	enum STATE
	{
		STATE_APPEAR = 0,	// 出現状態
		STATE_NORMAL,		// 通常状態
		STATE_NITRO,		// ブースト状態
		STATE_DAMAGE,		// ダメージ状態
		STATE_DEATH,		// 死亡状態
		STATE_SPAWN,		// 復活中状態
		STATE_MAX
	};

	// モーション列挙型
	enum MOTION
	{
		MOTION_NEUTRAL = 0,	// 待機モーション
		MOTION_LEFT,		// 左曲がり
		MOTION_RIGHT,		// 左曲がり
		MOTION_WIN,			// 勝利モーション
		MOTION_CLEAR,		// クリアモーション
		MOTION_FAILED,		// 失敗モーション
		MOTION_THROW,		// 投げモーション
		MOTION_MAX
	};

private:	// 自分だけがアクセス可能な定義

	// 情報構造体
	struct SInfo
	{
		D3DXVECTOR3 pos;		// 位置
		D3DXVECTOR3 rot;		// 向き
		D3DXVECTOR3 move;		// 移動量
		float fSlideMove;		// スライディング開始移動量
		D3DXVECTOR3 posOld;		// 設定位置
		D3DXVECTOR3 rotOld;		// 過去向き
		D3DXMATRIX mtxWorld;	// ワールドマトリックス
		STATE state;			// 状態
		float fStateCounter;	// 状態管理カウンター
		CRoad* pRoad;			// 最寄りの道
		bool bHit;
		int bHitInterval;

		// コンストラクタ
		SInfo() : pos(VECTOR3_ZERO), rot(VECTOR3_ZERO), move(VECTOR3_ZERO), fSlideMove(0.0f), 
			posOld(VECTOR3_ZERO), mtxWorld(), state(STATE::STATE_NORMAL), fStateCounter(0.0f), pRoad(nullptr) {}
	};

	// 受信用情報構造体
	struct SRecvInfo
	{
		D3DXVECTOR3 pos;		// 位置
		D3DXVECTOR3 rot;		// 向き

		// コンストラクタ
		SRecvInfo() : pos(VECTOR3_ZERO), rot(VECTOR3_ZERO) {}
	};

public:	// 誰でもアクセス可能

	CPlayer(int nId = -1);
	virtual ~CPlayer();	// デストラクタ

	// メンバ関数
	HRESULT Init(void);
	HRESULT Init(const char *pBodyName, const char *pLegName);	// オーバーロード
	void Uninit(void);
	void Update(void);
	static CPlayer* Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, const D3DXVECTOR3& move,
		const int nId);

	// メンバ関数(設定)
	void SetMove(const D3DXVECTOR3& move) { m_Info.move = move; }
	void SetPosition(const D3DXVECTOR3& pos) { m_Info.pos = pos; }
	void SetRotation(const D3DXVECTOR3& rot) { m_Info.rot = rot; }
	void SetType(TYPE type);
	void BindId(int nId) { m_nId = nId; }
	void SetMotion(int nMotion);
	void SetDraw(bool bDraw);
	void SetRotDiff(float fDiff) { m_fRotDest = fDiff; }
	CBaggage* ThrowBaggage(D3DXVECTOR3* pTarget);
	void Damage(float fDamage);

	// 受信用情報
	void SetRecvPosition(const D3DXVECTOR3& pos) { m_RecvInfo.pos = pos; }
	void SetRecvRotation(const D3DXVECTOR3& rot) { m_RecvInfo.rot = rot; }

	void SetNumDeliveryStatus(int nNum) { m_nNumDeliveryStatus = nNum; }
	void AddDeliveryCount(void) { m_nNumDeliveryStatus++; }

	void EffectUninit(void);
	
	// メンバ関数(取得)
	D3DXVECTOR3& GetMove(void) { return m_Info.move; }
	D3DXVECTOR3& GetPosition(void) { return m_Info.pos; }
	D3DXVECTOR3& GetRotation(void) { return m_Info.rot; }
	D3DXVECTOR3& GetOldPosition(void) { return m_Info.posOld; }
	D3DXVECTOR3& GetOldRotation(void) { return m_Info.rotOld; }
	CRoad* GetRoad(void) { return m_Info.pRoad; }
	CObjectX* GetObj() { return m_pObj; }
	CCharacter* GetCharacteter() { return m_pCharacter; }
	CPredRoute* GetPredRoute() { return m_pPredRoute; }
	int GetModelIndex(void);
	STATE GetState(void) { return m_Info.state; }
	float GetEngine(void) { return m_fEngine; }
	float GetLife(void) { return m_fLife; }
	int GetNumDeliverStatus(void) { return m_nNumDeliveryStatus; }
	float GetLifeOrigin() { return m_fLifeOrigin; }
	int GetId() { return m_nId; }
	CRadio* GetRadio() { return m_pRadio; }
	TYPE GetType() { return m_type; }

protected:	// 自分だけがアクセス可能

	//=============================
	// 関数リスト
	//=============================
	typedef void(CPlayer::* SETTYPE_FUNC)();
	static SETTYPE_FUNC m_SetTypeFunc[];

	// 状態設定関数
	void SetStateSend();
	void SetStateRecv();
	void SetStateActive();
	void SetStateGameStartOk();
	void SetStateTutorialActive();
	void SetStateNone();

	// メンバ関数
	void SetMatrix(void);
	void StateSet(void);
	void Controller(void);
	void Move(void);
	void Rotate(void);
	bool Collision(void);
	bool CollisionObjX(void);
	bool CollisionEnemy(void);
	bool CollisionRoad(void);
	bool CollisionGimick(void);
	bool CollisionField(void);
	void Engine(float fThrottle);
	void SearchRoad(void);
	void Nitro();
	void GetBestPath();
	void DEBUGKEY();
	void SendData();
	void Respawn();
	void SetCol();
	void CameraMove(const D3DXVECTOR3& rot);

	// 受信用メンバ関数
	void RecvInerSet();

	// メンバ変数
	SInfo m_Info;				// 自分自身の情報
	float m_fRotMove;			// 現在の角度
	float m_fRotDiff;			// 目的の角度
	float m_fRotDest;			// 角度計算

	// 速度計算用
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

	int m_nId;				// ID
	TYPE m_type;			// 種類
	CObjectX* m_pObj;		// バイク
	CCharacter* m_pCharacter;	// キャラクター
	CBaggage* m_pBaggage;	// 荷物
	CNavi* m_pNavi;			// ナビ
	CContainer* m_pContainer;
	CPredRoute* m_pPredRoute;	// 予測用
	std::vector<CRoad::SSearch*> m_pPath;
	CScrollText2D* m_pFont[NUM_TXT];
	int m_nNumDeliveryStatus;  // 配達した数

	// えふぇくしあ
	CEffekseer::CEffectData * m_pTailLamp;
	CEffekseer::CEffectData* m_pBackdust;
	CEffekseer::CEffectData* m_pAfterburner;
	CEffekseer::CEffectData* m_pDamageEffect;
	CEffekseer::CEffectData* m_pDust;
	// サウンド
	CMasterSound::CObjectSound* m_pSound;
	CMasterSound::CObjectSound* m_pSoundBrake;
	CMasterSound::CObjectSound* m_pCollSound;
	CMasterSound::CObjectSound* m_pambient;
	float m_fbrakeVolume;
	float m_fbrakePitch;
	CRadio* m_pRadio;
	//シェーダーライト
	CShaderLight::SLight* m_pShaderLight;
	// オンライン関連
	SRecvInfo m_RecvInfo;		// 受信情報
};


#endif
