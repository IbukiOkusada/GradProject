//==========================================================
//
// Xファイルモデルの処理全般 [objectX.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "main.h"
#include "objectX.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"
#include "Xfile.h"
#include "fxmanager.h"
#include "edit_manager.h"
#include "game.h"
#include "camera.h"
#include "camera_manager.h"
#include "camera_action.h"

// 静的メンバ変数宣言
Clist<CObjectX*> CObjectX::m_List = {};

//==========================================================
// コンストラクタ
//==========================================================
CObjectX::CObjectX(int nPriority) : CObject(nPriority)
{
	m_nIdxModel = -1;
	m_bEnableCollision = true;
	m_fShadowHeight = 0.0f;
	m_ColMulti = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	m_AddCol = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);
	m_scale = VECTOR3_ZERO;
	m_pos = VECTOR3_ZERO;
	m_rot = VECTOR3_ZERO;
	m_bHit = false;
	m_bHitOld = false;
	m_bShadow = true;
	SetDrawShader(true);
	m_List.Regist(this);
}

//==========================================================
// デストラクタ
//==========================================================
CObjectX::~CObjectX()
{

}

//==========================================================
//モデルの初期化処理
//==========================================================
HRESULT CObjectX::Init(void)
{
	//各種変数の初期化
	m_Type = TYPE_NORMAL;
	m_scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	return S_OK;
}

//==========================================================
//モデルの終了処理
//==========================================================
void CObjectX::Uninit(void)
{
	if (!GetDeath())
	{
		// リストから外す
		ListOut();

		// 廃棄
		Release();
	}
}

//==========================================================
//モデルの更新処理
//==========================================================
void CObjectX::Update(void)
{

}

//==========================================================
//モデルの描画処理
//==========================================================
void CObjectX::Draw(void)
{
	
		Update();
	

	m_bHitOld = m_bHit;
	m_bHit = false;

	// マトリックス計算
	if (m_Type == TYPE_NORMAL)
	{
		CalWorldMtx();
	}
	else
	{
		Quaternion();
	}

	// 描画
	DrawOnry();

	// 影の描画
	DrawShadow();
}
//==========================================================
//モデルの描画処理
//==========================================================
void CObjectX::DrawOnShader(void)
{
		//Update();

	m_bHitOld = m_bHit;
	m_bHit = false;

	// マトリックス計算
	if (m_Type == TYPE_NORMAL)
	{
		CalWorldMtx();
	}
	else
	{
		Quaternion();
	}

	// 描画
	if (CEditManager::GetInstance() == nullptr)
	{
		//DrawOnry();
		DrawOnryShader();
	}
	else
	{
		//DrawOnry();
		DrawOnryShader();
	}

	// 影の描画
	DrawShadow();
}
//==========================================================
// ワールドマトリックス計算
//==========================================================
void CObjectX::CalWorldMtx()
{
	D3DXMATRIX mtxRot, mtxTrans;			//計算用マトリックス

	//ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// スケールの反映
	D3DXMatrixScaling(&m_mtxscale, m_scale.x, m_scale.y, m_scale.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &m_mtxscale);

	//向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	//位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);
}

