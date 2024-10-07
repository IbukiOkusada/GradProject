 //==========================================================
//
// メッシュウォールの処理 [meshwall.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "meshwall.h"
#include "texture.h"
#include "manager.h"
#include "renderer.h"
#include "player.h"
#include "player_manager.h"
#include "camera.h"
#include "camera_manager.h"

// マクロ定義

// 静的メンバ変数宣言
CMeshWall *CMeshWall::m_pTop = NULL;	// 先頭のオブジェクトへのポインタ
CMeshWall *CMeshWall::m_pCur = NULL;	// 最後尾のオブジェクトへのポインタ

//==========================================================
// コンストラクタ
//==========================================================
CMeshWall::CMeshWall() : CObjectMesh(5)
{
	m_pNext = NULL;
	m_pPrev = NULL;
	m_type = TYPE_NORMAL;

	// 自分自身をリストに追加
	if (m_pTop != NULL)
	{// 先頭が存在している場合
		m_pCur->m_pNext = this;	// 現在最後尾のオブジェクトのポインタにつなげる
		m_pPrev = m_pCur;
		m_pCur = this;	// 自分自身が最後尾になる
	}
	else
	{// 存在しない場合
		m_pTop = this;	// 自分自身が先頭になる
		m_pCur = this;	// 自分自身が最後尾になる
	}
}

