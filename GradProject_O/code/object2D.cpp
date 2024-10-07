//===============================================
//
// オブジェクト(2D)の処理 [object2D.cpp]
// Author : Ibuki Okusada
//
//===============================================
#include "object2D.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"

//===============================================
// マクロ定義
//===============================================
#define VTX	(4)	// 頂点数
#define WIDTH	(100.0f)	// 幅
#define HEIGHT	(100.0f)	// 高さ
#define POS_X	(300.0f)	// X座標
#define POS_Y	(300.0f)	// Y座標
#define TEX_FILENAME "data\\TEXTURE\\meteor001.png"	// テクスチャ名

//====================================================================================
// オブジェクト2Dクラス
//====================================================================================
//===============================================
// コンストラクタ
//===============================================
//CObject2D::CObject2D()
//{
//	// 値をクリアする
//	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
//	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
//	m_fLength = 0.0f;
//	m_fAngle = 0.0f;
//	m_fWidth = 0.0f;
//	m_fHeight = 0.0f;
//}

//===============================================
// コンストラクタ(オーバーロード)
//===============================================
CObject2D::CObject2D(const D3DXVECTOR3 pos) : CObject(3)
{
	// 規定値を入れる
	m_pos = pos;
	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_fLength = 0.0f;
	m_fAngle = 0.0f;
	m_fWidth = 0.0f;
	m_fHeight = 0.0f;
	m_nIdxTexture = -1;
	m_col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
}

//===============================================
// コンストラクタ(オーバーロード)
//===============================================
CObject2D::CObject2D(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, const int nPriority) : CObject(nPriority)
{
	// 規定値を入れる
	m_pos = pos;
	m_rot = rot;
	m_fLength = 0.0f;
	m_fAngle = 0.0f;
	m_fWidth = 0.0f;
	m_fHeight = 0.0f;
	m_nIdxTexture = -1;
	m_col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
}

//===============================================
// コンストラクタ(オーバーロード)
//===============================================
CObject2D::CObject2D(int nPriority) : CObject(nPriority)
{
	// 値をクリアする
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_fLength = 0.0f;
	m_fAngle = 0.0f;
	m_fWidth = 0.0f;
	m_fHeight = 0.0f;
	m_nIdxTexture = -1;
	m_col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
}

//===============================================
// デストラクタ
//===============================================
CObject2D::~CObject2D()
{

}

//===============================================
// 初期化処理
//===============================================
HRESULT CObject2D::Init(void)
{
	LPDIRECT3DDEVICE9 pDevice;		//デバイスへのポインタ

	//デバイスの取得
	pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	// テクスチャの読み込み(割り当てるのでいらない)
	/*D3DXCreateTextureFromFile(pDevice, 
		TEX_FILENAME, 
		&m_pTexture);*/

	//頂点バッファの生成
	pDevice->CreateVertexBuffer(
		sizeof(VERTEX_2D) * VTX,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);
	
	if (m_pVtxBuff == NULL)
	{// 生成に失敗した場合
		return E_FAIL;
	}

	//対角線の長さを算出する
	m_fLength = sqrtf(WIDTH * WIDTH + HEIGHT * HEIGHT) * 0.5f;

	//対角線の角度を算出する
	m_fAngle = atan2f(WIDTH, HEIGHT);

	// 頂点情報設定
	SetVtx();

	return S_OK;
}

