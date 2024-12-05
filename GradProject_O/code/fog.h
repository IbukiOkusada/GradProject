//========================================================================================
//
// フォグヘッダー[fog.h]
// Author: 丹野 竜之介
//
//========================================================================================
#ifndef _FOG_H_
#define _FOG_H_
#include "main.h"

class CFog
{
public:
	CFog();
	~CFog();
	
	//プロトタイプ宣言
	void Init();
	void Uninit();
	void Update();
	void Draw();
	void Set(D3DFOGMODE FogMode, D3DXCOLOR FogColor, float FogStart, float FogEnd, float FogDensity);
private:
	D3DFOGMODE m_FogMode;	//フォグのモード
	D3DXCOLOR m_FogColor;	//フォグの色
	float m_FogStart;		//フォグの開始地点
	float m_FogEnd;		//フォグの終了地点
	float m_FogDensity;	//フォグの密度
};




#endif // !_FOG_H_
