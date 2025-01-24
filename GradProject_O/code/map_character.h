//==========================================================
//
// マップ配置のキャラクターの処理 [doll.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _MAP_CHARACTER_H_		// このマクロが定義されていない場合
#define _MAP_CHARACTER_H_		// 二重インクルード防止用マクロを定義

#include "task.h"
#include "objectX.h"
#include "player.h"
#include <list>
#include "task.h"
#include "effekseerControl.h"

// 前方宣言
class CCharacter;

//==========================================================
// ロボットのクラス定義
//==========================================================
class CMapCharacter : public CTask
{
public:	// 誰でもアクセス可能

	// 情報構造体
	struct SInfo
	{
		D3DXVECTOR3 pos;		// 位置
		D3DXVECTOR3 rot;		// 向き
		D3DXVECTOR3 scale;		// 移動量

		// コンストラクタ
		SInfo() : pos(VECTOR3_ZERO), rot(VECTOR3_ZERO),
			scale(VECTOR3_ZERO) {}
	};

private:

	

public:	// 誰でもアクセス可能

	CMapCharacter();	// コンストラクタ(オーバーロード)
	~CMapCharacter();	// デストラクタ

	// メンバ関数
	HRESULT Init();
	HRESULT Init(const D3DXVECTOR3& rot, const char* pPath, int nPlayMotion);
	void Uninit(void);
	void Update(void);
	static CMapCharacter* Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, const D3DXVECTOR3& scale, const char* pPath, int nPlayMotion);

	// メンバ関数(取得)
	D3DXVECTOR3 GetPosition(void) { return m_Info.pos; }
	D3DXVECTOR3 GetRotation(void) { return m_Info.rot; }
	CMapCharacter::SInfo GetInfo(void) { return m_Info; }

	// メンバ関数(設定)
	void SetScale(const D3DXVECTOR3& scale);
	void SetPosition(const D3DXVECTOR3& pos);
	void SetRotation(const D3DXVECTOR3& rot);

protected:	// 派生クラスからもアクセス可能

	// メンバ関数
	void Set();

	// メンバ変数
	CCharacter* m_pCharacter;	// キャラクター

private:	// 自分だけがアクセス可能

	// メンバ関数

	// メンバ変数
	SInfo m_Info;				// 自分自身の情報
};

#endif

