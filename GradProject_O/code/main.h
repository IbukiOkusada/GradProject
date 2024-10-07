//==========================================================
//
// メイン処理 [main.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _MAIN_H_		// このマクロが定義されていない場合
#define _MAIN_H_		// 二重インクルード防止用マクロを定義

//ライブラリのリンク
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")		// システム時刻取得に必要
#pragma comment(lib, "dinput8.lib")		// 入力処理に必要

//マクロ定義
#define SCREEN_WIDTH		(1280)			//ウインドウの幅
#define SCREEN_HEIGHT		(720)			//ウインドウの高さ
#define FVF_VERTEX_2D		(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)
#define FVF_VERTEX_3D		(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1)	//頂点フォーマット[3D]

//頂点情報(2D)の構造体を定義
typedef struct
{
	D3DXVECTOR3 pos;	//頂点座標
	float rhw;			//座標変換用係数(1.0fで固定)
	D3DCOLOR col;		//頂点カラー
	D3DXVECTOR2 tex;	//テクスチャ座標
}VERTEX_2D;

//頂点情報(3D)の構造体を定義
typedef struct
{
	D3DXVECTOR3 pos;	//頂点座標
	D3DXVECTOR3 nor;	//法線ベクトル
	D3DCOLOR col;		//頂点カラー
	D3DXVECTOR2 tex;	//テクスチャ座標
}VERTEX_3D;

//プロトタイプ宣言
int GetFPS(void);

#endif