//===============================================
// 初期化処理
//===============================================
HRESULT CObject2D::Init(const char *pFileName)
{
	LPDIRECT3DDEVICE9 pDevice;		//デバイスへのポインタ

	//デバイスの取得
	pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	// テクスチャの読み込み(割り当てるのでいらない)
	/*D3DXCreateTextureFromFile(pDevice,
		pFileName,
		&m_pTexture);*/

	//頂点バッファの生成
	pDevice->CreateVertexBuffer(
		sizeof(VERTEX_2D) * VTX,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	if (m_pVtxBuff == NULL)
	{// 生成に失敗した場合
		return E_FAIL;
	}

	//対角線の長さを算出する
	m_fLength = sqrtf(m_fWidth * m_fWidth + m_fHeight * m_fHeight) * 0.5f;

	//対角線の角度を算出する
	m_fAngle = atan2f(m_fWidth, m_fHeight);

	// 頂点情報設定
	SetVtx();

	return S_OK;
}

//===============================================
// 終了処理
//===============================================
void CObject2D::Uninit(void)
{
	// 頂点バッファの廃棄
	if (m_pVtxBuff != NULL)
	{
		m_pVtxBuff->Release();
		m_pVtxBuff = NULL;
	}

	// 廃棄
	Release();
}

//===============================================
// 更新処理
//===============================================
void CObject2D::Update(void)
{
	// 頂点情報の更新
	//SetVtx();
}

//===============================================
// 描画処理
//===============================================
void CObject2D::Draw(void)
{
	LPDIRECT3DDEVICE9 pDevice;		//デバイスへのポインタ
	CTexture *pTexture = CManager::GetInstance()->GetTexture();

	//デバイスの取得
	pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	//頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(
		0,
		m_pVtxBuff,
		0,
		sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	// テクスチャの設定
	pDevice->SetTexture(0, pTexture->SetAddress(m_nIdxTexture));

	// 描画
	pDevice->DrawPrimitive(
		D3DPT_TRIANGLESTRIP,	//プリミティブの種類
		0,
		2	//頂点情報構造体のサイズ
	);
}

//===============================================
// 頂点情報設定
//===============================================
void CObject2D::SetVtx(void)
{
	VERTEX_2D *pVtx;

	//頂点バッファをロックし頂点情報へのポインタを取得
	m_pVtxBuff->Lock(
		0,
		0,
		(void**)&pVtx,
		0
	);

	//頂点座標の設定
	pVtx[0].pos.x = m_pos.x + sinf(m_rot.z + (-D3DX_PI + m_fAngle)) * m_fLength;
	pVtx[0].pos.y = m_pos.y + cosf(m_rot.z + (-D3DX_PI + m_fAngle)) * m_fLength;
	pVtx[0].pos.z = 0.0f;
	pVtx[1].pos.x = m_pos.x + sinf(m_rot.z + (D3DX_PI - m_fAngle)) * m_fLength;
	pVtx[1].pos.y = m_pos.y + cosf(m_rot.z + (D3DX_PI - m_fAngle)) * m_fLength;
	pVtx[1].pos.z = 0.0f;
	pVtx[2].pos.x = m_pos.x + sinf(m_rot.z + -m_fAngle) * m_fLength;
	pVtx[2].pos.y = m_pos.y + cosf(m_rot.z + -m_fAngle) * m_fLength;
	pVtx[2].pos.z = 0.0f;
	pVtx[3].pos.x = m_pos.x + sinf(m_rot.z + m_fAngle) * m_fLength;
	pVtx[3].pos.y = m_pos.y + cosf(m_rot.z + m_fAngle) * m_fLength;
	pVtx[3].pos.z = 0.0f;

	// rhwの設定
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	// 頂点カラーの設定
	pVtx[0].col = m_col;
	pVtx[1].col = m_col;
	pVtx[2].col = m_col;
	pVtx[3].col = m_col;

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	//頂点バッファをアンロックする
	m_pVtxBuff->Unlock();
}

//===============================================
// 頂点情報設定(パターンアニメーション)
//===============================================
void CObject2D::SetVtx(const int nPatternAnim, const int nTexWidth, const int nTexHeight)
{
	VERTEX_2D *pVtx;

	//頂点バッファをロックし頂点情報へのポインタを取得
	m_pVtxBuff->Lock(
		0,
		0,
		(void**)&pVtx,
		0
	);

	//頂点座標の設定
	pVtx[0].tex = D3DXVECTOR2(1.0f / nTexWidth * (float)nPatternAnim, (1.0f / nTexHeight * (float)(nPatternAnim / nTexWidth)));
	pVtx[1].tex = D3DXVECTOR2(1.0f / nTexWidth * (float)(nPatternAnim + 1), (1.0f / nTexHeight * (float)(nPatternAnim / nTexWidth)));
	pVtx[2].tex = D3DXVECTOR2(1.0f / nTexWidth * (float)nPatternAnim, (1.0f / nTexHeight * (float)(nPatternAnim / nTexWidth + 1)));
	pVtx[3].tex = D3DXVECTOR2(1.0f / nTexWidth * (float)(nPatternAnim + 1), (1.0f / nTexHeight * (float)(nPatternAnim / nTexWidth + 1)));

	//頂点バッファをアンロックする
	m_pVtxBuff->Unlock();
}

//===============================================
// 頂点情報設定(テクスチャアニメーション)
//===============================================
void CObject2D::SetVtx(const float fTexU, const float fTexV, const float fWidth, const float fHeight)
{
	VERTEX_2D *pVtx;

	//頂点バッファをロックし頂点情報へのポインタを取得
	m_pVtxBuff->Lock(
		0,
		0,
		(void**)&pVtx,
		0
	);

	//頂点座標の設定
	pVtx[0].pos.x = m_pos.x - fWidth;
	pVtx[0].pos.y = m_pos.y - fHeight;
	pVtx[0].pos.z = 0.0f;
	pVtx[1].pos.x = m_pos.x + fWidth;
	pVtx[1].pos.y = m_pos.y - fHeight;
	pVtx[1].pos.z = 0.0f;
	pVtx[2].pos.x = m_pos.x - fWidth;
	pVtx[2].pos.y = m_pos.y + fHeight;
	pVtx[2].pos.z = 0.0f;
	pVtx[3].pos.x = m_pos.x + fWidth;
	pVtx[3].pos.y = m_pos.y + fHeight;
	pVtx[3].pos.z = 0.0f;

	// rhwの設定
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	// 頂点カラーの設定
	pVtx[0].col = m_col;
	pVtx[1].col = m_col;
	pVtx[2].col = m_col;
	pVtx[3].col = m_col;

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(fTexU, fTexV);
	pVtx[1].tex = D3DXVECTOR2(fTexU + 1.0f, fTexV);
	pVtx[2].tex = D3DXVECTOR2(fTexU, fTexV + 1.0f);
	pVtx[3].tex = D3DXVECTOR2(fTexU + 1.0f, fTexV + 1.0f);

	//頂点バッファをアンロックする
	m_pVtxBuff->Unlock();
}

//===============================================
// 頂点情報設定
//===============================================
CObject2D *CObject2D::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, const int nPriority)
{
	CObject2D *pObject2D = NULL;

	// オブジェクト2Dの生成
	pObject2D = new CObject2D(pos, rot, nPriority);

	if (pObject2D != NULL)
	{// 生成できた場合
		// 初期化処理
		pObject2D->Init();

		// 種類設定
		pObject2D->SetType(TYPE_NONE);
	}
	else
	{// 生成に失敗した場合
		return NULL;
	}

	return pObject2D;
}

