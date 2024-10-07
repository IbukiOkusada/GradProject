//==========================================================
//
// メッシュフィールドの処理 [meshfield.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "meshfield.h"
#include "texture.h"
#include "manager.h"
#include "debugproc.h"
#include "input.h"
#include "player_manager.h"

CMeshField *CMeshField::m_pTop = NULL;	// 先頭のオブジェクトへのポインタ
CMeshField *CMeshField::m_pCur = NULL;	// 最後尾のオブジェクトへのポインタ

//==========================================================
// コンストラクタ
//==========================================================
CMeshField::CMeshField()
{
	m_pNext = NULL;
	m_pPrev = NULL;

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

	m_bHot = false;
	m_tex = D3DXVECTOR2(0.0f, 0.0f);
	m_texmove = D3DXVECTOR2(0.0f, 0.0f);
}

//==========================================================
// デストラクタ
//==========================================================
CMeshField::~CMeshField()
{

}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CMeshField::Init(void)
{
	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CMeshField::Uninit(void)
{
	//頂点バッファの廃棄
	CObjectMesh::Uninit();

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
}

//==========================================================
// 更新処理
//==========================================================
void CMeshField::Update(void)
{
	// テクスチャ更新
	m_tex += m_texmove;
	if (m_tex.x < 0.0f)
	{
		m_tex.x += 1.0f;
	}
	else if (m_tex.x > 1.0f)
	{
		m_tex.x -= 1.0f;
	}
	if (m_tex.y < 0.0f)
	{
		m_tex.y += 1.0f;
	}
	else if (m_tex.y > 1.0f)
	{
		m_tex.y -= 1.0f;
	}

	SetTex();
}

//==========================================================
// 描画処理
//==========================================================
void CMeshField::Draw(void)
{
	// 描画
	CObjectMesh::Draw();
}

//==========================================================
// 頂点情報設定
//==========================================================
void CMeshField::SetVtxInfo(void)
{
	int nVertex = GetVertex();			// 頂点数を取得
	int nNumWidth = GetNumWidth();		// 幅枚数を取得
	int nNumHeight = GetNumHeight();	// 高さ枚数を取得

	//頂点座標の設定(左奥から右手前に向かって頂点情報を設定する
	for (int nCntpVtx = 0; nCntpVtx < nVertex; nCntpVtx++)
	{
		//頂点座標
		m_pVtx[nCntpVtx].pos.x = -(m_fWidth * nNumWidth) + (nCntpVtx % (nNumWidth + 1) * (m_fWidth * 2));
		m_pVtx[nCntpVtx].pos.y = 0.0f;
		m_pVtx[nCntpVtx].pos.z = (m_fHeight * nNumHeight) + ((nCntpVtx / (nNumWidth + 1) * (-m_fHeight * 2)));

		//色
		m_pVtx[nCntpVtx].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		m_pVtx[nCntpVtx].tex = D3DXVECTOR2(m_tex.x + 1.0f * (nCntpVtx % (nNumWidth + 1)), m_tex.y + 1.0f * (nCntpVtx / (nNumWidth + 1)));
	}

	// 法線ベクトルの設定
	D3DXVECTOR3 nor, vec1, vec2;
	VERTEX_3D *aVtx[4];

	for (int nCntHeight = 0; nCntHeight < GetNumHeight(); nCntHeight++)
	{
		for (int nCntWidth = 0; nCntWidth < GetNumWidth(); nCntWidth++)
		{

			aVtx[0] = &m_pVtx[nCntHeight * (GetNumWidth() + 1) + nCntWidth + 0];
			aVtx[1] = &m_pVtx[nCntHeight * (GetNumWidth() + 1) + nCntWidth + 1];
			aVtx[2] = &m_pVtx[nCntHeight * (GetNumWidth() + 1) + nCntWidth + GetNumWidth() + 1];
			aVtx[3] = &m_pVtx[nCntHeight * (GetNumWidth() + 1) + nCntWidth + GetNumWidth() + 2];

			//Pos0からのベクトルを求める
			vec1 = aVtx[1]->pos - aVtx[0]->pos;
			vec2 = aVtx[2]->pos - aVtx[0]->pos;

			D3DXVec3Cross(&aVtx[0]->nor, &vec1, &vec2);

			D3DXVec3Normalize(&aVtx[0]->nor, &aVtx[0]->nor);	// ベクトルを正規化する

			//Pos3からのベクトルを求める
			vec1 = aVtx[2]->pos - aVtx[3]->pos;
			vec2 = aVtx[1]->pos - aVtx[3]->pos;

			D3DXVec3Cross(&aVtx[3]->nor, &vec1, &vec2);

			D3DXVec3Normalize(&aVtx[3]->nor, &aVtx[3]->nor);	// ベクトルを正規化する

			aVtx[1]->nor = (aVtx[0]->nor + aVtx[3]->nor) / 2;
			aVtx[2]->nor = (aVtx[0]->nor + aVtx[3]->nor) / 2;
		}
	}

	//for (int nCntpVtx = 0; nCntpVtx < nVertex; nCntpVtx++)
	//{
	//	if (nCntpVtx > 0 && nCntpVtx < nVertex - 1)
	//	{// 最初と最後以外
	//		if (nCntpVtx > GetNumWidth() + 1 && nCntpVtx < (GetNumWidth() + 1) * GetNumHeight() &&
	//			nCntpVtx % (GetNumWidth() + 1) > 0 && nCntpVtx % (GetNumWidth() + 1) < GetNumWidth())
	//		{// 6つの面と関わっている
	//			D3DXVECTOR3 norAdd = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	//			norAdd += m_pVtx[nCntpVtx - 1].nor;
	//			norAdd += m_pVtx[nCntpVtx + 1].nor;
	//			norAdd += m_pVtx[GetNumWidth() + 1].nor;
	//			norAdd += m_pVtx[GetNumWidth() + 2].nor;
	//			norAdd += m_pVtx[GetNumWidth() - 1].nor;
	//			norAdd += m_pVtx[GetNumWidth() - 2].nor;
	//			m_pVtx[nCntpVtx].nor = norAdd / 6;
	//		}
	//	}
	//}

	// 頂点設定
	SetVtx();
}

//==========================================================
// テクスチャ情報設定
//==========================================================
void CMeshField::SetTex(void)
{
	int nVertex = GetVertex();			// 頂点数を取得
	int nNumWidth = GetNumWidth();		// 幅枚数を取得

	// テクスチャ座標(左奥から右手前に向かって頂点情報を設定する
	for (int nCntpVtx = 0; nCntpVtx < nVertex; nCntpVtx++)
	{
		m_pVtx[nCntpVtx].tex = D3DXVECTOR2(m_tex.x + 1.0f * (nCntpVtx % (nNumWidth + 1)), m_tex.y + 1.0f * (nCntpVtx / (nNumWidth + 1)));
	}

	SetVtx();
}

//==========================================================
// 生成
//==========================================================
CMeshField *CMeshField::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, 
	float fWidth, float fHeight, const char *pFileName, const int nWidth, const int nHeight, const int nPriority)
{
	CMeshField *pMeshField = NULL;	// メッシュフィールドのポインタ
	CTexture *pTexture = CManager::GetInstance()->GetTexture();	// テクスチャへのポインタ

	// メモリの確保
	pMeshField = new CMeshField;

	if (pMeshField != NULL)
	{// 確保できた場合

		// 初期化
		pMeshField->Init();

		// 座標設定
		pMeshField->SetPosition(pos);

		// 向き設定
		pMeshField->SetRotation(rot);

		// 頂点生成
		pMeshField->CObjectMesh::Create(nWidth, nHeight);

		// サイズ設定
		pMeshField->SetSize(fWidth, fHeight);

		// テクスチャ設定
		pMeshField->BindTexture(pTexture->Regist(pFileName));
	}

	return pMeshField;
}