//==========================================================
// クォータニオンワールドマトリックス計算
//==========================================================
void CObjectX::Quaternion()
{
	D3DXMATRIX mtxRot, mtxTrans;			//計算用マトリックス
	D3DXQUATERNION qYaw, qPitch, qRoll;
	D3DXVECTOR3 quaternion = VECTOR3_ZERO;
	quaternion.y = 1.0f;

	// ヨー: Y軸回転
	D3DXQuaternionRotationAxis(&qYaw, &quaternion, m_rot.y);

	quaternion.y = 0.0f;
	quaternion.x = 1.0f;
	// ピッチ: X軸回転
	D3DXQuaternionRotationAxis(&qPitch, &quaternion, m_rot.x);

	quaternion.x = 0.0f;
	quaternion.z = 1.0f;
	// ロール: Z軸回転
	D3DXQuaternionRotationAxis(&qRoll, &quaternion, m_rot.z);
	D3DXQUATERNION qResult;
	D3DXQuaternionMultiply(&qResult, &qRoll, &qPitch);  // ロールとピッチを掛け合わせ
	D3DXQuaternionMultiply(&qResult, &qResult, &qYaw);  // さらにヨーを掛け合わせ

	//ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// スケールの反映
	D3DXMatrixScaling(&m_mtxscale, m_scale.x, m_scale.y, m_scale.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &m_mtxscale);

	//向きを反映
	D3DXMatrixRotationQuaternion(&mtxRot, &qResult);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	//位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);
}
//==========================================================
// 描画のみ
//==========================================================
void CObjectX::DrawOnry()
{
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();		//デバイスへのポインタを取得
	CTexture* pTexture = CManager::GetInstance()->GetTexture();	// テクスチャへのポインタ
	CXFile* pModelFile = CManager::GetInstance()->GetModelFile();	// Xファイル情報のポインタ
	CXFile::SFileData* pFileData = pModelFile->SetAddress(m_nIdxModel);
	D3DMATERIAL9 matDef;					//現在のマテリアル保存用
	D3DXMATERIAL* pMat;						//マテリアルデータへのポインタ

	// 正規化を有効にする
	pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);

	// モデル使用されていない
	if (pFileData == nullptr) { return; }

	//アルファテストを有効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 10);

	//ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	//現在のマテリアルを取得
	pDevice->GetMaterial(&matDef);

	//マテリアルデータへのポインタを取得
	pMat = (D3DXMATERIAL*)pFileData->pBuffMat->GetBufferPointer();
	for (int nCntMat = 0; nCntMat < (int)pFileData->dwNumMat; nCntMat++)
	{
		D3DMATERIAL9 mat = pMat[nCntMat].MatD3D;
		mat.Diffuse.r = mat.Diffuse.r * m_ColMulti.r + m_AddCol.r;
		mat.Diffuse.g = mat.Diffuse.g * m_ColMulti.g + m_AddCol.g;
		mat.Diffuse.b = mat.Diffuse.b * m_ColMulti.b + m_AddCol.b;
		mat.Diffuse.a = mat.Diffuse.a * m_ColMulti.a + m_AddCol.a;
		mat.Ambient.r = mat.Ambient.r * m_ColMulti.r + m_AddCol.r;
		mat.Ambient.g = mat.Ambient.g * m_ColMulti.g + m_AddCol.g;
		mat.Ambient.b = mat.Ambient.b * m_ColMulti.b + m_AddCol.b;
		mat.Ambient.a = mat.Ambient.a * m_ColMulti.a + m_AddCol.a;

		//マテリアルの設定
		pDevice->SetMaterial(&mat);

		//テクスチャの設定
		pDevice->SetTexture(0, pTexture->SetAddress(pFileData->pIdexTexture[nCntMat]));

		//モデル(パーツ)の描画
		pFileData->pMesh->DrawSubset(nCntMat);
	}

	//保存していたマテリアルを戻す
	pDevice->SetMaterial(&matDef);

	//アルファテストを無効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 255);

	// 正規化を無効にする
	pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, FALSE);
}
//==========================================================
// シェーダー描画
//==========================================================
void CObjectX::DrawOnryShader()
{
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();		//デバイスへのポインタを取得
	CFXManager* pFx = CFXManager::GetInstance();
	CTexture* pTexture = CManager::GetInstance()->GetTexture();	// テクスチャへのポインタ
	CXFile* pModelFile = CManager::GetInstance()->GetModelFile();	// Xファイル情報のポインタ
	CXFile::SFileData* pFileData = pModelFile->SetAddress(m_nIdxModel);
	D3DMATERIAL9 matDef;					//現在のマテリアル保存用
	D3DXMATERIAL* pMat;						//マテリアルデータへのポインタ

	// 正規化を有効にする
	pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);

	// モデル使用されていない
	if (pFileData == nullptr) { return; }

	//アルファテストを有効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 10);

	pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	pDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	//ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);
	pFx->SetMatrixWorld(m_mtxWorld);
	pFx->SetScale(m_mtxscale);
	//現在のマテリアルを取得
	pDevice->GetMaterial(&matDef);

	//マテリアルデータへのポインタを取得
	pMat = (D3DXMATERIAL*)pFileData->pBuffMat->GetBufferPointer();
	for (int nCntMat = 0; nCntMat < (int)pFileData->dwNumMat; nCntMat++)
	{
		D3DMATERIAL9 mat = pMat[nCntMat].MatD3D;
		mat.Diffuse.r = mat.Diffuse.r * m_ColMulti.r + m_AddCol.r;
		mat.Diffuse.g = mat.Diffuse.g * m_ColMulti.g + m_AddCol.g;
		mat.Diffuse.b = mat.Diffuse.b * m_ColMulti.b + m_AddCol.b;
		mat.Diffuse.a = mat.Diffuse.a * m_ColMulti.a + m_AddCol.a;
		
		//マテリアルの設定
		pDevice->SetMaterial(&mat);
		pFx->SetMaterial(mat);
		//テクスチャの設定
	
		pFx->SetParamToEffect();
		pFx->Begin();
		pFx->BeginPass();
		pDevice->SetTexture(0, pTexture->SetAddress(pFileData->pIdexTexture[nCntMat]));
		//モデル(パーツ)の描画
		pFileData->pMesh->DrawSubset(nCntMat);
		pFx->EndPass();
		pFx->End();
	}

	//保存していたマテリアルを戻す
	pDevice->SetMaterial(&matDef);

	//アルファテストを無効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 255);

	// 正規化を無効にする
	pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, FALSE);
}
//==========================================================
// シャドウマトリックスでの影の描画
//==========================================================
void CObjectX::DrawShadow()
{
	// 影使用しない
	if (!m_bShadow) { return; }

	// モデル使用されていない
	CXFile* pModelFile = CManager::GetInstance()->GetModelFile();	// Xファイル情報のポインタ
	CXFile::SFileData* pFileData = pModelFile->SetAddress(m_nIdxModel);
	if (pFileData == nullptr) { return; }

	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();		//デバイスへのポインタを取得
	D3DMATERIAL9 matDef;					//現在のマテリアル保存用
	D3DXMATERIAL* pMat;						//マテリアルデータへのポインタ
	D3DXMATRIX mtxShadow;
	D3DLIGHT9 light;
	D3DXVECTOR4 posLight;
	D3DXVECTOR3 pos, normal;
	D3DXPLANE plane;

	// ライトの位置を設定
	pDevice->GetLight(0, &light);
	posLight = D3DXVECTOR4(-light.Direction.x, -light.Direction.y, -light.Direction.z, 0.0f);

	// 平面情報を設定
	D3DXVECTOR3 DefPos = D3DXVECTOR3(m_mtxWorld._41, m_mtxWorld._42, m_mtxWorld._43);
	pos = D3DXVECTOR3(m_mtxWorld._41, m_mtxWorld._42, m_mtxWorld._43);

	if (pos.y > DefPos.y) {	// 現在の腕の位置よりも高い
		pos = DefPos;
	}

	pos.y = m_fShadowHeight + 0.1f;
	normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	D3DXPlaneFromPointNormal(&plane, &pos, &normal);

	// シャドウマトリックスの初期化
	D3DXMatrixIdentity(&mtxShadow);

	// シャドウマトリックスの作成
	D3DXMatrixShadow(&mtxShadow, &posLight, &plane);
	D3DXMatrixMultiply(&mtxShadow, &m_mtxWorld, &mtxShadow);

	//ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &mtxShadow);

	//現在のマテリアルを取得
	pDevice->GetMaterial(&matDef);

	// モデル情報を取得
	pFileData = pModelFile->SetAddress(m_nIdxModel);

	if (pFileData != NULL)
	{// 使用されている場合
		//マテリアルデータへのポインタを取得
		pMat = (D3DXMATERIAL*)pFileData->pBuffMat->GetBufferPointer();
		for (int nCntMat = 0; nCntMat < (int)pFileData->dwNumMat; nCntMat++)
		{
			D3DMATERIAL9 mat = pMat[nCntMat].MatD3D;
			mat.Ambient = { 0.0f, 0.0f, 0.0f, 0.4f };
			mat.Diffuse = { 0.0f, 0.0f, 0.0f, 0.4f };
			mat.Emissive = { 0.0f, 0.0f, 0.0f, 0.7f };
			mat.Specular = { 0.0f, 0.0f, 0.0f, 0.7f };
			pDevice->SetMaterial(&mat);

			//テクスチャの設定
			pDevice->SetTexture(0, nullptr);

			//モデル(パーツ)の描画
			pFileData->pMesh->DrawSubset(nCntMat);
		}
	}

	//保存していたマテリアルを戻す
	pDevice->SetMaterial(&matDef);

	m_fShadowHeight = GetPosition().y;
}
void CObjectX::DrawShadowonShader()
{
	// 影使用しない
	if (!m_bShadow) { return; }

	// モデル使用されていない
	CXFile* pModelFile = CManager::GetInstance()->GetModelFile();	// Xファイル情報のポインタ
	CXFile::SFileData* pFileData = pModelFile->SetAddress(m_nIdxModel);
	if (pFileData == nullptr) { return; }

	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();		//デバイスへのポインタを取得
	CFXManager* pFx = CFXManager::GetInstance();
	D3DMATERIAL9 matDef;					//現在のマテリアル保存用
	D3DXMATERIAL* pMat;						//マテリアルデータへのポインタ
	D3DXMATRIX mtxShadow;
	D3DLIGHT9 light;
	D3DXVECTOR4 posLight;
	D3DXVECTOR3 pos, normal;
	D3DXPLANE plane;

	// ライトの位置を設定
	pDevice->GetLight(0, &light);
	posLight = D3DXVECTOR4(-light.Direction.x, -light.Direction.y, -light.Direction.z, 0.0f);

	// 平面情報を設定
	D3DXVECTOR3 DefPos = D3DXVECTOR3(m_mtxWorld._41, m_mtxWorld._42, m_mtxWorld._43);
	pos = D3DXVECTOR3(m_mtxWorld._41, m_mtxWorld._42, m_mtxWorld._43);

	if (pos.y > DefPos.y) {	// 現在の腕の位置よりも高い
		pos = DefPos;
	}

	pos.y = m_fShadowHeight + 0.1f;
	normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	D3DXPlaneFromPointNormal(&plane, &pos, &normal);

	// シャドウマトリックスの初期化
	D3DXMatrixIdentity(&mtxShadow);

	// シャドウマトリックスの作成
	D3DXMatrixShadow(&mtxShadow, &posLight, &plane);
	D3DXMatrixMultiply(&mtxShadow, &m_mtxWorld, &mtxShadow);

	//ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &mtxShadow);
	pFx->SetMatrixWorld(mtxShadow);
	//現在のマテリアルを取得
	pDevice->GetMaterial(&matDef);

	// モデル情報を取得
	pFileData = pModelFile->SetAddress(m_nIdxModel);

	if (pFileData != NULL)
	{// 使用されている場合
		//マテリアルデータへのポインタを取得
		pMat = (D3DXMATERIAL*)pFileData->pBuffMat->GetBufferPointer();
		for (int nCntMat = 0; nCntMat < (int)pFileData->dwNumMat; nCntMat++)
		{
			D3DMATERIAL9 mat = pMat[nCntMat].MatD3D;
			mat.Ambient = { 0.0f, 0.0f, 0.0f, 0.7f };
			mat.Diffuse = { 0.0f, 0.0f, 0.0f, 0.7f };
			mat.Emissive = { 0.0f, 0.0f, 0.0f, 0.7f };
			mat.Specular = { 0.0f, 0.0f, 0.0f, 0.7f };
			pDevice->SetMaterial(&mat);
			pFx->SetMaterial(mat);
			pFx->SetParamToEffect();
			pFx->Begin();
			pFx->BeginPass();
			//テクスチャの設定
			pDevice->SetTexture(0, nullptr);
		
			//モデル(パーツ)の描画
			pFileData->pMesh->DrawSubset(nCntMat);
			pFx->EndPass();
			pFx->End();
		}
	}

	//保存していたマテリアルを戻す
	pDevice->SetMaterial(&matDef);

	m_fShadowHeight = GetPosition().y;
}
//==========================================================
// 生成
//==========================================================
CObjectX *CObjectX::Create(D3DXVECTOR3 pos,D3DXVECTOR3 rot, const char *pFileName, const int nPriority)
{
	CObjectX *pObjectX = NULL;
	CXFile *pModelFile = CManager::GetInstance()->GetModelFile();

	// オブジェクトXの生成
	pObjectX = DEBUG_NEW CObjectX(nPriority);

	if (pObjectX != NULL)
	{// 生成できた場合
		// 初期化処理
		pObjectX->Init();

		// 座標
		pObjectX->SetPosition(pos);

		// 向き
		pObjectX->SetRotation(rot);

		// 種類設定
		pObjectX->SetType(TYPE_MODEL);

		// 読み込み確認
		pObjectX->BindFile(pModelFile->Regist(pFileName));
	}
	else
	{// 生成に失敗した場合
		return NULL;
	}

	return pObjectX;
}

