//==========================================================
//
// ギミック [Gimmick.h]
// Author : 丹野竜之介
//
//==========================================================
#ifndef _Gimmick_H_		// このマクロが定義されていない場合
#define _Gimmick_H_		// 二重インクルード防止用マクロを定義

#include "task.h"
//#include "task.h"	// これでファイルインクルードできます

//==========================================================
// ギミックのクラス定義
//==========================================================
class CGimmick : public CTask
{
private:

	// 基本情報
	struct SInfo
	{
		D3DXVECTOR3 pos;	// 位置
		D3DXVECTOR3 rot;	// 向き
		D3DXVECTOR3 scale;	// スケール

		// コンストラクタ
		SInfo() : pos(VECTOR3_ZERO), rot(VECTOR3_ZERO), scale(VECTOR3_ONE){}
	};

public:	// 誰でもアクセス可能

	CGimmick();	// コンストラクタ(オーバーロード)
	virtual ~CGimmick();	// デストラクタ

	// メンバ関数
	virtual HRESULT Init(void) = 0;
	virtual void Uninit(void);
	virtual void Update(void)= 0;
	// メンバ関数
	D3DXVECTOR3 GetPos(){ return m_Info.pos; }
	D3DXVECTOR3 GetRot() { return m_Info.rot; }
	D3DXVECTOR3 GetScale() { return m_Info.scale; }
	void SetPos(D3DXVECTOR3 pos) { m_Info.pos = pos; }
	void SetRot(D3DXVECTOR3 rot) { m_Info.rot = rot; }
	void SetScale(D3DXVECTOR3 scale) { m_Info.scale = scale; }

private:	// 自分だけがアクセス可能

	// メンバ関数

	// メンバ変数
	SInfo m_Info;
};

#endif

