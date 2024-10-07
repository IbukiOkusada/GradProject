//==========================================================
//
// 階層構造の処理全般 [character.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "character.h"
#include "manager.h"
#include "renderer.h"
#include "debugproc.h"
#include "texture.h"
#include "Xfile.h"
#include "model.h"
#include <string.h>
#include "motion.h"

//===================================================
// コンストラクタ
//===================================================
CCharacter::CCharacter()
{
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_ParentMtx = NULL;
	m_pMotion = NULL;
	m_ppParts = NULL;

	//ワールドマトリックスの初期化
	/*D3DXMatrixIdentity(&m_mtxWorld);*/
}

//===================================================
// デストラクタ
//===================================================
CCharacter::~CCharacter()
{

}

//===================================================
// 初期化処理
//===================================================
HRESULT CCharacter::Init(void)
{
	// ファイルを開く
	OpenFile("data\\TXT\\motion_sample.txt");

	return S_OK;
}

//===================================================
// 初期化処理
//===================================================
HRESULT CCharacter::Init(const char* pFileName)
{
	// ファイルを開く
	OpenFile(pFileName);

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CCharacter::Uninit(void)
{
	// モーション情報の終了
	if (m_pMotion != NULL)
	{
		m_pMotion->Uninit();
		delete m_pMotion;
		m_pMotion = NULL;
	}

	// パーツの終了
	if (m_ppParts != NULL)
	{// 使用していた場合
		for (int nCnt = 0; nCnt < m_nNumParts; nCnt++)
		{
			// 終了処理
			m_ppParts[nCnt]->Uninit();
			m_ppParts[nCnt] = NULL;	// 使用していない状態にする
		}

		delete[] m_ppParts;	// ポインタの開放
		m_ppParts = NULL;	// 使用していない状態にする
	}
}

//===================================================
// 更新処理
//===================================================
void CCharacter::Update(void)
{
	D3DXMATRIX mtxRot, mtxTrans;	//計算用マトリックス
	D3DXMATRIX mtxParent;			// 親のマトリックス情報

	//ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	SetRotation(m_rot);

	//向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	//位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	if (m_ParentMtx != NULL)
	{// 覚えている場合
		mtxParent = *m_ParentMtx;

		//パーツのマトリックスと親のマトリックスをかけ合わせる
		D3DXMatrixMultiply(&m_mtxWorld,
			&m_mtxWorld, &mtxParent);
	}

	if (m_pMotion != NULL)
	{// モーションを使用している場合
		m_pMotion->Update();
	}
}

//===================================================
// 描画処理
//===================================================
void CCharacter::Draw(void)
{

}

//==========================================================
// パーツの情報テキスト読み込み
//==========================================================
void CCharacter::OpenFile(const char *pFileData)
{
	FILE *pFile;

	pFile = fopen(pFileData, "r");

	if (pFile != NULL)
	{//ファイルが開けた場合
		char aStr[256];

		//開始文字まで読み込む
		while (1)
		{
			//テキスト読み込み
			int nResult = fscanf(pFile, "%s", &aStr[0]);

			if (strcmp(&aStr[0], "SCRIPT") == 0)
			{//スクリプト開始の文字が確認できた場合

				// 読み込み開始
				LoadFile(pFile);
				break;
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
}

//==========================================================
// パーツテキスト情報読み込み
//==========================================================
void CCharacter::LoadFile(FILE *pFile)
{
	char aStr[256] = "";
	char aStrOld[256];

	//情報読み込み開始
	while (1)
	{
		strcpy(&aStrOld[0], &aStr[0]);
		int nResult = fscanf(pFile, "%s", &aStr[0]);

		if (strcmp(&aStr[0], "NUM_MODEL") == 0)
		{//使用するモデル数の場合
			fscanf(pFile, "%s", &aStr[0]); //=
			fscanf(pFile, "%d", &m_nNumParts); //パーツ数取得

			m_ppParts = new CModel*[m_nNumParts];	// パーツ数分確保

			for (int nCnt = 0; nCnt < m_nNumParts; nCnt++)
			{
				m_ppParts[nCnt] = NULL;
			}

			//ファイル名取得
			LoadXFile(pFile);
		}
		else if (strcmp(&aStr[0], "CHARACTERSET") == 0)
		{//キャラクター情報の場合
			LoadSetData(pFile);
		}
		else if (strcmp(&aStr[0], "MOTIONSET") == 0)
		{//モーション情報の場合

			if (m_pMotion == NULL)
			{// モーションが確保されていない場合
				m_pMotion = new CMotion;
				m_pMotion->SetModel(m_ppParts, m_nNumParts);
			}

			if (m_pMotion != NULL)
			{// 使用されている場合
				m_pMotion->LoadMotionData(pFile);
			}
		}
		else if (strcmp(&aStr[0], "END_SCRIPT") == 0)
		{//スクリプト終了の文字が確認できた場合
			break;
		}
		else if (nResult == EOF)
		{//ファイルの最後まで読み込んでしまった場合
			break;
		}
	}
}

//==========================================================
// パーツモデルファイル名読み込み
//==========================================================
void CCharacter::LoadXFile(FILE *pFile)
{
	int nParts = 0;		//現在のパーツ番号
	char aStr[128] = {};	//配置数文字格納用

	while (1)
	{
		if (nParts >= m_nNumParts)
		{//パーツ総数まで到達した場合
			break;
		}
		else
		{//到達していない場合
			//テキスト読み込み
			int nResult = fscanf(pFile, "%s", &aStr[0]);

			if (strcmp(&aStr[0], "MODEL_FILENAME") == 0)
			{//モデル情報読み込み
				fscanf(pFile, "%s", &aStr[0]); //=
				fscanf(pFile, "%s", &aStr[0]); //ファイル名

				// モデルのインスタンスを生成してファイルを読み込む
				m_ppParts[nParts] = CModel::Create(&aStr[0]);

				nParts++;
			}
			if (strcmp(&aStr[0], "END_SCRIPT") == 0)
			{//スクリプト終了の文字が確認できた場合
				break;
			}
			else if (nResult == EOF)
			{//ファイルの最後まで読み込んでしまった場合
				break;
			}
		}
	}
}

//==========================================================
// パーツの初期設置情報読み込み
//==========================================================
void CCharacter::LoadSetData(FILE *pFile)
{
	char aStr[128] = {};	//配置数文字格納用
	int nCntParts = 0;
	while (1)
	{
		//テキスト読み込み
		fscanf(pFile, "%s", &aStr[0]);

		if (strcmp(&aStr[0], "PARTSSET") == 0)
		{//パーツ情報確認文字あった場合

			CMotion::KEY Key = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };	// 情報

			while (1)
			{
				//読み込み
				fscanf(pFile, "%s", &aStr[0]);

				if (nCntParts >= m_nNumParts)
				{//パーツ総数に到達した場合
					break;
				}
				if (strcmp(&aStr[0], "POS") == 0)
				{//座標
					fscanf(pFile, "%s", &aStr[0]);	//(=)読み込み
					fscanf(pFile, "%f", &Key.fPosX);	//x座標読み込み
					fscanf(pFile, "%f", &Key.fPosY);	//y座標読み込み
					fscanf(pFile, "%f", &Key.fPosZ);	//z座標読み込み

					// 初期情報を設定
					if (m_ppParts[nCntParts] != NULL)
					{// 使用されている場合
						m_ppParts[nCntParts]->SetPosition(D3DXVECTOR3(Key.fPosX, Key.fPosY, Key.fPosZ));
					}
				}
				else if (strcmp(&aStr[0], "ROT") == 0)
				{//向き
					fscanf(pFile, "%s", &aStr[0]);	//(=)読み込み
					fscanf(pFile, "%f", &Key.fRotX);	//x座標読み込み
					fscanf(pFile, "%f", &Key.fRotY);	//y座標読み込み
					fscanf(pFile, "%f", &Key.fRotZ);	//z座標読み込み

					// 初期情報を設定
					if (m_ppParts[nCntParts] != NULL)
					{// 使用されている場合
						m_ppParts[nCntParts]->SetRotation(D3DXVECTOR3(Key.fRotX, Key.fRotY, Key.fRotZ));
					}
				}
				else if (strcmp(&aStr[0], "INDEX") == 0)
				{//パーツ番号
					int nIdx = 0;
					fscanf(pFile, "%s", &aStr[0]);	//(=)読み込み
					fscanf(pFile, "%d", &nIdx);	//読み込み
				}
				else if (strcmp(&aStr[0], "PARENT") == 0)
				{//親番号
					int nIdxParent = 0;
					fscanf(pFile, "%s", &aStr[0]);		//(=)読み込み
					fscanf(pFile, "%d", &nIdxParent);	//読み込み

					if (nIdxParent >= 0 && nIdxParent <= m_nNumParts)
					{// パーツ総数の範囲内の場合
						if (m_ppParts[nIdxParent] != NULL && m_ppParts[nCntParts] != NULL)
						{// 親も自分自身も使用している場合
							m_ppParts[nCntParts]->SetParent(m_ppParts[nIdxParent]->GetMtx());
						}
					}
					else
					{
						m_ppParts[nCntParts]->SetParent(&m_mtxWorld);
					}
				}
				else if (strcmp(&aStr[0], "END_PARTSSET") == 0)
				{//パーツ情報終了文字確認できた場合
					nCntParts++;
				}
			}


		}

		if (strcmp(&aStr[0], "END_CHARACTERSET") == 0)
		{//キャラクター情報終了文字確認できた場合
			return;	//なにもせず返す
		}
	}
}

//==========================================================
// 生成
//==========================================================
CCharacter *CCharacter::Create(const char* pFileName)
{
	CCharacter *pCharacter = NULL;

	// キャラクターの生成
	pCharacter = new CCharacter;

	if (pCharacter != NULL)
	{// 生成できた場合
		// 初期化処理
		pCharacter->Init(pFileName);
	}
	else
	{// 生成に失敗した場合
		return NULL;
	}

	return pCharacter;
}

//==========================================================
// 生成(座標向き指定)
//==========================================================
CCharacter *CCharacter::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, const char* pFileName)
{
	CCharacter *pCharacter = NULL;

	// オブジェクト2Dの生成
	pCharacter = new CCharacter;

	if (pCharacter != NULL)
	{// 生成できた場合
		// 初期化処理
		pCharacter->Init(pFileName);

		// 座標設定
		pCharacter->SetPosition(pos);

		// 向き設定
		pCharacter->SetRotation(rot);
	}
	else
	{// 生成に失敗した場合
		return NULL;
	}

	return pCharacter;
}

//==========================================================
// 向き設定
//==========================================================
void CCharacter::SetRotation(const D3DXVECTOR3 rot)
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
	{// x座標角度限界
		m_rot.x += D3DX_PI * 2;
	}
	else if (m_rot.x > D3DX_PI)
	{// x座標角度限界
		m_rot.x += -D3DX_PI * 2;
	}

	if (m_rot.y < -D3DX_PI)
	{// x座標角度限界
		m_rot.y += D3DX_PI * 2;
	}
	else if (m_rot.y > D3DX_PI)
	{// x座標角度限界
		m_rot.y += -D3DX_PI * 2;
	}
}

//==========================================================
// パーツを取得
//==========================================================
CModel *CCharacter::GetParts(int nIdx)
{
	if (nIdx >= 0 && nIdx < m_nNumParts)
	{// 範囲内
		return m_ppParts[nIdx];
	}
	return NULL;
}

//==========================================================
// 描画設定
//==========================================================
void CCharacter::SetDraw(bool bDraw)
{
	if (m_ppParts != NULL)
	{// 使用していた場合
		for (int nCnt = 0; nCnt < m_nNumParts; nCnt++)
		{
			// 終了処理
			m_ppParts[nCnt]->SetDraw(bDraw);
		}
	}
}

//==========================================================
// 影設定
//==========================================================
void CCharacter::SetShadow(bool bShadow)
{
	if (m_ppParts != NULL)
	{// 使用していた場合
		for (int nCnt = 0; nCnt < m_nNumParts; nCnt++)
		{
			// 終了処理
			m_ppParts[nCnt]->SetShadow(bShadow);
			m_ppParts[nCnt]->SetCharaParent(&m_mtxWorld);
		}
	}
}

//==========================================================
// マトリックス設定
//==========================================================
void CCharacter::SetCharaMtx(void)
{
	if (m_ppParts != NULL)
	{// 使用していた場合
		for (int nCnt = 0; nCnt < m_nNumParts; nCnt++)
		{
			// 親のマトリックスを設定
			m_ppParts[nCnt]->SetCharaParent(&m_mtxWorld);
		}
	}
}

//==========================================================
// マテリアルカラー設定
//==========================================================
void CCharacter::SetChangeMatCol(const D3DXCOLOR& col)
{
	D3DMATERIAL9 mat;
	mat.Ambient = col;
	mat.Diffuse = col;
	mat.Emissive = col;
	mat.Power = col;
	mat.Specular = col;

	if (m_ppParts != NULL)
	{// 使用していた場合
		for (int nCnt = 0; nCnt < m_nNumParts; nCnt++)
		{
			// マテリアル設定
			m_ppParts[nCnt]->SetMaterial(mat);
		}
	}
}

//==========================================================
// マテリアル変更設定
//==========================================================
void CCharacter::SetChangeMat(const bool bChange)
{
	if (m_ppParts != NULL)
	{// 使用していた場合
		for (int nCnt = 0; nCnt < m_nNumParts; nCnt++)
		{
			// マテリアル設定
			m_ppParts[nCnt]->ChangeCol(bChange);
		}
	}
}