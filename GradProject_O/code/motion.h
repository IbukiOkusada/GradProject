//===================================================
//
// 階層構造によるモーション処理 [motion.h]
// Author Ibuki Okusada
//
//===================================================
#ifndef _MOITON_H_
#define _MOITON_H_

#include "main.h"		// main.hで定義しているものが必要なためinclude

#define MAX_MOTION		(64)

// 前方宣言
class CModel;

//**********************************************************
// モーションクラスの定義
//**********************************************************
class CMotion
{
public: // 誰でもアクセス可能な定義

	// キー
	struct KEY
	{
		float fPosX;
		float fPosY;
		float fPosZ;
		float fRotX;
		float fRotY;
		float fRotZ;
	};

private:	// 自分だけがアクセス可能な定義

	// キー情報
	struct KEY_INFO
	{
		int nFrame;			// 再生フレーム
		KEY *aKey;			// パーツの総数
	};

	// モーション情報
	struct INFO
	{
		bool bLoop;			// ループするかどうか
		int nNumKey;			// キー数
		KEY_INFO *pKeyInfo;	// キーの総数
	};

	//胴体ファイル情報
	struct BodyFileData
	{
		CModel **ppParts;		// パーツの情報
		int nNumParts;			// パーツ数
	};

public:		// 誰でもアクセス可能
	CMotion();	// コンストラクタ
	~CMotion();	// デストラクタ

	// メンバ関数
	void Uninit(void);
	void Update(void);
	void BlendSet(int nType);
	void Set(int nType);
	void InitSet(int nType);
	void SetModel(CModel **pModel, int nNumParts);
	void LoadMotionData(FILE *pFile);
	int GetNowKey(void) { return m_nNowKey; }
	int GetNowMotion(void) { return m_nNowMotion; }
	int GetOldMotion(void) { return m_nOldType; }
	float GetNowFrame(void) { return m_fNowFrame; }
	int GetNowNumKey(void) { return m_aInfo[m_nNowMotion].nNumKey; }
	bool GetEnd(void) { return m_bEnd; }

private:	// 自分だけがアクセス可能

	// メンバ変数
	KEY m_OldKey[64];		// 前回のキー情報
	INFO m_aInfo[MAX_MOTION];	// モーション情報
	int m_nNumMotion;		// モーション数
	int m_nNowFrame;		// 現在のフレーム数
	float m_fNowFrame;	// 現在のフレーム数
	int m_nNowMotion;		// 現在のモーション番号
	bool m_bEnd;			// 終了したかどうか
	int m_nNowKey;		// 現在のキー数
	int m_nOldType;		// 前回のモーションタイプ
	BodyFileData m_FileData;// ファイル情報
};

#endif
