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

Clist<CMeshField*>* CMeshField::m_List = nullptr;

//==========================================================
// コンストラクタ
//==========================================================
CMeshField::CMeshField()
{
	if (m_List == nullptr)
	{
		m_List = m_List->Create();
	}

	m_List->Regist(this);
	m_bCollision = true;
	m_tex = D3DXVECTOR2(0.0f, 0.0f);
	m_texmove = D3DXVECTOR2(0.0f, 0.0f);
}

//==========================================================
// デストラクタ
//==========================================================
CMeshField::~CMeshField()
{
	m_List->Delete(this);

	if (m_List->Empty())
	{
		delete m_List;
		m_List = nullptr;
	}
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
	pMeshField = DEBUG_NEW CMeshField;

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