//===============================================
// 生成
//===============================================
CObject2D *CObject2D::Create(const int nPriority)
{
	CObject2D *pObject2D = NULL;

	// オブジェクト2Dの生成
	pObject2D = new CObject2D(nPriority);

	if (pObject2D != NULL)
	{// 生成できた場合
		// 初期化処理
		pObject2D->Init();

		// 種類設定
		pObject2D->SetType(TYPE_NONE);
	}
	else
	{// 生成に失敗した場合
		return NULL;
	}

	return pObject2D;
}

//===============================================
// 座標設定
//===============================================
void CObject2D::SetPosition(const D3DXVECTOR3 pos)
{
	m_pos = pos;
}

//===============================================
// 向き設定
//===============================================
void CObject2D::SetRotation(const D3DXVECTOR3 rot)
{
	m_rot = rot;

	if (m_rot.z < -D3DX_PI)
	{// z座標角度限界
		m_rot.z += D3DX_PI * 2;
	}
	else if (m_rot.z > D3DX_PI)
	{// z座標角度限界
		m_rot.z += -D3DX_PI * 2;
	}

	if (m_rot.x < -D3DX_PI)
	{// z座標角度限界
		m_rot.x += D3DX_PI * 2;
	}
	else if (m_rot.x > D3DX_PI)
	{// z座標角度限界
		m_rot.x += -D3DX_PI * 2;
	}

	if (m_rot.y < -D3DX_PI)
	{// z座標角度限界
		m_rot.y += D3DX_PI * 2;
	}
	else if (m_rot.y > D3DX_PI)
	{// z座標角度限界
		m_rot.y += -D3DX_PI * 2;
	}
}

//===============================================
// テクスチャ番号の割り当て
//===============================================
void CObject2D::BindTexture(int nIdx)
{
	m_nIdxTexture = nIdx;
}

//===============================================
// サイズ設定
//===============================================
void CObject2D::SetLength(float fWidth, float fHeight)
{
	m_fWidth = fWidth * 0.5f;
	m_fHeight = fHeight * 0.5f;
	//対角線の長さを算出する
	m_fLength = sqrtf(fWidth * fWidth + fHeight * fHeight) * 0.5f;

	//対角線の角度を算出する
	m_fAngle = atan2f(fWidth, fHeight);

	// 頂点情報設定
	SetVtx();
}

