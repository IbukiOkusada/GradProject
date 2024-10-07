//==========================================================
//
// Xファイルモデルの処理全般 [objectX.cpp]
// Author : Ibuki Okusada (当たり判定調整 : Soma Ishihara)
//
//==========================================================
#include "main.h"
#include "objectX.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"
#include "Xfile.h"

// 静的メンバ変数宣言
CObjectX *CObjectX::m_pTop = NULL;	// 先頭のオブジェクトへのポインタ
CObjectX *CObjectX::m_pCur = NULL;	// 最後尾のオブジェクトへのポインタ

//==========================================================
// コンストラクタ
//==========================================================
CObjectX::CObjectX(int nPriority) : CObject(nPriority)
{
	m_nIdxModel = -1;

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

	m_bEnableCollision = true;
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
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	return S_OK;
}

//==========================================================
//モデルの終了処理
//==========================================================
void CObjectX::Uninit(void)
{
	// リストから外す
	ListOut();

	// 廃棄
	Release();
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
	//Update();

	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();		//デバイスへのポインタを取得
	CTexture *pTexture = CManager::GetInstance()->GetTexture();	// テクスチャへのポインタ
	CXFile *pModelFile = CManager::GetInstance()->GetModelFile();	// Xファイル情報のポインタ
	CXFile::FileData *pFileData = pModelFile->SetAddress(m_nIdxModel);
	D3DXMATRIX mtxRot, mtxTrans;			//計算用マトリックス
	D3DMATERIAL9 matDef;					//現在のマテリアル保存用
	D3DXMATERIAL *pMat;						//マテリアルデータへのポインタ

	//アルファテストを有効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 10);

	if (pFileData != NULL)
	{// モデルが使用されている場合
		//ワールドマトリックスの初期化
		D3DXMatrixIdentity(&m_mtxWorld);

		//向きを反映
		D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
		D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

		//位置を反映
		D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
		D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

		//ワールドマトリックスの設定
		pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

		//現在のマテリアルを取得
		pDevice->GetMaterial(&matDef);

		//マテリアルデータへのポインタを取得
		pMat = (D3DXMATERIAL*)pFileData->pBuffMat->GetBufferPointer();
		for (int nCntMat = 0; nCntMat < (int)pFileData->dwNumMat; nCntMat++)
		{
			D3DMATERIAL9 mat = pMat[nCntMat].MatD3D;

			//マテリアルの設定
			pDevice->SetMaterial(&mat);

			//テクスチャの設定
			pDevice->SetTexture(0, pTexture->SetAddress(pFileData->pIdexTexture[nCntMat]));

			//モデル(パーツ)の描画
			pFileData->pMesh->DrawSubset(nCntMat);
		}

		//保存していたマテリアルを戻す
		pDevice->SetMaterial(&matDef);
	}

	//アルファテストを無効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 255);
}

