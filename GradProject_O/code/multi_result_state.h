//===============================================
//
// マルチリザルト画面の状態管理処理 [multi_result_state.h]
// Author : Ibuki Okusada
//
//===============================================
#ifndef _MULTI_RESULT_STATE_H_		// このマクロが定義されていない場合
#define _MULTI_RESULT_STATE_H_		// 二重インクルード防止用マクロを定義

// 前方宣言
class CMultiResult;
class CPolice;

//===============================================
// マルチリザルトの状態クラス定義(基底クラス)
//===============================================
class CMultiResultState
{
public:

	// 状態列挙
	enum STATE
	{
		STATE_START = 0,	// 開始
		STATE_RANK,			// ランク表示
		STATE_MULTIRANK,		// 結果表示
		STATE_END,
		STATE_MAX
	};

public:
	
	// コンストラクタ
	CMultiResultState(STATE _state) : m_bCreate(false), m_state(_state) {}

	// デストラクタ
	virtual ~CMultiResultState(){}

	// メンバ関数
	void Update(CMultiResult* pResult) 
	{
		if (pResult == nullptr) { return; }

		// 生成
		if (!m_bCreate) {
			Create(pResult);
			m_bCreate = true;
		}

		// 処理
		Controll(pResult);
	}

protected:
	
	//===============================================
	// メンバ関数
	//===============================================
	// 必要処理
	virtual void Controll(CMultiResult* pResult) = 0;
	// 生成
	virtual void Create(CMultiResult* pResult){}

private:

	// メンバ変数
	bool m_bCreate;	// 一度生成したか否か
	STATE m_state;	// 状態
};

//*************************************************************************************
// 開始状態クラス(派生クラス)
//===============================================
class CMultiResultStateStart : public CMultiResultState
{
public:

	// コンストラクタ
	CMultiResultStateStart();

	// デストラクタ
	~CMultiResultStateStart() override;

private:

	// メンバ関数
	void Controll(CMultiResult* pResult) override;
};

//===============================================
// ランク表示状態クラス(派生クラス)
//===============================================
class CMultiResultStateRank : public CMultiResultState
{
public:

	// コンストラクタ
	CMultiResultStateRank();

	// デストラクタ
	~CMultiResultStateRank() override;

private:

	// メンバ関数
	void Controll(CMultiResult* pResult) override;
	void RankPlayerMove(CMultiResult* pResult);
};

//===============================================
// 結果表示状態クラス(派生クラス)
//===============================================
class CMultiResultStateResult : public CMultiResultState
{
public:

	// コンストラクタ
	CMultiResultStateResult();

	// デストラクタ
	~CMultiResultStateResult() override;

private:

	// メンバ関数
	void Controll(CMultiResult* pResult) override;
	void Create(CMultiResult* pResult) override;
	void Uninit(CMultiResult* pResult);
	void PoliceUpdate(CMultiResult* pResult);

	// メンバ変数
	CPolice** m_ppPolice;	// 警察の生成数
	int m_nNumCreatePolice;	// 警察の生成数
};

//===============================================
// 終了可能状態クラス(派生クラス)
//===============================================
class CMultiResultStateEnd : public CMultiResultState
{
public:

	// コンストラクタ
	CMultiResultStateEnd();

	// デストラクタ
	~CMultiResultStateEnd() override;

private:

	// メンバ関数
	void Controll(CMultiResult* pResult) override;

	// メンバ変数
	float m_fFadeCnt;	// フェードまでのカウント
};

#endif
