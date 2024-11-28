//<=================================================
//タイトルでの荷物関連処理
//
//Author : Kazuki Watanabe
//<=================================================
#ifndef _TITLEBAGGAGE_H_
#define _TITLEBAGGAGE_H_

#include "baggage.h"
#include "task.h"

class CCharacter;
//<===============================================
//荷物クラスを継承して定義
//<===============================================
class CTitleBaggage : public CBaggage
{
public:

	CTitleBaggage() {}	// コンストラクタ(オーバーロード)
	~CTitleBaggage() {}	// デストラクタ

	//メンバ関数
	HRESULT Init(void);
	void Uninit(void) { CBaggage::Uninit(); }
	void Update(void) { CBaggage::Update(); }
	static CTitleBaggage* Create(const D3DXVECTOR3& pos);

private:

};
//<===============================================
//ゴールクラスを継承して定義
//<===============================================
class CTitleGoal : public CTask
{
public:

	CTitleGoal() {}	// デストラクタ
	~CTitleGoal() {}	// デストラクタ

	//メンバ関数
	HRESULT Init(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot);
	HRESULT Init(void) { return S_OK; }
	void Uninit(void);
	void Update(void);
	static CTitleGoal* Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot);

	//取得
	CCharacter* GetCHR(void) {return m_pPeople;}
	D3DXVECTOR3 GetPos(void) {return m_rPos;}

private:

	CCharacter* m_pPeople;
	D3DXVECTOR3 m_rPos;

};

#endif