//==========================================================
// 幅設定
//==========================================================
void CMeshField::SetSize(float fWidth, float fHeight)
{
	// サイズ設定
	m_fWidth = fWidth;
	m_fHeight = fHeight;

	// 頂点情報設定
	SetVtxInfo();
}

//==========================================================
// 全ての床との判定
//==========================================================
float CMeshField::GetHeight(const D3DXVECTOR3& pos, const D3DXVECTOR3& posOld)
{
	CMeshField *pMesh = CMeshField::GetTop();	// 先頭を取得
	float fHeight = pos.y;	// 高さ
	D3DXVECTOR3 Pos0, Pos1, Pos2, Pos3;
	D3DXVECTOR3 vecToPos;	//判定用変数
	D3DXVECTOR3 vec1, vec2;	//判定用変数
	D3DXVECTOR3 nor0, nor3;
	float fRate, fRate2;	//判定用変数
	float fMaxField;		//判定用
	float fField, fField2;
	int nNowWidth = -1;		// 乗っている幅番号
	int nNowHeight = -1;	// 乗っている高さ番号

	while (pMesh != NULL)
	{// 使用されている間繰り返し
		CMeshField *pMeshNext = pMesh->GetNext();	// 次を保持
		D3DXVECTOR3 MeshPos = pMesh->GetPosition();

		if (pos.x < MeshPos.x + pMesh->m_pVtx[0].pos.x || pos.x > MeshPos.x + pMesh->m_pVtx[pMesh->GetVertex() - 1].pos.x ||
			pos.z > MeshPos.z + pMesh->m_pVtx[0].pos.z || pos.z < MeshPos.z + pMesh->m_pVtx[pMesh->GetVertex() - 1].pos.z)
		{// 範囲外
			pMesh = pMeshNext;	// 次に移動

			continue;
		}

		// サイズを取得
		vec1 = pMesh->m_pVtx[pMesh->GetVertex() - 1].pos - pMesh->m_pVtx[0].pos;
		vec2 = pos - (MeshPos + pMesh->m_pVtx[0].pos);

		if (vec1.x == 0.0f || vec1.z == 0.0f)
		{// 直線の場合
			return fHeight;
		}

		//xとz座標のメッシュ内の割合を求める
		nor0 = D3DXVECTOR3(vec2.x / vec1.x, 0.0f, vec2.z / vec1.z);

		// 幅の場所を取得
		nNowWidth = (int)((float)(pMesh->GetNumWidth() * nor0.x));

		// 高さの場所を取得
		nNowHeight = (int)((float)(pMesh->GetNumHeight() * nor0.z));

		if (nNowHeight * (pMesh->GetNumWidth() + 1) + nNowWidth + pMesh->GetNumWidth() + 2 >= pMesh->GetVertex())
		{
			pMesh = pMeshNext;	// 次に移動
			continue;
		}

		Pos0 = pMesh->m_pVtx[nNowHeight * (pMesh->GetNumWidth() + 1) + nNowWidth + pMesh->GetNumWidth() + 1].pos;
		Pos1 = pMesh->m_pVtx[nNowHeight * (pMesh->GetNumWidth() + 1) + nNowWidth + 0].pos;
		Pos2 = pMesh->m_pVtx[nNowHeight * (pMesh->GetNumWidth() + 1) + nNowWidth + pMesh->GetNumWidth() + 2].pos;
		Pos3 = pMesh->m_pVtx[nNowHeight * (pMesh->GetNumWidth() + 1) + nNowWidth + 1].pos;

		// Pos0からのベクトルを求める
		vec1 = Pos1 - Pos0;
		vec2 = Pos2 - Pos0;

		// 現在の座標のベクトルを求める
		vecToPos = D3DXVECTOR3(pos.x - (MeshPos.x + Pos0.x),
			pos.y - (MeshPos.y + Pos0.y),
			pos.z - (MeshPos.z + Pos0.z));

		D3DXVec3Cross(&nor0, &vec1, &vec2);

		D3DXVec3Normalize(&nor0, &nor0);	// ベクトルを正規化する

		// 面積を求める
		fMaxField = (vec1.x * vec2.z) - (vec1.z * vec2.x);

		// 現在の位置との面積を求める
		fField = (vecToPos.x * vec2.z) - (vecToPos.z * vec2.x);
		fField2 = (vecToPos.z * vec1.x) - (vecToPos.x * vec1.z);

		// 交点の割合を求める
		fRate = fField / fMaxField;
		fRate2 = fField2 / fMaxField;

		// 範囲内判定
		if (nor0.y != 0.0f)
		{
			if (fRate >= 0.0f && fRate <= 1.0f && fRate2 >= 0.0f && fRate2 <= 1.0f && (fRate + fRate2) <= 1.0f)
			{// 三角ポリゴンの中にいる
				float fValue = (-((pos.x - (MeshPos.x + Pos0.x)) * nor0.x) +
					-((pos.z - (MeshPos.z + Pos0.z)) * nor0.z)) / nor0.y + (MeshPos.y + Pos0.y);

				if (pos.y < fValue && posOld.y >= fValue)
				{
					fHeight = fValue;
				}
			}
		}

		// Pos3からのベクトルを求める
		vec1 = Pos2 - Pos3;
		vec2 = Pos1 - Pos3;

		D3DXVec3Cross(&nor3, &vec2, &vec1);

		D3DXVec3Normalize(&nor3, &nor3);	// ベクトルを正規化する

		// 現在の座標のベクトルを求める
		vecToPos = D3DXVECTOR3(pos.x - (MeshPos.x + Pos3.x),
			pos.y - (MeshPos.y + Pos3.y),
			pos.z - (MeshPos.z + Pos3.z));

		// 面積を求める
		fMaxField = (vec1.x * vec2.z) - (vec1.z * vec2.x);

		// 現在の位置との面積を求める
		fField = (vecToPos.x * vec2.z) - (vecToPos.z * vec2.x);
		fField2 = (vecToPos.z * vec1.x) - (vecToPos.x * vec1.z);

		// 交点の割合を求める
		fRate = fField / fMaxField;
		fRate2 = fField2 / fMaxField;

		// 範囲内判定
		if (nor0.y != 0.0f)
		{
			if (fRate >= 0.0f && fRate <= 1.0f && fRate2 >= 0.0f && fRate2 <= 1.0f && (fRate + fRate2) <= 1.0f)
			{// 三角ポリゴンの中にいる

				float fValue = (-((pos.x - (MeshPos.x + Pos3.x)) * nor3.x) +
					-((pos.z - (MeshPos.z + Pos3.z)) * nor3.z)) / nor3.y + (MeshPos.y + Pos3.y);

				if (pos.y < fValue && posOld.y >= fValue)
				{
					fHeight = fValue;
				}
			}
		}

		pMesh = pMeshNext;	// 次に移動
	}

	return fHeight;
}