//==========================================================
// デストラクタ
//==========================================================
CMeshWall::~CMeshWall()
{

}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CMeshWall::Init(void)
{
	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CMeshWall::Uninit(void)
{
	// リストから自分自身を削除する
	if (m_pTop == this)
	{// 自身が先頭
		if (m_pNext != NULL)
		{// 次が存在している
			m_pTop = m_pNext;	// 次を先頭にする
			m_pNext->m_pPrev = NULL;	// 次の前のポインタを覚えていないようにする
		}
		else
		{// 存在していない
			m_pTop = NULL;	// 先頭がない状態にする
			m_pCur = NULL;	// 最後尾がない状態にする
		}
	}
	else if (m_pCur == this)
	{// 自身が最後尾
		if (m_pPrev != NULL)
		{// 次が存在している
			m_pCur = m_pPrev;			// 前を最後尾にする
			m_pPrev->m_pNext = NULL;	// 前の次のポインタを覚えていないようにする
		}
		else
		{// 存在していない
			m_pTop = NULL;	// 先頭がない状態にする
			m_pCur = NULL;	// 最後尾がない状態にする
		}
	}
	else
	{
		if (m_pNext != NULL)
		{
			m_pNext->m_pPrev = m_pPrev;	// 自身の次に前のポインタを覚えさせる
		}
		if (m_pPrev != NULL)
		{
			m_pPrev->m_pNext = m_pNext;	// 自身の前に次のポインタを覚えさせる
		}
	}

	//頂点バッファの廃棄
	CObjectMesh::Uninit();
}

//==========================================================
// 更新処理
//==========================================================
void CMeshWall::Update(void)
{

}

//==========================================================
// 描画処理
//==========================================================
void CMeshWall::Draw(void)
{
	LPDIRECT3DDEVICE9 pDevice;	//デバイスへのポインタ

	for (int nCntpVtx = 0; nCntpVtx < GetVertex(); nCntpVtx++)
	{
		m_pVtx[nCntpVtx].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	}

	// 
	SetVtx();
	SceltonWall();

	//デバイスの取得
	pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	// カリング設定
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// 描画
	CObjectMesh::Draw();

	// カリング設定直す
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

//==========================================================
// 頂点情報設定
//==========================================================
void CMeshWall::SetVtxInfo(void)
{
	int nVertex = GetVertex();			// 頂点数を取得
	int nNumWidth = GetNumWidth();		// 幅枚数を取得
	int nNumHeight = GetNumHeight();	// 高さ枚数を取得

	//頂点座標の設定(左奥から右手前に向かって頂点情報を設定する
	for (int nCntpVtx = 0; nCntpVtx < nVertex; nCntpVtx++)
	{
		//頂点座標
		m_pVtx[nCntpVtx].pos.x = -(m_fWidth * nNumWidth) + (nCntpVtx % (nNumWidth + 1) * (m_fWidth * 2));
		m_pVtx[nCntpVtx].pos.y = ((m_fHeight * 2) * nNumHeight) + ((nCntpVtx / (nNumWidth + 1) * (-m_fHeight * 2)));
		m_pVtx[nCntpVtx].pos.z = 0.0f;

		//法線
		m_pVtx[nCntpVtx].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

		//色
		m_pVtx[nCntpVtx].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		m_pVtx[nCntpVtx].tex = D3DXVECTOR2(1.0f * (nCntpVtx % (nNumWidth + 1)), 1.0f * (nCntpVtx / (nNumWidth + 1)));
	}

	// 頂点設定
	SetVtx();
}

//==========================================================
// 生成
//==========================================================
CMeshWall *CMeshWall::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, float fWidth, float fHeight,
	const char *pFileName, const int nWidth, const int nHeight, const int nPriority )
{
	CMeshWall *pMeshWall = NULL;	// メッシュフィールドのポインタ
	CTexture *pTexture = CManager::GetInstance()->GetTexture();	// テクスチャへのポインタ

	// メモリの確保
	pMeshWall = new CMeshWall;

	if (pMeshWall != NULL)
	{// 確保できた場合

		// 初期化
		pMeshWall->Init();

		// 座標設定
		pMeshWall->SetPosition(pos);

		// 向き設定
		pMeshWall->SetRotation(rot);

		// 頂点生成
		pMeshWall->CObjectMesh::Create(nWidth, nHeight);

		// サイズ設定
		pMeshWall->SetSize(fWidth, fHeight);

		// テクスチャ設定
		pMeshWall->BindTexture(pTexture->Regist(pFileName));
	}

	return pMeshWall;
}

//==========================================================
// 幅設定
//==========================================================
void CMeshWall::SetSize(float fWidth, float fHeight)
{
	// サイズ設定
	m_fWidth = fWidth;
	m_fHeight = fHeight;

	// 頂点情報設定
	SetVtxInfo();
}

//==========================================================
// 当たり判定
//==========================================================
D3DXVECTOR3 CMeshWall::Collision(D3DXVECTOR3 &pos, D3DXVECTOR3 &posOld, D3DXVECTOR3& move, const D3DXVECTOR3& vtxMax, const D3DXVECTOR3& vtxMin, CObjectX::COLLISION_AXIS& axis, int& nType)
{
	CMeshWall *pObj = m_pTop;
	D3DXVECTOR3 nor = { 0.0f, 0.0f, 0.0f };

	while (pObj != NULL)
	{
		bool bHit = false;
		CMeshWall *pObjNext = pObj->m_pNext;

		if (pos.y - vtxMin.y > pObj->GetPosition().y + pObj->GetHeight() * 2 * pObj->GetNumHeight() || pos.y + vtxMax.y < pObj->GetPosition().y) {	// 高さ範囲外
			pObj = pObjNext;
			continue;
		}

		if (pObj->GetRotation().y == 0.0f * D3DX_PI || pObj->GetRotation().y == 0.0f * -D3DX_PI)
		{//壁が正面を向いている場合(Z軸がマイナスの方を見ている)
			if (
				pos.x + vtxMax.x >= pObj->GetPosition().x - pObj->GetWidth() * pObj->GetNumWidth() &&
				pos.x + vtxMin.x <= pObj->GetPosition().x + pObj->GetWidth() * pObj->GetNumWidth() &&
				posOld.z + vtxMax.z <= pObj->GetPosition().z &&
				pos.z + vtxMax.z > pObj->GetPosition().z)
			{//当たっている場合
				pos.z = pObj->GetPosition().z - vtxMax.z;
				nor.z = pos.z - pObj->GetPosition().z;
				move.z = 0.0f;
				bHit = true;
			}
		}
		if (pObj->GetRotation().y == 0.5f * D3DX_PI)
		{//壁が右を向いている場合(X軸がプラスの方を見ている)

			if (
				pos.z + vtxMax.z >= pObj->GetPosition().z - pObj->GetWidth() * pObj->GetNumWidth() &&
				pos.z + vtxMin.z <= pObj->GetPosition().z + pObj->GetWidth() * pObj->GetNumWidth() &&
				posOld.x + vtxMax.x <= pObj->GetPosition().x &&
				pos.x + vtxMax.x > pObj->GetPosition().x)
			{//当たっている場合
				pos.x = pObj->GetPosition().x - vtxMax.x;
				nor.x = pos.x - pObj->GetPosition().x;
				move.x = 0.0f;
				bHit = true;
			}
		}
		if (pObj->GetRotation().y == -0.5f * D3DX_PI)
		{//壁が左を向いている場合(X軸がマイナスの方を見ている)
			if (
				pos.z + vtxMax.z >= pObj->GetPosition().z - pObj->GetWidth() * pObj->GetNumWidth() &&
				pos.z + vtxMin.z <= pObj->GetPosition().z + pObj->GetWidth() * pObj->GetNumWidth() &&
				posOld.x + vtxMin.x >= pObj->GetPosition().x &&
				pos.x + vtxMin.x < pObj->GetPosition().x)
			{//当たっている場合
				pos.x = pObj->GetPosition().x - vtxMin.x;
				nor.x = pos.x - pObj->GetPosition().x;
				move.x = 0.0f;
				bHit = true;
			}

		}
		if (pObj->GetRotation().y == 1.0f * D3DX_PI || pObj->GetRotation().y == 1.0f * -D3DX_PI)
		{//壁が奥を向いている場合(Z軸がプラスの方を見ている)
			if (
				pos.x + vtxMax.x >= pObj->GetPosition().x - pObj->GetWidth() * pObj->GetNumWidth() &&
				pos.x + vtxMin.x <= pObj->GetPosition().x + pObj->GetWidth() * pObj->GetNumWidth() &&
				posOld.z + vtxMin.z >= pObj->GetPosition().z &&
				pos.z + vtxMin.z < pObj->GetPosition().z)
			{//当たっている場合
				pos.z = pObj->GetPosition().z - vtxMin.z;
				nor.z = pos.z - pObj->GetPosition().z;
				move.z = 0.0f;
				bHit = true;
			}
		}

		if (bHit) {	// 当たった
			nType = pObj->m_type;
		}

		pObj = pObjNext;
	}

	D3DXVec3Normalize(&nor, &nor);

	return nor;
}

//==========================================================
// 種類設定
//==========================================================
void CMeshWall::SetType(const int type)
{
	m_type = type;
}

//==========================================================
// 壁の透過設定
//==========================================================
void CMeshWall::SceltonWall(void)
{
	D3DXVECTOR3 pVtxPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	//頂点の座標
	int nVerTex = GetVertex();
	D3DXVECTOR3 pos = GetPosition();
	D3DXVECTOR3 rot = GetRotation();
	float fHeight = GetHeight();
	float fWidth = GetWidth();
	int nNumWidth = GetNumWidth();		// 幅枚数を取得
	int nNumHeight = GetNumHeight();	// 高さ枚数を取得
	CCamera* pCamera = CCameraManager::GetInstance()->GetTop();
	CPlayer* pPlayer = CPlayerManager::GetInstance()->GetTop();

	// 距離を取る
	{
		//視点とモデルの距離を求める
		float fLength =
			sqrtf((pos.x - pPlayer->GetPosition().x) * (pos.x - pPlayer->GetPosition().x)
				+ (pos.z - pPlayer->GetPosition().z) * (pos.z - pPlayer->GetPosition().z));
		if (fLength >= fWidth * static_cast<float>(nNumWidth)) {
			return;
		}
	}

	//頂点座標の設定(左奥から右手前に向かって頂点情報を設定する
	for (int nCntpVtx = 0; nCntpVtx < nVerTex; nCntpVtx++)
	{
		pVtxPos = D3DXVECTOR3(
			pos.x + cosf(rot.y) * (-(fWidth * nNumWidth) + (nCntpVtx % (nNumWidth + 1) * (fWidth * 2))),
			pos.y + ((fHeight * 2) * nNumHeight) + ((nCntpVtx / (nNumWidth + 1) * (-fHeight * 2))),
			pos.z + sinf(rot.y) * ((fWidth * nNumWidth) + (nCntpVtx % (nNumWidth + 1) * (-fWidth * 2)))
		);

		//視点とモデルの距離を求める
		float fLength =
			sqrtf((pVtxPos.x - pCamera->GetPositionV().x) * (pVtxPos.x - pCamera->GetPositionV().x)
				+ (pVtxPos.y - pCamera->GetPositionV().y) * (pVtxPos.y - pCamera->GetPositionV().y)
				+ (pVtxPos.z - pCamera->GetPositionV().z) * (pVtxPos.z - pCamera->GetPositionV().z));

		float fPlayLength =
			sqrtf((pPlayer->GetPosition().x - pCamera->GetPositionV().x) * (pPlayer->GetPosition().x - pCamera->GetPositionV().x)
				+ (pPlayer->GetPosition().z - pCamera->GetPositionV().z) * (pPlayer->GetPosition().z - pCamera->GetPositionV().z));

		//角度を求める
		float fRotY = atan2f(pCamera->GetPositionV().z - pVtxPos.z, pCamera->GetPositionV().x - pVtxPos.x);
		float fDiff = pCamera->GetRotation().y - fRotY;

		if (fDiff > D3DX_PI)
		{
			fDiff += -D3DX_PI * 2.0f;
		}
		else if (fDiff < -D3DX_PI)
		{
			fDiff += D3DX_PI * 2.0f;
		}


		if (fLength <= 0)
		{
			fLength *= -1.0f;
		}

		if (fLength < fPlayLength
			&& fDiff >= -1.5f && fDiff <= 1.5f)
		{//距離が視点と注視点の距離より短い場合
			m_pVtx[nCntpVtx].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.2f);
		}
		else
		{
			m_pVtx[nCntpVtx].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		}
	}

	SetVtx();
}