//==========================================================
// モデルのファイル設定
//==========================================================
void CObjectX::BindFile(int nIdx)
{
	m_nIdxModel = nIdx;	//使用するモデルの設定
}

//==========================================================
// モデルファイル読み込み
//==========================================================
void CObjectX::BindFile(const char* file)
{
	CXFile* pModelFile = CManager::GetInstance()->GetModelFile();
	m_nIdxModel = pModelFile->Regist(file);	//使用するモデルの設定
}

//==========================================================
// 座標の設定
//==========================================================
void CObjectX::SetPosition(const D3DXVECTOR3& pos)
{
	m_pos = pos; 
}

//==========================================================
// 向きの設定
//==========================================================
void CObjectX::SetRotation(const D3DXVECTOR3& rot)
{ 
	m_rot = rot;

	correction::Adjust(&m_rot);
}

//==========================================================
// スケールの設定
//==========================================================
void CObjectX::SetScale(const D3DXVECTOR3& scale)
{
	m_scale = scale;
}

//==========================================================
// 当たり判定
//==========================================================
void CObjectX::SetRotSize(D3DXVECTOR3 &SetMax, D3DXVECTOR3 &SetMin, D3DXVECTOR3 vtxMax, D3DXVECTOR3 vtxMin, float fRot)
{
	//すべて同じ値を先に入れる
	SetMax.y = vtxMax.y;	//Maxのy座標
	SetMin.y = vtxMin.y;	//Minのy座標

	//向きによって変更する
	if (fRot == D3DX_PI * 0.5f)
	{//左向き
		//最大値反映
		SetMax.x = vtxMax.z;	//x座標
		SetMax.z = -vtxMin.x;	//z座標

		//最小値反映
		SetMin.x = vtxMin.z;	//x座標
		SetMin.z = -vtxMax.x;	//z座標
	}
	else if (fRot == -D3DX_PI * 0.5f)
	{//右向き
		//最大値反映
		SetMax.x = -vtxMin.z;	//x座標
		SetMax.z = vtxMax.x;	//z座標

		//最小値反映
		SetMin.x = -vtxMax.z;	//x座標
		SetMin.z = vtxMin.x;	//z座標
	}
	else if(fRot == D3DX_PI || fRot == -D3DX_PI)
	{//反転している場合
		//高さ以外の最大値を最小値にする
		SetMax.x = -vtxMin.x;	//x座標
		SetMax.z = -vtxMin.z;	//z座標

		//高さ以外の最小値を最大値にする
		SetMin.x = -vtxMax.x;	//x座標
		SetMin.z = -vtxMax.z;	//z座標
	}
	else
	{//それ以外の場合
		//最大値反映
		SetMax.x = vtxMax.x;	//x座標
		SetMax.z = vtxMax.z;	//z座標

		//最小値反映
		SetMin.x = vtxMin.x;	//x座標
		SetMin.z = vtxMin.z;	//z座標
	}
}