//==========================================================
// メッシュ調整
//==========================================================
void CMeshField::Edit(float *pLength, float *pSpeed)
{
	CPlayer *pPlayer = CPlayerManager::GetInstance()->GetTop();

	if (pPlayer == NULL)
	{
		return;
	}

	CInputKeyboard *pInputKey = CManager::GetInstance()->GetInputKeyboard();
	D3DXVECTOR3 pos =D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	// リセット
	if (pInputKey->GetPress(DIK_F6) == true)
	{
		for (int nCntpVtx = 0; nCntpVtx < GetVertex(); nCntpVtx++)
		{
			m_pVtx[nCntpVtx].pos.y = 0.0f;
		}
	}

	// 範囲設定
	if (pInputKey->GetPress(DIK_5) == true)
	{
		*pLength += 1.0f;
	}
	else if (pInputKey->GetPress(DIK_6) == true)
	{
		*pLength -= 1.0f;
	}

	// 速度設定
	if (pInputKey->GetPress(DIK_Y) == true)
	{
		*pSpeed += 0.5f;
	}
	else if (pInputKey->GetPress(DIK_H) == true)
	{
		*pSpeed -= 0.5f;
	}

	// 幅設定
	if (pInputKey->GetPress(DIK_I) == true)
	{
		m_fWidth += 0.5f;
	}
	else if (pInputKey->GetPress(DIK_K) == true)
	{
		m_fWidth -= 0.5f;
	}

	// 高さ設定
	if (pInputKey->GetPress(DIK_O) == true)
	{
		m_fHeight += 1.0f;
	}
	else if (pInputKey->GetPress(DIK_L) == true)
	{
		m_fHeight -= 1.0f;
	}

	// 範囲内頂点調整
	if (pInputKey->GetPress(DIK_U) == true)
	{
		for (int nCntpVtx = 0; nCntpVtx < GetVertex(); nCntpVtx++)
		{
			D3DXVECTOR3 VtxPos = m_pVtx[nCntpVtx].pos + GetPosition();

			float fLength =
				sqrtf((VtxPos.x - pos.x) * (VtxPos.x - pos.x)
					+ (VtxPos.z - pos.z) * (VtxPos.z - pos.z));

			if (fLength <= *pLength)
			{// 範囲内の頂点
				m_pVtx[nCntpVtx].pos.y += *pSpeed;	// 座標を移動
			}
		}
	}
	else if (pInputKey->GetPress(DIK_J) == true)
	{
		for (int nCntpVtx = 0; nCntpVtx < GetVertex(); nCntpVtx++)
		{
			D3DXVECTOR3 VtxPos = m_pVtx[nCntpVtx].pos + GetPosition();

			float fLength =
				sqrtf((VtxPos.x - pos.x) * (VtxPos.x - pos.x)
					+ (VtxPos.z - pos.z) * (VtxPos.z - pos.z));

			if (fLength <= *pLength)
			{// 範囲内の頂点
				m_pVtx[nCntpVtx].pos.y -= *pSpeed;	// 座標を移動
			}
		}
	}

	// 頂点設定
	for (int nCntpVtx = 0; nCntpVtx < GetVertex(); nCntpVtx++)
	{
		//頂点座標
		m_pVtx[nCntpVtx].pos.x = -(m_fWidth * GetNumWidth()) + (nCntpVtx % (GetNumWidth() + 1) * (m_fWidth * 2));
		m_pVtx[nCntpVtx].pos.z = (m_fHeight * GetNumHeight()) + ((nCntpVtx / (GetNumWidth() + 1) * (-m_fHeight * 2)));
	}

	// 法線ベクトルの設定
	D3DXVECTOR3 nor, vec1, vec2;
	VERTEX_3D *aVtx[4];

	for (int nCntHeight = 0; nCntHeight < GetNumHeight(); nCntHeight++)
	{
		for (int nCntWidth = 0; nCntWidth < GetNumWidth(); nCntWidth++)
		{
			aVtx[0] = &m_pVtx[nCntHeight * (GetNumWidth() + 1) + nCntWidth + 0];
			aVtx[1] = &m_pVtx[nCntHeight * (GetNumWidth() + 1) + nCntWidth + 1];
			aVtx[2] = &m_pVtx[nCntHeight * (GetNumWidth() + 1) + nCntWidth + GetNumWidth() + 1];
			aVtx[3] = &m_pVtx[nCntHeight * (GetNumWidth() + 1) + nCntWidth + GetNumWidth() + 2];

			//Pos0からのベクトルを求める
			vec1 = aVtx[1]->pos - aVtx[0]->pos;
			vec2 = aVtx[2]->pos - aVtx[0]->pos;

			D3DXVec3Cross(&aVtx[0]->nor, &vec1, &vec2);

			D3DXVec3Normalize(&aVtx[0]->nor, &aVtx[0]->nor);	// ベクトルを正規化する

			//Pos3からのベクトルを求める
			vec1 = aVtx[2]->pos - aVtx[3]->pos;
			vec2 = aVtx[1]->pos - aVtx[3]->pos;

			D3DXVec3Cross(&aVtx[3]->nor, &vec1, &vec2);

			D3DXVec3Normalize(&aVtx[3]->nor, &aVtx[3]->nor);	// ベクトルを正規化する

			aVtx[1]->nor = (aVtx[0]->nor + aVtx[3]->nor) / 2;
			aVtx[2]->nor = (aVtx[0]->nor + aVtx[3]->nor) / 2;
		}
	}

	// 保存
	if (pInputKey->GetPress(DIK_F7) == true)
	{
		FILE *pFile = fopen("data\\TXT\\mesh.txt", "w");
		D3DXVECTOR3 ROT;

		if (pFile == NULL)
		{//ファイルが開けなかった場合
			return;
		}

		fprintf(pFile, "#==============================================================================\n"
			"# 起伏データファイル[mesh.txt]\n"
			"# Author : IBUKI OKUSADA\n"
			"#==============================================================================\n\n");
		fprintf(pFile, "#----------------------------------------------\n"
			"# 高さ情報\n"
			"#----------------------------------------------\n\n");

		// 頂点の高さを書き出し
		for (int nCntpVtx = 0; nCntpVtx < GetVertex(); nCntpVtx++)
		{
			fprintf(pFile, "	HEIGHT = %f	[%d]\n", m_pVtx[nCntpVtx].pos.y, nCntpVtx);
		}

		//ファイルを閉じる
		fclose(pFile);
	}

	// 頂点情報設定
	SetVtx();

	// デバッグ表示
	CManager::GetInstance()->GetDebugProc()->Print("\n-------------------------------------------------------\n");
	CManager::GetInstance()->GetDebugProc()->Print("メッシュフィールド起伏操作方法 ---------------\n");
	CManager::GetInstance()->GetDebugProc()->Print("リセット【F6】: 保存【F7】: 選択範囲【 %f 】: 範囲操作【 5, 6 】\n", *pLength);
	CManager::GetInstance()->GetDebugProc()->Print(" 頂点上昇 【 U, J 】: 速度変更【 Y, H 】[ %f ] : 頂点数-> 縦[ %d ], 横[ %d ]\n", *pSpeed, GetNumHeight(), GetNumWidth());
	CManager::GetInstance()->GetDebugProc()->Print(" 幅  【 I, K 】[ %f ]\n", m_fWidth);
	CManager::GetInstance()->GetDebugProc()->Print(" 高さ【 T, H 】[ %f ]\n", m_fHeight);
}