//==========================================================
// 生成
//==========================================================
CObjectX *CObjectX::Create(D3DXVECTOR3 pos,D3DXVECTOR3 rot, const char *pFileName, const int nPriority)
{
	CObjectX *pObjectX = NULL;
	CXFile *pModelFile = CManager::GetInstance()->GetModelFile();

	// オブジェクトXの生成
	pObjectX = new CObjectX(nPriority);

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
// 座標の設定
//==========================================================
void CObjectX::SetPosition(const D3DXVECTOR3 pos)
{
	m_pos = pos; 
}

//==========================================================
// 向きの設定
//==========================================================
void CObjectX::SetRotation(const D3DXVECTOR3 rot)
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
// 当たり判定
//==========================================================
bool CObjectX::Collision(D3DXVECTOR3 &pos, D3DXVECTOR3 &posOld, D3DXVECTOR3 &move, D3DXVECTOR3 vtxMin, D3DXVECTOR3 vtxMax, COLLISION_AXIS& Axis)
{
	CObjectX *pObj = m_pTop;	// 先頭取得
	bool bLand = false;	// 着地したか否か
	D3DXVECTOR3 moveOld = move;

	//仮置き
	D3DXVECTOR3 posTemp = posOld;

	//Y
	posTemp.y = pos.y;
	pObj = m_pTop;
	while (pObj != NULL)
	{
		CObjectX* pObjNext = pObj->m_pNext;
		if (pObj->m_bEnableCollision == true && pObj->CollisionCheck(posTemp, posOld, move, vtxMin, vtxMax, TYPE_Y))
		{
			bLand = true;
			Axis = TYPE_Y;
		}

		pObj = pObjNext;
	}

	//Z
	posTemp.z = pos.z;
	pObj = m_pTop;
	while (pObj != NULL)
	{
		CObjectX* pObjNext = pObj->m_pNext;
		if (pObj->m_bEnableCollision == true)
		{
			if (pObj->CollisionCheck(posTemp, posOld, move, vtxMin, vtxMax, TYPE_Z)) {
				Axis = TYPE_Z;
			}
		}

		pObj = pObjNext;
	}

	//X
	posTemp.x = pos.x;
	pObj = m_pTop;
	while (pObj != NULL)
	{
		CObjectX *pObjNext = pObj->m_pNext;
		if (pObj->m_bEnableCollision == true)
		{
			if (pObj->CollisionCheck(posTemp, posOld, move, vtxMin, vtxMax, TYPE_X)) {
				Axis = TYPE_X;
			}
		}

		pObj = pObjNext;
	}

	pos = posTemp;

	return bLand;
}

//==========================================================
// 当たり判定(触れたかどうか)
//==========================================================
bool CObjectX::Touch(D3DXVECTOR3& pos, D3DXVECTOR3& posOld, D3DXVECTOR3& move, D3DXVECTOR3 vtxMin, D3DXVECTOR3 vtxMax)
{
	CObjectX *pObj = m_pTop;	// 先頭取得
	bool bLand = false;	// 着地したか否か

	while (pObj != NULL)
	{
		CObjectX *pObjNext = pObj->m_pNext;
		if (pObj->m_bEnableCollision == true)
		{
			pObj->CollisionCheck(pos, posOld, move, vtxMin, vtxMax, TYPE_X);
			pObj->CollisionCheck(pos, posOld, move, vtxMin, vtxMax, TYPE_Z);
		}

		pObj = pObjNext;
	}

	return bLand;
}

//==========================================================
// 当たり判定(外積使用)
//==========================================================
bool CObjectX::CollisionCloss(D3DXVECTOR3& pos, D3DXVECTOR3& posOld, D3DXVECTOR3* posCollisioned)
{
	CObjectX* pObj = m_pTop;	// 先頭取得
	D3DXVECTOR3 posNear = D3DXVECTOR3(FLT_MAX, 0.0f, 0.0f);
	bool bCollision = false;

	while (pObj != NULL)
	{
		CObjectX* pObjNext = pObj->m_pNext;
		D3DXVECTOR3 posObjColl = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		if (pObj->CollisionCheckCloss(pos, posOld, &posObjColl))
		{
			float fLength = D3DXVec3Length(&(posOld - posObjColl));

			if (D3DXVec3Length(&(posOld - posNear)) > fLength)
			{
				posNear = posObjColl;
			}
			bCollision = true;
		}

		pObj = pObjNext;
	}

	if (posCollisioned != nullptr)
	{
		*posCollisioned = posNear;
	}

	return bCollision;
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
// 個別判定チェック
//==========================================================
bool CObjectX::CollisionCheck(D3DXVECTOR3 &pos, D3DXVECTOR3 &posOld, D3DXVECTOR3 &move, D3DXVECTOR3 vtxMin, D3DXVECTOR3 vtxMax, COLLISION_AXIS axis, const float fRefMulti)
{
	CXFile *pFile = CManager::GetInstance()->GetModelFile();
	bool bLand = false;	// 着地したか否か
	D3DXVECTOR3 vtxObjMax = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vtxObjMin = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	// 向きを反映
	int nIdx = GetIdx();
	SetRotSize(vtxObjMax,
		vtxObjMin,
		pFile->GetMax(nIdx),
		pFile->GetMin(nIdx),
		m_rot.y);

	
	switch (axis)
	{
	case CObjectX::TYPE_X:	//X
		if (pos.y + vtxMax.y > m_pos.y + vtxObjMin.y
			&& pos.y + vtxMin.y < m_pos.y + vtxObjMax.y
			&& pos.z + vtxMax.z > m_pos.z + vtxObjMin.z
			&& pos.z + vtxMin.z < m_pos.z + vtxObjMax.z)
		{//範囲内にある
			if (posOld.x + vtxMin.x >= m_pos.x + vtxObjMax.x
				&& pos.x + vtxMin.x < m_pos.x + vtxObjMax.x)
			{//右から左にめり込んだ
				move.x *= -0.0f;
				move.x *= fRefMulti;
				pos.x = m_pos.x + vtxObjMax.x - vtxMin.x + 0.1f + move.x;
			}
			else if (posOld.x + vtxMax.x <= m_pos.x + vtxObjMin.x
				&& pos.x + vtxMax.x > m_pos.x + vtxObjMin.x)
			{//左から右にめり込んだ
			 //位置を戻す
				move.x *= -0.0f;
				move.x *= fRefMulti;
				pos.x = m_pos.x + vtxObjMin.x - vtxMax.x - 0.1f + move.x;
			}
		}
		break;
	case CObjectX::TYPE_Y:	//Y
		if (pos.x + vtxMax.x > m_pos.x + vtxObjMin.x
			&& pos.x + vtxMin.x < m_pos.x + vtxObjMax.x
			&& pos.z + vtxMax.z > m_pos.z + vtxObjMin.z
			&& pos.z + vtxMin.z < m_pos.z + vtxObjMax.z)
		{//範囲内にある
			//上からの判定
			if (posOld.y + vtxMin.y >= m_pos.y + vtxObjMax.y
				&& pos.y + vtxMin.y < m_pos.y + vtxObjMax.y)
			{//上からめり込んだ
				//上にのせる
				pos.y = m_pos.y + vtxObjMax.y - vtxMin.y;
				move.y = 0.0f;
				bLand = true;
			}

			//下からの判定
			if (posOld.y + vtxMax.y <= m_pos.y + vtxObjMin.y
				&& pos.y + vtxMax.y > m_pos.y + vtxObjMin.y)
			{//下からめり込んだ
			 //その高さのまま
				pos.y = m_pos.y - vtxObjMin.y - vtxMax.y;
				move.y = 0.0f;
				bLand = true;
			}
		}
		break;
	case CObjectX::TYPE_Z:	//Z
		if (pos.x + vtxMax.x > m_pos.x + vtxObjMin.x
			&& pos.x + vtxMin.x < m_pos.x + vtxObjMax.x
			&& pos.y + vtxMax.y > m_pos.y + vtxObjMin.y
			&& pos.y + vtxMin.y < m_pos.y + vtxObjMax.y)
		{//範囲内にある
			if (posOld.z + vtxMin.z >= m_pos.z + vtxObjMax.z
				&& pos.z + vtxMin.z < m_pos.z + vtxObjMax.z)
			{//奥から手前にめり込んだ
			 //位置を戻す
				move.z *= -0.0f;
				move.z *= fRefMulti;
				pos.z = m_pos.z + vtxObjMax.z - vtxMin.z + 0.1f + move.z;
				//move.z = 0.0f;
			}
			else if (posOld.z + vtxMax.z <= m_pos.z + vtxObjMin.z
				&& pos.z + vtxMax.z > m_pos.z + vtxObjMin.z)
			{//手前から奥にめり込んだ
			 //位置を戻す
				move.z *= -0.0f;
				move.z *= fRefMulti;
				pos.z = m_pos.z + vtxObjMin.z - vtxMax.z - 0.1f + move.z;
				//move.z = 0.0f;
			}
		}
		break;
	default:
		break;
	}

	return bLand;
}

//==========================================================
// 個別判定チェック(外積使用)
//==========================================================
bool CObjectX::CollisionCheckCloss(D3DXVECTOR3& pos, D3DXVECTOR3& posOld, D3DXVECTOR3* posCollisioned)
{
	CXFile* pFile = CManager::GetInstance()->GetModelFile();
	D3DXVECTOR3 vtxObjMax = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vtxObjMin = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	bool bCollision = false;
	D3DXVECTOR3 posCulcNear = D3DXVECTOR3(FLT_MAX, 0.0f, 0.0f);

	// 向きを反映
	int nIdx = GetIdx();
	SetRotSize(vtxObjMax,
		vtxObjMin,
		pFile->GetMax(nIdx),
		pFile->GetMin(nIdx),
		m_rot.y);

	D3DXVECTOR3 posPoint[4] =
	{
		D3DXVECTOR3(m_pos.x + vtxObjMin.x,0.0f,m_pos.z + vtxObjMin.z),
		D3DXVECTOR3(m_pos.x + vtxObjMax.x,0.0f,m_pos.z + vtxObjMin.z),
		D3DXVECTOR3(m_pos.x + vtxObjMax.x,0.0f,m_pos.z + vtxObjMax.z),
		D3DXVECTOR3(m_pos.x + vtxObjMin.x,0.0f,m_pos.z + vtxObjMax.z)
	};

	D3DXVECTOR3 vecMove, vecLine;
	D3DXVECTOR3 vecToPos, vecToPosOld;
	float fAreaA = 1.0f, fAreaB = 1.1f;

	for (int cnt = 0; cnt < 4; cnt++)
	{
		vecMove = pos - posOld;
		vecLine = posPoint[(cnt + 1) % 4] - posPoint[cnt];	//境界線ベクトル
		vecToPos = pos - posPoint[cnt];
		vecToPos.y = 0.0f;
		vecToPosOld = posOld - posPoint[cnt];
		vecToPosOld.y = 0.0f;

		//面積求める
		fAreaA = (vecToPos.z * vecMove.x) - (vecToPos.x * vecMove.z);
		fAreaB = (vecLine.z * vecMove.x) - (vecLine.x * vecMove.z);

		if ((vecLine.z * vecToPosOld.x) - (vecLine.x * vecToPosOld.z) >= 0.0f && (vecLine.z * vecToPos.x) - (vecLine.x * vecToPos.z) < 0.0f)
		{
			if (fAreaA / fAreaB >= 0.0f && fAreaA / fAreaB <= 1.0f)
			{
				float fRate = fAreaA / fAreaB;
				D3DXVECTOR3 posXZ = posPoint[cnt];
				posXZ.x += vecLine.x * fRate;
				posXZ.y = posOld.y;
				posXZ.z += vecLine.z * fRate;
				float fTemp1 = D3DXVec3Length(&(D3DXVECTOR3(pos.x, posOld.y, pos.z) - posOld));
				float fTemp2 = D3DXVec3Length(&(D3DXVECTOR3(posXZ.x, posOld.y, posXZ.z) - posOld));
				float fRate2 = fTemp2 / fTemp1;
				D3DXVECTOR3 posCulc = posOld + (pos - posOld) * fRate2;

				if (posCulc.y >= m_pos.y + vtxObjMin.y && posCulc.y <= m_pos.y + vtxObjMax.y)
				{//ごっつん
					//衝突位置が欲しければあげる
					if (posCollisioned != nullptr)
					{//ほしいみたいなのであげる
						if (D3DXVec3Length(&(posCulc - posOld)) < D3DXVec3Length(&(posCulcNear - posOld)))
						{
							posCulcNear = posCulc;
						}
					}

					bCollision = true;
				}
			}
		}
	}

	if (bCollision == true)
	{
		*posCollisioned = posCulcNear;
	}

	return bCollision;
}

//==========================================================
// リストから外す
//==========================================================
void CObjectX::ListOut(void)
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
}

void CObjectX::CollisionLand(D3DXVECTOR3 &pos)
{
	CObjectX *pObj = m_pTop;	// 先頭取得
	CXFile *pFile = CManager::GetInstance()->GetModelFile();

	while (pObj != NULL)
	{
		CObjectX *pObjNext = pObj->m_pNext;
		D3DXVECTOR3 vtxObjMax = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3 vtxObjMin = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

		// 向きを反映
		int nIdx = pObj->GetIdx();
		pObj->SetRotSize(vtxObjMax,
			vtxObjMin,
			pFile->GetMax(nIdx),
			pFile->GetMin(nIdx),
			pObj->m_rot.y);

		if (pos.x >= pObj->m_pos.x + vtxObjMin.x
			&& pos.x <= pObj->m_pos.x + vtxObjMax.x
			&& pos.z >= pObj->m_pos.z + vtxObjMin.z
			&& pos.z <= pObj->m_pos.z + vtxObjMax.z)
		{//範囲内にある
			//上からの判定
			if (pos.y >= pObj->m_pos.y + vtxObjMax.y)
			{//上からめり込んだ
				//上にのせる
				pos.y = pObj->m_pos.y + vtxObjMax.y + 4.0f;
			}
		}

		pObj = pObjNext;
	}
}

//==========================================================
// 当たり判定
//==========================================================
D3DXVECTOR3 CObjectX::Collision(D3DXVECTOR3& pos, D3DXVECTOR3& posOld, D3DXVECTOR3& move, D3DXVECTOR3 vtxMin, D3DXVECTOR3 vtxMax, D3DXVECTOR3 vtxMinOld, D3DXVECTOR3 vtxMaxOld, COLLISION_AXIS& Axis)
{
	CObjectX *pObj = m_pTop;	// 先頭取得
	bool bLand = false;	// 着地したか否か
	D3DXVECTOR3 moveOld = move;
	D3DXVECTOR3 nor = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	//仮置き
	D3DXVECTOR3 posTemp = posOld;

	//Y
	posTemp.y = pos.y;
	pObj = m_pTop;
	while (pObj != NULL)
	{
		CObjectX* pObjNext = pObj->m_pNext;
		if (pObj->m_bEnableCollision == true && pObj->CollisionCheck(posTemp, posOld, move, vtxMin, vtxMax, vtxMinOld, vtxMaxOld, TYPE_Y))
		{
			bLand = true;
			Axis = TYPE_Y;
			nor.y = posTemp.y - pObj->GetPosition().y;
		}

		pObj = pObjNext;
	}

	//Z
	posTemp.z = pos.z;
	pObj = m_pTop;
	while (pObj != NULL)
	{
		CObjectX* pObjNext = pObj->m_pNext;
		if (pObj->m_bEnableCollision == true && pObj->CollisionCheck(posTemp, posOld, move, vtxMin, vtxMax, vtxMinOld, vtxMaxOld, TYPE_Z))
		{
			Axis = TYPE_Z;
			nor.z = posTemp.z - pObj->GetPosition().z;
		}

		pObj = pObjNext;
	}

	//X
	posTemp.x = pos.x;
	pObj = m_pTop;
	while (pObj != NULL)
	{
		CObjectX *pObjNext = pObj->m_pNext;
		if (pObj->m_bEnableCollision == true && pObj->CollisionCheck(posTemp, posOld, move, vtxMin, vtxMax, vtxMinOld, vtxMaxOld, TYPE_X))
		{
			bLand = true;
			Axis = TYPE_X;
			nor.x = posTemp.x - pObj->GetPosition().x;
		}

		pObj = pObjNext;
	}

	pos = posTemp;
	D3DXVec3Normalize(&nor, &nor);

	return nor;
}

//==========================================================
// 当たり判定確認
//==========================================================
bool CObjectX::CollisionCheck(D3DXVECTOR3& pos, D3DXVECTOR3& posOld, D3DXVECTOR3& move, D3DXVECTOR3 vtxMin, D3DXVECTOR3 vtxMax, D3DXVECTOR3 vtxMinOld, D3DXVECTOR3 vtxMaxOld, COLLISION_AXIS axis)
{
	CXFile *pFile = CManager::GetInstance()->GetModelFile();
	bool bLand = false;	// 着地したか否か
	D3DXVECTOR3 vtxObjMax = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vtxObjMin = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	// 向きを反映
	int nIdx = GetIdx();
	SetRotSize(vtxObjMax,
		vtxObjMin,
		pFile->GetMax(nIdx),
		pFile->GetMin(nIdx),
		m_rot.y);


	switch (axis)
	{
	case CObjectX::TYPE_X:	//X
		if (pos.y + vtxMax.y > m_pos.y + vtxObjMin.y
			&& pos.y + vtxMin.y < m_pos.y + vtxObjMax.y
			&& pos.z + vtxMax.z > m_pos.z + vtxObjMin.z
			&& pos.z + vtxMin.z < m_pos.z + vtxObjMax.z)
		{//範囲内にある
			if (posOld.x + vtxMinOld.x >= m_pos.x + vtxObjMax.x
				&& pos.x + vtxMin.x < m_pos.x + vtxObjMax.x)
			{//右から左にめり込んだ
				move.x *= -0.0f;
				pos.x = m_pos.x + vtxObjMax.x - vtxMin.x + move.x;
				bLand = true;
			}
			else if (posOld.x + vtxMaxOld.x <= m_pos.x + vtxObjMin.x
				&& pos.x + vtxMax.x > m_pos.x + vtxObjMin.x)
			{//左から右にめり込んだ
				move.x *= -0.0f;
				pos.x = m_pos.x + vtxObjMin.x - vtxMax.x + move.x;
				bLand = true;
			}
		}
		break;
	case CObjectX::TYPE_Y:	//Y
		if (pos.x + vtxMax.x > m_pos.x + vtxObjMin.x
			&& pos.x + vtxMin.x < m_pos.x + vtxObjMax.x
			&& pos.z + vtxMax.z > m_pos.z + vtxObjMin.z
			&& pos.z + vtxMin.z < m_pos.z + vtxObjMax.z)
		{//範囲内にある
		 //上からの判定
			if (posOld.y + vtxMinOld.y >= m_pos.y + vtxObjMax.y
				&& pos.y + vtxMin.y < m_pos.y + vtxObjMax.y)
			{//上からめり込んだ
				pos.y = m_pos.y + vtxObjMax.y - vtxMin.y;
				move.y = 0.0f;
				bLand = true;
			}

			//下からの判定
			if (posOld.y + vtxMaxOld.y <= m_pos.y + vtxObjMin.y
				&& pos.y + vtxMax.y > m_pos.y + vtxObjMin.y)
			{//下からめり込んだ
				pos.y = m_pos.y - vtxObjMin.y - vtxMax.y;
				move.y = 0.0f;
				bLand = true;
			}
		}
		break;
	case CObjectX::TYPE_Z:	//Z
		if (pos.x + vtxMax.x > m_pos.x + vtxObjMin.x
			&& pos.x + vtxMin.x < m_pos.x + vtxObjMax.x
			&& pos.y + vtxMax.y > m_pos.y + vtxObjMin.y
			&& pos.y + vtxMin.y < m_pos.y + vtxObjMax.y)
		{//範囲内にある
			if (posOld.z + vtxMinOld.z >= m_pos.z + vtxObjMax.z
				&& pos.z + vtxMin.z < m_pos.z + vtxObjMax.z)
			{//奥から手前にめり込んだ
			 //位置を戻す
				move.z *= -0.0f;
				pos.z = m_pos.z + vtxObjMax.z - vtxMin.z + move.z;
				bLand = true;
			}
			else if (posOld.z + vtxMaxOld.z <= m_pos.z + vtxObjMin.z
				&& pos.z + vtxMax.z > m_pos.z + vtxObjMin.z)
			{//手前から奥にめり込んだ
			 //位置を戻す
				move.z *= -0.0f;
				pos.z = m_pos.z + vtxObjMin.z - vtxMax.z + move.z;
				bLand = true;
			}
		}
		break;
	default:
		break;
	}

	return bLand;
}