//==========================================================
// リストから外す
//==========================================================
void CObjectX::ListOut(void)
{	
	m_List.Delete(this);
}

//==========================================================
// 最大値取得
//==========================================================
D3DXVECTOR3 CObjectX::GetVtxMax(void)
{
	CXFile* pModelFile = CManager::GetInstance()->GetModelFile();	// Xファイル情報のポインタ
	CXFile::SFileData* pFileData = pModelFile->SetAddress(m_nIdxModel);
	D3DXVECTOR3 max = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	if (pFileData == nullptr) { return max; }

	max.x = pFileData->vtxMax.x * m_scale.x;
	max.y = pFileData->vtxMax.y * m_scale.y;
	max.z = pFileData->vtxMax.z * m_scale.z;

	return max;
}

//==========================================================
// 最小値取得
//==========================================================
D3DXVECTOR3 CObjectX::GetVtxMin(void)
{
	CXFile* pModelFile = CManager::GetInstance()->GetModelFile();	// Xファイル情報のポインタ
	CXFile::SFileData* pFileData = pModelFile->SetAddress(m_nIdxModel);

	D3DXVECTOR3 min = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	if (pFileData == nullptr) { return min; }

	min.x = pFileData->vtxMin.x * m_scale.x;
	min.y = pFileData->vtxMin.y * m_scale.y;
	min.z = pFileData->vtxMin.z * m_scale.z;

	return min;
}