//==========================================================
// 起伏読み込み
//==========================================================
void CMeshField::UpDownLoad(const char *pFileName)
{
	FILE *pFile;	// ファイルへのポインタ
	int nVertex = 0;

	pFile = fopen(pFileName, "r");

	if (pFile != NULL)
	{//ファイルが開けた場合
		char aStr[256];

		//開始文字まで読み込む
		while (1)
		{
			//テキスト読み込み
			int nResult = fscanf(pFile, "%s", &aStr[0]);

			if (strcmp(&aStr[0], "HEIGHT") == 0)
			{//スクリプト開始の文字が確認できた場合
				fscanf(pFile, "%s", &aStr[0]);	// (=)読み込み
				fscanf(pFile, "%f", &m_pVtx[nVertex].pos.y);	// 高さ読み込み
				nVertex++;

				if (nVertex >= GetVertex())
				{
					break;
				}
			}
			else if (nResult == EOF)
			{//ファイルの最後まで読み込んでしまった場合
				break;
			}
		}
		//ファイルを閉じる
		fclose(pFile);
	}
	else
	{//ファイルが開けなかった場合
		return;
	}

	// 法線ベクトルの設定
	D3DXVECTOR3 nor, vec1, vec2;
	VERTEX_3D *aVtx[4];

	for (int nCntHeight = 0; nCntHeight < GetNumHeight(); nCntHeight++)
	{
		for (int nCntWidth = 0; nCntWidth < GetNumWidth(); nCntWidth++)
		{

			aVtx[0] = &m_pVtx[nCntHeight * (GetNumWidth() + 1) + nCntWidth + 0];
			aVtx[1] = &m_pVtx[nCntHeight * (GetNumWidth() + 1) + nCntWidth + 1];
			aVtx[2] = &m_pVtx[nCntHeight * (GetNumWidth() + 1) + nCntWidth + GetNumWidth() + 1];
			aVtx[3] = &m_pVtx[nCntHeight * (GetNumWidth() + 1) + nCntWidth + GetNumWidth() + 2];

			//Pos0からのベクトルを求める
			vec1 = aVtx[1]->pos - aVtx[0]->pos;
			vec2 = aVtx[2]->pos - aVtx[0]->pos;

			D3DXVec3Cross(&aVtx[0]->nor, &vec1, &vec2);

			D3DXVec3Normalize(&aVtx[0]->nor, &aVtx[0]->nor);	// ベクトルを正規化する

			//Pos3からのベクトルを求める
			vec1 = aVtx[2]->pos - aVtx[3]->pos;
			vec2 = aVtx[1]->pos - aVtx[3]->pos;

			D3DXVec3Cross(&aVtx[3]->nor, &vec1, &vec2);

			D3DXVec3Normalize(&aVtx[3]->nor, &aVtx[3]->nor);	// ベクトルを正規化する

			aVtx[1]->nor = (aVtx[0]->nor + aVtx[3]->nor) / 2;
			aVtx[2]->nor = (aVtx[0]->nor + aVtx[3]->nor) / 2;
		}
	}

	// 頂点情報設定
	SetVtx();
}

//==========================================================
// 現在地のエリア確認
//==========================================================
CMeshField *CMeshField::GetArea(D3DXVECTOR3 pos)
{
	int nCnt = 0;
	// 床の描画
	CMeshField *pMesh = CMeshField::GetTop();	// 先頭を取得

	while (pMesh != NULL)
	{// 使用されている間繰り返し
		CMeshField *pMeshNext = pMesh->GetNext();	// 次を保持
		D3DXVECTOR3 MeshPos = pMesh->GetPosition();

		if (pos.x > MeshPos.x + pMesh->m_pVtx[0].pos.x && pos.x < MeshPos.x + pMesh->m_pVtx[pMesh->GetVertex() - 1].pos.x &&
			pos.z < MeshPos.z + pMesh->m_pVtx[0].pos.z && pos.z > MeshPos.z + pMesh->m_pVtx[pMesh->GetVertex() - 1].pos.z)
		{// 範囲内
			return pMesh;
		}

		pMesh = pMeshNext;	// 次に移動

		nCnt++;
	}
	return NULL;
}