//===============================================
// 幅設定
//===============================================
void CObject2D::SetWidth(float fWidth)
{
	m_fWidth = fWidth;
}

//===============================================
// 高さ設定
//===============================================
void CObject2D::SetHeight(float fHeight)
{
	m_fHeight = fHeight;
}

//===============================================
// テクスチャ番号設定
//===============================================
void CObject2D::SetIdxTex(int nIdx)
{
	m_nIdxTexture = nIdx;
}

//===============================================
// 頂点情報の色変更
//===============================================
void CObject2D::SetCol(const D3DXCOLOR col)
{
	m_col = col;
	VERTEX_2D *pVtx;

	//頂点バッファをロックし頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点カラーの設定
	pVtx[0].col = col;
	pVtx[1].col = col;
	pVtx[2].col = col;
	pVtx[3].col = col;

	//頂点バッファをアンロックする
	m_pVtxBuff->Unlock();
}

//===============================================
// 対角線の長さ
//===============================================
void CObject2D::SetLength(const float fLength)
{
	m_fLength = fLength;	// 長さを取得
}

//===============================================
// 角度の長さ
//===============================================
void CObject2D::SetAngle(const float fAngle)
{
	m_fAngle = fAngle;	// 角度を取得
}

//===============================================
// サイズ設定
//===============================================
void CObject2D::SetSize(float fWidth, float fHeight)
{
	m_fHeight = fHeight;
	m_fWidth = fWidth;

	VERTEX_2D *pVtx;

	//頂点バッファをロックし頂点情報へのポインタを取得
	m_pVtxBuff->Lock(
		0,
		0,
		(void**)&pVtx,
		0
	);

	//頂点座標の設定
	pVtx[0].pos.x = m_pos.x - fWidth;
	pVtx[0].pos.y = m_pos.y - fHeight;
	pVtx[0].pos.z = 0.0f;
	pVtx[1].pos.x = m_pos.x + fWidth;
	pVtx[1].pos.y = m_pos.y - fHeight;
	pVtx[1].pos.z = 0.0f;
	pVtx[2].pos.x = m_pos.x - fWidth;
	pVtx[2].pos.y = m_pos.y + fHeight;
	pVtx[2].pos.z = 0.0f;
	pVtx[3].pos.x = m_pos.x + fWidth;
	pVtx[3].pos.y = m_pos.y + fHeight;
	pVtx[3].pos.z = 0.0f;

	//頂点バッファをアンロックする
	m_pVtxBuff->Unlock();
}

//===============================================
// プレイヤー用頂点情報設定
//===============================================
void CObject2D::SetPlayerVtx(void)
{
	VERTEX_2D *pVtx;

	//頂点バッファをロックし頂点情報へのポインタを取得
	m_pVtxBuff->Lock(
		0,
		0,
		(void**)&pVtx,
		0
	);

	//頂点座標の設定
	pVtx[0].pos.x = m_pos.x - m_fWidth;
	pVtx[0].pos.y = m_pos.y - m_fHeight;
	pVtx[0].pos.z = 0.0f;
	pVtx[1].pos.x = m_pos.x + m_fWidth;
	pVtx[1].pos.y = m_pos.y - m_fHeight;
	pVtx[1].pos.z = 0.0f;
	pVtx[2].pos.x = m_pos.x - m_fWidth;
	pVtx[2].pos.y = m_pos.y;
	pVtx[2].pos.z = 0.0f;
	pVtx[3].pos.x = m_pos.x + m_fWidth;
	pVtx[3].pos.y = m_pos.y;
	pVtx[3].pos.z = 0.0f;

	//頂点バッファをアンロックする
	m_pVtxBuff->Unlock();
}

//===============================================
// テクスチャ座標設定
//===============================================
void CObject2D::SetTex(const float fTexU, const float fTexV, const float fWidth, const float fHeight)
{
	VERTEX_2D *pVtx;

	//頂点バッファをロックし頂点情報へのポインタを取得
	m_pVtxBuff->Lock(
		0,
		0,
		(void**)&pVtx,
		0
	);

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(fTexU, fTexV);
	pVtx[1].tex = D3DXVECTOR2(fTexU + fWidth, fTexV);
	pVtx[2].tex = D3DXVECTOR2(fTexU, fTexV + fHeight);
	pVtx[3].tex = D3DXVECTOR2(fTexU + fWidth, fTexV + fHeight);

	//頂点バッファをアンロックする
	m_pVtxBuff->Unlock();
}