//==========================================================
// 描画確認
//==========================================================
void CObjectX::DrawCheck()
{
	CCamera* pCamera = CCameraManager::GetInstance()->GetTop();
	float maxlen = Game::DOME_LENGTH * 0.5f;
	float size = 0.0045f;

	if (!pCamera->GetAction()->IsFinish())
	{
		maxlen = Game::DOME_LENGTH * 0.75f;
	}

	if (CManager::GetInstance()->GetMode() != CScene::MODE::MODE_GAME)
	{
		maxlen *= 3.0f;
		size *= 30.0f;
	}

	if (CEditManager::GetInstance() != nullptr)
	{
		SetColMulti(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
		SetDraw();

		return;
	}

	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();
	D3DXMATRIX mtxProjection, mtxView, mtxWorld;
	D3DVIEWPORT9 Viewport;
	D3DXVECTOR3 pos = VECTOR3_ZERO;
	D3DXVECTOR3 mypos = GetPosition();

	// 必要な情報取得
	pDevice->GetTransform(D3DTS_PROJECTION, &mtxProjection);
	pDevice->GetTransform(D3DTS_VIEW, &mtxView);
	pDevice->GetViewport(&Viewport);

	//ワールドマトリックスの初期化
	D3DXMatrixIdentity(&mtxWorld);

	// スクリーン座標取得
	D3DXVec3Project(&pos, &mypos, &Viewport, &mtxProjection, &mtxView, &mtxWorld);

	float f = 0.5f * (m_scale.x + m_scale.z);

	// 画面外なら出さない
	if (pos.x < 0.0f - (SCREEN_WIDTH * size * f) || pos.x > SCREEN_WIDTH + (SCREEN_WIDTH * size * f) ||
		pos.y < 0.0f - (SCREEN_HEIGHT * size * m_scale.y) || pos.y > SCREEN_HEIGHT + (SCREEN_HEIGHT * size * m_scale.y) ||
		pos.z >= 1.0f) {

		// 色を透明に近づける
		D3DXCOLOR col = GetColMulti();
		if (col.a > 0.0f)
		{
			col.a -= 0.1f;

			if (col.a <= 0.0f)
			{
				col.a = 0.0f;
				SetDraw(false);
			}
		}

		SetColMulti(col);

		return;
	}

	// 距離を取る
	{
		D3DXVECTOR3 lenpos = pCamera->GetPositionR() - m_pos;
		if (D3DXVec3Length(&lenpos) >= maxlen)
		{
			// 色を透明に近づける
			D3DXCOLOR col = GetColMulti();

			if (col.a > 0.0f)
			{
				col.a -= 0.1f;

				if (col.a <= 0.0f)
				{
					col.a = 0.0f;
					SetDraw(false);
				}
			}

			SetColMulti(col);

			return;
		}
	}

	// 色を元に近づける
	D3DXCOLOR col = GetColMulti();
	if (col.a < 1.0f)
	{
		col.a += 0.1f;

		if (col.a >= 1.0f)
		{
			col.a = 1.0f;
		}
	}

	SetColMulti(col);
	SetDraw();
}
