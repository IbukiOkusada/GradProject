//===============================================
//
// タイトル画面の管理処理 [title.cpp]
// Author : Ibuki Okusada
//
//===============================================
//改変者
//Kazuki Watanabe
//<==============================================
#include "title.h"
#include "manager.h"
#include "input.h"
#include "fade.h"
#include "sound.h"
#include "camera.h"
#include "texture.h"
#include "input_gamepad.h"
#include "input_keyboard.h"
#include "object2D.h"
#include "map_manager.h"
#include "meshfield.h"
#include "PlayerTitle.h"
#include "PoliceTitle.h"
#include "goal.h"
#include "camera_manager.h"
#include "number.h"
#include "TitleBaggage.h"
#include "debugproc.h"
#include "objectsound.h"

//静的メンバ変数
CPoliceTitle* CTitle::m_apPolice[INITIAL::POLICE_MAX] = {nullptr};

//<===============================================
//無名名前空間
//<===============================================
namespace 
{
	//<************************************************
	//D3DXVECTOR3型
	//<************************************************ 
	const D3DXVECTOR3 PRESSENTER_POS = { SCREEN_WIDTH, SCREEN_HEIGHT * 0.9f, 0.0f };			//プレスエンターの座標位置
	const D3DXVECTOR3 TITLELOGO_POS = { SCREEN_WIDTH, SCREEN_HEIGHT * 0.23f, 0.0f };			//タイトルロゴの座標位置
	const D3DXVECTOR3 TEAMLOGO_POS = { SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f ,0.0f };		//チームロゴの座標位置
	const D3DXVECTOR3 PolicePos = { 2530.0f, 50.0f, -800.0f };									//警察位置
	const D3DXVECTOR3 DEST_ROT = { 0.0f,-3.14f,0.0f };											//目的向き

	constexpr int nNatPriority = 5;																//共通して使う優先度変数
	const D3DXCOLOR INV_COL = { 0.0f,0.0f,0.0f,0.0f };											//透明の時の色
	const D3DXCOLOR VIS_COL = { 0.0f,0.0f,0.0f,1.0f };											//非透明の時の色


	//<************************************************
	//float型
	//<************************************************ 
	constexpr float MAX_ALPHA = 1.0f;															//透明度の最大値
	constexpr float MIN_ALPHA = 0.3f;															//透明度の最小値
	constexpr float ALPHA_ZERO = 0.0f;															//透明の時のα値
	constexpr float TITLELOGO_DEST = SCREEN_WIDTH * 0.5f;										//タイトルロゴの目標位置

	const D3DXCOLOR ZERO_COL = D3DXCOLOR(1.0f, 1.0f, 1.0f, ALPHA_ZERO);							//透明色の値
	const D3DXCOLOR ONE_COL = D3DXCOLOR(1.0f, 1.0f, 1.0f, MAX_ALPHA);							//非透明の値
	
}
//<===============================================
//コンストラクタ
//<===============================================
CTitle::CTitle()
{
	//int関係
	m_nCounter = 0;
	m_nCounterRanking = 0;
	m_nLogoAlpgha = 0;
	m_nLogoCou = 0;

	//選択肢関連
	m_nSelect = SELECT::SELECT_SINGLE;
	m_nSelectYN = SELECT_YN::SELECT_YN_YES;
	m_eState = STATE::STATE_TEAMLOGO;

	m_fDis = 0.0f;

	//bool系
	m_bPush = false;
	m_bNext = false;
	m_bSkipped = false;
	m_bCol = false;
	m_bIniting = false;
	m_bSizing = false;
	m_bSelected = false;

	//ポインタ系
	m_pPlayer = nullptr;
	m_pCam = nullptr;

	//配列の初期化
	for (int nCnt = 0; nCnt < OBJ2D_MAX; nCnt++)
	{
		m_pObject2D[nCnt] = nullptr;
	}

	//配列の初期化
	for (int nCnt = 0; nCnt < SELECT::SELECT_MAX; nCnt++)
	{
		m_apSelect[nCnt] = nullptr;
	}

	//配列の初期化
	for (int nCnt = 0; nCnt < SELECT_YN::SELECT_YN_MAX; nCnt++)
	{
		m_apYesNoObj[nCnt] = nullptr;
	}

	//配列の初期化
	for (int nCnt = 0; nCnt < INITIAL::POLICE_MAX; nCnt++)
	{
		m_apPolice[nCnt] = nullptr;
	}

}

//<===============================================
//デストラクタ
//<===============================================
CTitle::~CTitle()
{

}

//<===============================================
//初期化処理
//<===============================================
HRESULT CTitle::Init(void)
{
	constexpr char* TEX_TEAMLOGO = "data\\TEXTURE\\team_logo.png";				//チームロゴ
	const D3DXVECTOR3 CAMERA_POS = { 3350.0f, 95.0f, 260.0f };					//カメラの初期位置
	constexpr int MOVE_LOGO = 135;												//次のステートに遷移するまでの時間
	constexpr int AUTOMOVE_RANKING = 10000;										//ランキング自動遷移時間
	constexpr float fSize[SIZING::SIZING_MAX] = { 250.0f, 125.0f };				//サイズ(チームロゴ)

	//int型変数の設定
	m_nCounterRanking = AUTOMOVE_RANKING;
	m_nCounter = MOVE_LOGO;

	//チームロゴ
	m_pObject2D[OBJ2D::OBJ2D_TeamLogo] = InitObj2D(TEAMLOGO_POS, 
		VECTOR3_ZERO,3, fSize[SIZING_WIDTH], fSize[SIZING_HEIGHT],true, TEX_TEAMLOGO);

	//カメラ初期状態
	m_pCam = CCameraManager::GetInstance()->GetTop();
	m_pCam->SetPositionR(CAMERA_POS);
	m_pCam->SetLength(100.0f);
	m_pCam->SetRotation(D3DXVECTOR3(0.0f, -0.0f, 1.79f));
	m_pCam->SetActive(true);

	return S_OK;
}

//<===============================================
//終了処理
//<===============================================
void CTitle::Uninit(void)
{
	CManager::GetInstance()->GetSound()->Stop();

	//カメラの視点の位置を初期化
	m_pCam->SetPositionR(VECTOR3_ZERO);

	//オブジェクト破棄
	SAFE_UNINIT(m_pPlayer);

	// マップ情報廃棄
	CMapManager::Release();
}
//<===============================================
//更新処理
//<===============================================
void CTitle::Update(void)
{
	//ステートごとに処理分け
	switch (m_eState)
	{
		//チームロゴステートだった場合
	case STATE::STATE_TEAMLOGO:

		//次への合図がなければ
		if (!m_bNext)
		{
			//キーボード入力かパッド入力があれば
			if (TriggerEnter()) {m_bPush = true;}

			//押された場合
			if (m_bPush)
			{
				m_bNext = true;
			}
			//押されていなかったら
			else
			{
				m_nCounter--;
				if (m_nCounter <= 0)
				{
					m_bNext = true;
					m_nCounter = 0;
				}
			}
		}
		//次に行く準備ができていたら
		else { StateLogo(); }

		break;

		//プレスエンターステートだった場合
	case STATE::STATE_PRE:

		StatePre();

		break;

		//プレスエンターステートだった場合
	case STATE::STATE_PRESSENTER:

		DebugCam();

		MoveP_E();

		break;

		//プレスエンターを押した後のステートだった場合
	case STATE::STATE_CHASING:

		ChaseMovement();

		break;

		//アイスを投げ入れるステートだった場合
	case STATE::STATE_ICETHROW:

		DebugCam();
		IceMovement();

		break;

		//それ以外だった場合
	default:

		break;
		
	}

	CScene::Update();
}

//<===============================================
//描画処理
//<===============================================
void CTitle::Draw(void)
{
	CScene::Draw();
}
//<===============================================
//チームロゴステートでの動き
//<===============================================
void CTitle::StateLogo(void)
{
	D3DXCOLOR TeamCol = m_pObject2D[OBJ2D::OBJ2D_TeamLogo]->GetCol();	// チームロゴの色情報を取得
	constexpr int nCountMax = 10;											// カウンターの固定値
	constexpr int nAmoValue = 20;											//色変更時の値

	//ステートがチームロゴだった場合
	if (m_eState == STATE::STATE_TEAMLOGO)
	{
		if(!m_bCol)
		{
			//透明に近づけていく
			TeamCol.a -= MAX_ALPHA / nAmoValue;

			//完全に透明になった場合
			if (TeamCol.a <= ALPHA_ZERO)
			{
				//透明度を透明に
				TeamCol.a = ALPHA_ZERO;

				//超えていたらカウントの初期化と透明終了合図を送る
				if (m_nCounter >= nCountMax)
				{
					//次のステートに移行する
					m_eState = STATE::STATE_PRE;
					m_nCounter = 0;
				}
				//超えていなかったらカウント増加
				else{m_nCounter++;}
				
			}
		}
		m_pObject2D[OBJ2D::OBJ2D_TeamLogo]->SetCol(TeamCol);
	}

}
//<===============================================
//プレスエンターステートでの動き
//<===============================================
void CTitle::StateP_E(void)
{
	// カウンターの固定値
	constexpr int nCountMax = 10;	

	//その場所についているかつプレスエンターの文字が表示されていなければ
	if (m_pPlayer->GetReached()&&!m_pObject2D[OBJ2D::OBJ2D_PressEnter]->GetDraw())
	{
		//超えていたら
		if (m_nCounter >= nCountMax)
		{
			MovingLogo();
		}
		//超えていなかったらカウント増加
		else{m_nCounter++;}

	}
	//その場所についていないかつプレスエンターの文字が表示されていなければ一番目の目的地にプレイヤーを移動させる
	else if (!m_pPlayer->GetReached() && !m_pObject2D[OBJ2D::OBJ2D_PressEnter]->GetDraw())
	{m_pPlayer->Moving(CPlayerTitle::DEST_FIRST);}

	//プレスエンターの文字が表示されていたらステートを変更
	else if (m_pObject2D[OBJ2D::OBJ2D_PressEnter]->GetDraw())
	{
		m_eState = STATE::STATE_PRESSENTER;

		//左に向くようにする
		m_pPlayer->SetRotation(VECTOR3_ZERO);
	}
}
//<===============================================
//プレスエンターでの動き
//<===============================================
void CTitle::MoveP_E(void)
{
	//初期化
	if (m_bIniting) { m_bIniting = false; }

	PreMove();
	//キーボード入力かパッド入力があれば
	if (TriggerEnter()){m_bPush = true;}

	//反応があったら
	if (m_bPush)
	{
		//必要ないので終了処理を挟む(いるかは不明だけど念のためです)
		SAFE_UNINIT(m_pObject2D[OBJ2D::OBJ2D_TITLELOGO]);
		SAFE_UNINIT(m_pObject2D[OBJ2D::OBJ2D_PressEnter]);

		//次のステートに移行する、ブラックカバーで隠す
		m_pObject2D[OBJ2D::OBJ2D_BLACKCOVER]->SetCol(D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f));
		m_eState = STATE::STATE_CHASING;
	}
	else { return; }
}
//<===============================================
//黒カバーの動き
//<===============================================
void CTitle::BlackCoverM(void)
{
	D3DXCOLOR BCoverCol = m_pObject2D[OBJ2D::OBJ2D_BLACKCOVER]->GetCol();	// チームロゴの色情報を取得
	constexpr int nAmoValue = 20;												//色変更時の値
	BCoverCol.a -= MAX_ALPHA / nAmoValue;									//透明に近づけていく

	if (BCoverCol.a <= 0.0f){BCoverCol.a = ALPHA_ZERO;}

	m_pObject2D[OBJ2D::OBJ2D_BLACKCOVER]->SetCol(BCoverCol);
}
//<===============================================
//タイトルロゴの動き
//<===============================================
void CTitle::MovingLogo(void)
{
	constexpr float fSpeed = 0.09f;													//タイトルロゴが動くスピード
	D3DXVECTOR3 TitlePos = m_pObject2D[OBJ2D::OBJ2D_TITLELOGO]->GetPosition();	//タイトルロゴの位置
	
	//描画設定をオンにし、移動用の変数にタイトルロゴの位置を取得させる
	m_pObject2D[OBJ2D::OBJ2D_TITLELOGO]->SetDraw(true);

	//目的地まで移動させる
	TitlePos.x += (TITLELOGO_DEST - TitlePos.x - 0.0f) * fSpeed;//X軸

	//目的地に着いたら
	if (Function::BoolToDest(m_pObject2D[OBJ2D::OBJ2D_TITLELOGO]->GetPosition(),
		D3DXVECTOR3(TITLELOGO_DEST, 0.0f, 0.0f), 5.0f, false))
	{
		//オンになっていなければ描画状態をオンにする
		if (!m_pObject2D[OBJ2D::OBJ2D_PressEnter]->GetDraw())
		{
			m_pObject2D[OBJ2D::OBJ2D_PressEnter]->SetDraw(true);
		}

		//カウント初期化
		m_nCounter = 0;
	}

	//タイトルロゴの設定
	m_pObject2D[OBJ2D::OBJ2D_TITLELOGO]->SetPosition(TitlePos);
	m_pObject2D[OBJ2D::OBJ2D_TITLELOGO]->SetVtx();
}
//<===============================================
//仮名ステートでの動き
//<===============================================
void CTitle::PreMove(void)
{
	D3DXVECTOR3 P_EPos = m_pObject2D[OBJ2D::OBJ2D_PressEnter]->GetPosition();	//プレスエンターの位置

	float PlayerRot = m_pPlayer->GetRotation().y;								//プレイヤーの向きを変える
	constexpr float fRotMove = 0.05f;											//向きの移動値
	constexpr float fSpeed = 0.09f;												//速度

	//目的地まで移動させる
	P_EPos.x += (TITLELOGO_DEST - P_EPos.x - 0.0f) * fSpeed;//X軸

	//目的地に着いたら
	if (Function::BoolToDest(m_pObject2D[OBJ2D::OBJ2D_PressEnter]->GetPosition(),
		D3DXVECTOR3(TITLELOGO_DEST, 0.0f, 0.0f), 3.0f, false))
	{
		//プレスエンターとタイトルロゴに関する処理
		ColChange(m_pObject2D[OBJ2D_PressEnter],20);
		LightOff();

		//反応がなかったら
		if (!m_bPush)
		{
			//ランキング画面に移行
			if (m_nCounterRanking <= 0) { CManager::GetInstance()->GetFade()->Set(CScene::MODE_RESULT); }
			else { m_nCounterRanking--; }
		}
	}

	//一番目の目的地にプレイヤーを移動させる
	m_pPlayer->Moving(CPlayerTitle::DEST_SECOND);

	//その場所についているかつプレスエンターの文字が表示されていなければ
	if (m_pPlayer->GetReached())
	{
		//
		if (PlayerRot <= DEST_ROT.y) { PlayerRot = DEST_ROT.y; }
		else { PlayerRot += -fRotMove; }

		//プレイヤーの向きに反映
		m_pPlayer->SetRotation(D3DXVECTOR3(0.0f, PlayerRot, 0.0f));

		//サウンド再生
		m_pPlayer->SetS(true);
	}
	//
	m_pObject2D[OBJ2D::OBJ2D_PressEnter]->SetPosition(P_EPos);
	m_pObject2D[OBJ2D::OBJ2D_PressEnter]->SetVtx();
}
//<===============================================
//仮名ステートでの動き
//<===============================================
void CTitle::StatePre(void)
{
	//初期化
	InitingP_E();

	//透明度が透明になっていなかったら
	if (!(m_pObject2D[OBJ2D_BLACKCOVER]->GetCol().a <= ALPHA_ZERO)){BlackCoverM();}

	//透明度が透明になっていたら
	else
	{
		//キーボード入力かパッド入力があれば、スキップ情報をtrueにする
		if (CInputKeyboard::GetInstance()->GetTrigger(DIK_RETURN) ||
			CInputPad::GetInstance()->GetTrigger(CInputPad::BUTTON_START, 0) ||
			CInputPad::GetInstance()->GetTrigger(CInputPad::BUTTON_A, 0)) {m_bSkipped = true;}

		//スキップされていなければ、スキップしていない際の動きを実行
		if (!m_bSkipped) { StateP_E(); }

		//されていたらスキップした際の動きの処理を実行
		else { SkipMovement(); }
	}
}
//<===============================================
//プレスエンターステートでの動き
//<===============================================
void CTitle::InitingP_E(void)
{
	//初期化済みであれば抜ける
	if (m_bIniting) {return;}

	const D3DXVECTOR3 PLAYER_POS = { 2630.0f, 50.0f, -1988.0f };							//プレイヤーの位置
	constexpr float fLogoLength = 150.0f;													//ロゴの長さ(サイズ)
	constexpr float fP_ELength = (350.0f, 350.0f);											//プレスエンターの長さ(サイズ)

	constexpr char* TEX_TITLELOGO = "data\\TEXTURE\\Title\\Title_logo.png";					//タイトルロゴのテクスチャネーム
	constexpr char* TEX_PRESSENTER = "data\\TEXTURE\\Title\\-PRESS ENTER-.png";				//プレスエンターのテクスチャネーム

	constexpr int nMaxPri = 7;																//優先度最大値

	//サイズ関連
	constexpr float fSizeBlack[SIZING_MAX] = { SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f };	//サイズ(ブラックカバー)
	constexpr float fSizeTitleLogo[SIZING_MAX] = { 650.0f,400.0f };							//サイズ(タイトルロゴ)
	constexpr float fSizePressEnter[SIZING_MAX] = { 550.0f,80.0f };						//サイズ(プレスエンター)

	//2つのbool情報を初期化
	m_bIniting = true;
	m_bPush = false;
	m_bNext = false;

	//必要ないので終了処理を挟む(いるかは不明だけど念のためです)
	SAFE_UNINIT(m_pObject2D[OBJ2D::OBJ2D_TeamLogo]);
	//<******************************************
	// 2Dオブジェクトの生成処理
	//<****************************************** 
	//ブラックカバー
	m_pObject2D[OBJ2D::OBJ2D_BLACKCOVER] = InitObj2D(TEAMLOGO_POS, VECTOR3_ZERO, nMaxPri,
		fSizeBlack[SIZING_WIDTH], fSizeBlack[SIZING_HEIGHT], true, nullptr, VIS_COL);

	//ブラックカバー
	m_pObject2D[OBJ2D::OBJ2D_TITLELOGO] = InitObj2D(TITLELOGO_POS, VECTOR3_ZERO, nNatPriority,
		fSizeTitleLogo[SIZING_WIDTH], fSizeTitleLogo[SIZING_HEIGHT], false, TEX_TITLELOGO);

	//ブラックカバー
	m_pObject2D[OBJ2D::OBJ2D_PressEnter] = InitObj2D(PRESSENTER_POS, VECTOR3_ZERO, nNatPriority,
		fSizePressEnter[SIZING_WIDTH], fSizePressEnter[SIZING_HEIGHT], false, TEX_PRESSENTER);
	m_pObject2D[OBJ2D::OBJ2D_PressEnter]->SetVtx();

	//必要なオブジェクトの生成
	CMapManager::GetInstance()->Load();
	m_pPlayer = CPlayerTitle::Create(PLAYER_POS, DEST_ROT, VECTOR3_ZERO,nullptr,nullptr);

	// 右側
	CMeshField::Create(D3DXVECTOR3(27250.0f, -10.0f, 3000.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), 1000.0f, 1000.0f, "data\\TEXTURE\\field000.jpg", 13, 16);

	// 左側
	CMeshField::Create(D3DXVECTOR3(-750.0f, -10.0f, 3000.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), 500.0f, 500.0f, "data\\TEXTURE\\field001.jpg", 26, 32);

	//警察の生成
	for (int nCnt = 0; nCnt < INITIAL::POLICE_MAX; nCnt++)
	{
		m_apPolice[nCnt] = CPoliceTitle::Create(
			D3DXVECTOR3(PolicePos.x + 150.0f *nCnt, PolicePos.y, PolicePos.z),
			DEST_ROT, VECTOR3_ZERO);
	}

	//タイトルBGM再生
	CManager::GetInstance()->GetSound()->Play(CSound::LABEL_BGM_TITLE);
}
//<===============================================
//色変更処理
//<===============================================
void CTitle::ColChange(CObject2D* pObj2D,const int nCntMax)
{
	D3DXCOLOR TitleLogoCol = pObj2D->GetCol();	//そのオブジェクトの色情報を取得
	constexpr int nAmoValue = 10;				//色変化値

	//色変更完了していなければ
	if (!m_bCol) 
	{
		//透明に近づけていく
		TitleLogoCol.a += MAX_ALPHA / -nAmoValue;

		//その値まで行ったら
		if (TitleLogoCol.a <= MIN_ALPHA)
		{
			TitleLogoCol.a = MIN_ALPHA;	//透明度をその値にする

			//超えていたらカウントの初期化と透明終了合図を送る
			if (m_nLogoAlpgha >= nCntMax)
			{
				//次のステートに移行する
				m_nLogoAlpgha = 0;
				m_bCol = true;
			}
			//超えていなかったらカウント増加
			else { m_nLogoAlpgha++; }

		}
	}
	//色変更完了していたら
	else
	{
		//非透明に近づけていく
		TitleLogoCol.a += MAX_ALPHA / nAmoValue;

		//最大値まで近づいたら
		if (TitleLogoCol.a >= MAX_ALPHA)
		{
			//透明度をその値にする
			TitleLogoCol.a = MAX_ALPHA;	

			//超えていたらカウントの初期化と終了合図を送る
			if (m_nLogoAlpgha >= nCntMax)
			{
				//次のステートに移行する
				m_nLogoAlpgha = 0;
				m_bCol = false;
			}
			//最大値ではなかったらカウント増加
			else { m_nLogoAlpgha++; }

		}
	}
	//色設定
	pObj2D->SetCol(TitleLogoCol);
}
//<===============================================
//ライトオフ処理
//<===============================================
void CTitle::LightOff(void)
{
	constexpr int nCountMax = 200;													//カウンターの固定値
	constexpr int nRandMax = 7;														//ランダム変数の最大値
	constexpr char* TEX_LIGHTON = "data\\TEXTURE\\Title\\Title_logo.png";			//タイトルロゴ(ライトオン)
	constexpr char* TEX_LIGHTOFF = "data\\TEXTURE\\Title\\Title_logo_lightoff.png";	//タイトルロゴ(ライトオフVer);

	//最大値まで行っていたら
	if (m_nLogoCou >= nCountMax)
	{
		//テクスチャがライトオフされていない状態だったら
		if (m_pObject2D[OBJ2D_TITLELOGO]->GetIdxTex() ==
			CManager::GetInstance()->GetTexture()->Regist(TEX_LIGHTON))
		{
			//ライトオフされているテクスチャに変更する
			m_pObject2D[OBJ2D_TITLELOGO]->BindTexture
			(CManager::GetInstance()->GetTexture()->Regist(TEX_LIGHTOFF));
		}
		//テクスチャがライトオフされていたら
		else if (m_pObject2D[OBJ2D_TITLELOGO]->GetIdxTex() ==
			CManager::GetInstance()->GetTexture()->Regist(TEX_LIGHTOFF))
		{
			//ライトオフされていないテクスチャにする
			m_pObject2D[OBJ2D_TITLELOGO]->BindTexture
			(CManager::GetInstance()->GetTexture()->Regist(TEX_LIGHTON));
		}

		//初期化
		m_nLogoCou = 0;
	}
	else{m_nLogoCou += rand() % nRandMax;}
}
//<===============================================
//追跡ステートに移行した際の動き
//<===============================================
void CTitle::ChaseMovement(void)
{
	//選択判定があれば
	if (m_bSelected) 
	{ 
		//不要になった2Dオブジェクトの破棄
		for (int nCnt = OBJ2D::OBJ2D_FRAME; nCnt < OBJ2D::OBJ2D_MAX; nCnt++)
		{SAFE_UNINIT(m_pObject2D[nCnt]);}

		//シングルかマルチかの選択肢の破棄
		for (int nCnt = 0; nCnt < SELECT_MAX; nCnt++)
		{SAFE_UNINIT(m_apSelect[nCnt]);}

		//はいといいえの選択肢の破棄
		for (int nCnt = 0; nCnt < SELECT_YN_MAX; nCnt++)
		{SAFE_UNINIT(m_apYesNoObj[nCnt]);}

		//ブラックカバーを付ける(非透明にする)
		m_pObject2D[OBJ2D::OBJ2D_BLACKCOVER]->SetCol(VIS_COL);

		//アイスステートに移行し、変数の設定をする
		m_eState = STATE::STATE_ICETHROW; 
		m_bIniting = false;
		m_nCounter = 0; 
	}
	//無かったら
	else
	{
		Selecting();
		Chasing();
	}
}
//<===============================================
//追跡ステートの追跡処理
//<===============================================
void CTitle::Chasing(void)
{
	//プレイヤーと警察を動かす
	m_pPlayer->MovingSelect();
	ChaseCamera();
}
//<===============================================
//追跡ステートの際のカメラの動き
//<===============================================
void CTitle::ChaseCamera(void)
{
	//<************************************************************
	//カメラ関係の変数
	//<************************************************************
	D3DXVECTOR3 CameraRot = m_pCam->GetRotation();		//カメラ向き
	D3DXVECTOR3 CameraPos = m_pCam->GetPositionR();		//カメラ位置

	//向き移動の際の移動値と目的向き
	constexpr float fRotMoveY = 0.02f,
		fRotMoveZ = 0.005f,
		fDestRotY = -0.66f,
		fDestRotZ = 1.15f;

	float rotY = m_pPlayer->GetRotation().y - D3DX_PI * 0.5f;

	float CameraPosDif[3] = {};	//カメラの補正距離
	CameraPosDif[0] = sinf(rotY) * 200.0f;
	CameraPosDif[1] = 210.0f;
	CameraPosDif[2] = cosf(rotY) * 200.0f;
		 
	float CameraDis = 2500.0f;							//カメラの距離

	//カメラの向きの調整
	if (CameraRot.y <= fDestRotY) { CameraRot.y = fDestRotY; }
	else { CameraRot.y -= fRotMoveY; }

	//カメラの向きの調整
	if (CameraRot.z >= fDestRotZ) { CameraRot.z = fDestRotZ; }
	else { CameraRot.z += fRotMoveZ; }
	
	//カメラの設定
	{
		float posX = CameraPos.x + ((m_pPlayer->GetPosition().x + CameraPosDif[0]) - CameraPos.x) * 0.075f;
		float posY = CameraPos.y + ((m_pPlayer->GetPosition().y + CameraPosDif[1]) - CameraPos.y) * 0.075f;
		float posZ = CameraPos.z + ((m_pPlayer->GetPosition().z + CameraPosDif[2]) - CameraPos.z) * 0.075f;
		m_pCam->SetPositionR(D3DXVECTOR3(posX,
			posY,
			posZ));
	}

	m_pCam->SetLength(CameraDis);
	m_pCam->SetRotation(CameraRot);
}
//<===============================================
//スキップした際の際の動き
//<===============================================
void CTitle::SkipMovement(void)
{
	const D3DXVECTOR3 PLAYER_POS = { 2630.0f, 50.0f, -250.0f };	//プレイヤーの位置
	
	//・タイトルロゴ
	m_pObject2D[OBJ2D::OBJ2D_TITLELOGO]->SetPosition(D3DXVECTOR3(TITLELOGO_DEST,TITLELOGO_POS.y, TITLELOGO_POS.z));
	m_pObject2D[OBJ2D::OBJ2D_TITLELOGO]->SetVtx();
	m_pObject2D[OBJ2D::OBJ2D_TITLELOGO]->SetDraw(true);

	//・プレスエンター
	m_pObject2D[OBJ2D::OBJ2D_PressEnter]->SetPosition(D3DXVECTOR3(TITLELOGO_DEST, PRESSENTER_POS.y, PRESSENTER_POS.z));
	m_pObject2D[OBJ2D::OBJ2D_PressEnter]->SetVtx();
	m_pObject2D[OBJ2D::OBJ2D_PressEnter]->SetDraw(true);

	//・プレイヤー
	m_pPlayer->SetPosition(PLAYER_POS);
	m_pPlayer->SetRotation(DEST_ROT);
	m_pPlayer->SetReached(true);

	//ステートを変更する
	m_eState = STATE::STATE_PRESSENTER;
}
//<===============================================
//人数選択に使う変数の初期化
//<===============================================
void CTitle::InitingSelect(void)
{
	//初期化済みであれば抜ける
	if (m_bIniting) { return; }

	//テクスチャネーム
	constexpr char* TEX_MODESELECT = "data\\TEXTURE\\Title\\select.png";					//"MODE SELECT"の文字
	constexpr char* TEX_CHECK = "data\\TEXTURE\\Title\\start.png";							//確認文字

	//選択肢(シングルとマルチ)
	constexpr char* SELECT_NAME[CTitle::SELECT_MAX] =
	{
		"data\\TEXTURE\\Title\\silen.png",	//シングルプレイ
		"data\\TEXTURE\\Title\\multi.png",	//マルチプレイ
	};

	//選択肢(YESNO)
	constexpr char* SELECT_YN_NAME[CTitle::SELECT_YN_MAX] =
	{
		"data\\TEXTURE\\Title\\YES.png",	//はい
		"data\\TEXTURE\\Title\\NO.png",	//いいえ
	};

	//サイズ関連
	constexpr float fSizeModeSelect[SIZING_MAX] = { 300.0f, 40.0f };					//サイズ(MODE SELECT)
	constexpr float fSizeSINGLEMULTI[SIZING_MAX] = { 200.0f, 50.0f };					//サイズ(シングルとマルチ)
	constexpr float fSizeCHECK[SIZING_MAX] = { 400.0f, 50.0f };							//サイズ(確認文字)
	constexpr float fSizeYESNO[SIZING_MAX] = { 52.0f, 40.0f };							//サイズ(選択肢YESNO)

	//オブジェクト2D関連
	const D3DXCOLOR InitFrameCol = D3DXCOLOR(0.0f,0.0f,0.0f,0.6f);					//フレームの初期色
	const D3DXVECTOR3 NUMCHAR_POS = D3DXVECTOR3(625.0f, 100.0f, 0.0f);					//どっちか
	const D3DXVECTOR3 CHECK_POS = D3DXVECTOR3(625.0f, 475.0f, 0.0f);					//確認
		 
	const D3DXVECTOR3 SELECT_POS = D3DXVECTOR3(400.0f, 275.0f, 0.0f);					//選択肢の位置
	constexpr float fDis_SELECT = 475.0f;												//距離1	

	const D3DXVECTOR3 YES_POS = D3DXVECTOR3(485.0f, CHECK_POS.y + 150.0f, 0.0f);		//はいといいえの位置
	constexpr float fDis_YESNO = 250.0f;												//距離2

	//bool型の情報定義
	m_bIniting = true;
	m_bPush = false;

	//サウンドを止める
	m_pPlayer->SetS(false);
	m_pPlayer->SetRotation(D3DXVECTOR3(0.0f, -3.14f, 0.0f));

	//フレーム
	m_pObject2D[OBJ2D::OBJ2D_FRAME] = InitObj2D(TEAMLOGO_POS, VECTOR3_ZERO, nNatPriority,
		0.0f, 0.0f, true, nullptr, InitFrameCol);

	//確認文字
	m_pObject2D[OBJ2D::OBJ2D_NUMCHAR] = InitObj2D(NUMCHAR_POS, VECTOR3_ZERO, nNatPriority,
		fSizeModeSelect[SIZING_WIDTH], fSizeModeSelect[SIZING_HEIGHT], false, TEX_MODESELECT);

	//シングルかマルチかの選択肢
	for (int nCnt = 0; nCnt < SELECT_MAX; nCnt++)
	{

		//シングルとマルチ
		m_apSelect[nCnt] = InitObj2D(D3DXVECTOR3(
			SELECT_POS.x + fDis_SELECT * nCnt,
			SELECT_POS.y,
			SELECT_POS.z), 
			VECTOR3_ZERO, 
			nNatPriority,
			fSizeSINGLEMULTI[SIZING_WIDTH],
			fSizeSINGLEMULTI[SIZING_HEIGHT], 
			false, SELECT_NAME[nCnt]);
	}

	//確認メッセージ
	m_pObject2D[OBJ2D::OBJ2D_CHECK] = InitObj2D(CHECK_POS, VECTOR3_ZERO, nNatPriority,
		fSizeCHECK[SIZING_WIDTH], fSizeCHECK[SIZING_HEIGHT], false, TEX_CHECK);

	//はいといいえの選択肢
	for (int nCnt = 0; nCnt < SELECT_YN_MAX; nCnt++)
	{
		m_apYesNoObj[nCnt] = InitObj2D(D3DXVECTOR3(
			YES_POS.x + fDis_YESNO * nCnt,
			YES_POS.y,
			YES_POS.z),
			VECTOR3_ZERO,
			nNatPriority,
			fSizeYESNO[SIZING_WIDTH],
			fSizeYESNO[SIZING_HEIGHT],
			false, SELECT_YN_NAME[nCnt]);
	}

	//ブラックカバーを外す(透明にする)
	m_pObject2D[OBJ2D::OBJ2D_BLACKCOVER]->SetCol(INV_COL);

	//サイレン音と到着情報を設定
	m_pPlayer->SetReached(false);
	CPoliceTitle::SetSiren(true);
	
}
//<===============================================
//フレームのサイズ調整
//<===============================================
void CTitle::Sizing(void)
{
	D3DXVECTOR3 FrameSize = m_pObject2D[OBJ2D::OBJ2D_FRAME]->GetSize();		//フレームサイズ
	constexpr float fSpeedX = 21.0f,fSpeedY = 15.0f;						//スピード

	const D3DXVECTOR3 FRAME_DEST = { 500.0f,320.0f,0.0f };					//フレームの目標値

	//X軸
	if (FrameSize.x >= FRAME_DEST.x) { FrameSize.x = FRAME_DEST.x; }
	else { FrameSize.x += fSpeedX;}

	//Y軸
	if (FrameSize.y >= FRAME_DEST.y) { FrameSize.y = FRAME_DEST.y; }
	else { FrameSize.y += fSpeedY;}

	//サイズ調整が終わったら、合図をtrueにする
	if (FrameSize.y >= FRAME_DEST.y&& FrameSize.x >= FRAME_DEST.x) { m_bSizing = true; }

	//サイズを設定
	m_pObject2D[OBJ2D::OBJ2D_FRAME]->SetSize(FrameSize.x,FrameSize.y);
}
//<===============================================
//人数選択の際の処理
//<===============================================
void CTitle::Selecting(void)
{
	const D3DXVECTOR3 NUMBER_POS = D3DXVECTOR3(625.0f, 325.0f, 0.0f);		//数字の位置
	constexpr int ONE_PLAYER = 1,MAX_PLAYER = 4;								//プレイヤーの数

	//初期化
	InitingSelect();

	//サイズ調整が終わっていたら
	if (m_bSizing)
	{
		//描画をオンにする
		if (!m_pObject2D[OBJ2D::OBJ2D_NUMCHAR]->GetDraw()) { m_pObject2D[OBJ2D::OBJ2D_NUMCHAR]->SetDraw(true); }

		//シングルかマルチかの選択肢
		for (int nCnt = 0; nCnt < SELECT_MAX; nCnt++)
		{
			//描画をオンにする
			if (!m_apSelect[nCnt]->GetDraw()) { m_apSelect[nCnt]->SetDraw(true); }

			//無駄なfor文を避けるため
			else { break; }
		}

		//反応があったら
		if (m_bPush)
		{
			//シングルが透明になっていたら元の色に戻す
			if (m_apSelect[SELECT::SELECT_SINGLE]->GetCol().a == ALPHA_ZERO)
			{m_apSelect[SELECT::SELECT_SINGLE]->SetCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));}

			//マルチが透明になっていたら元の色に戻す
			else if (m_apSelect[SELECT::SELECT_MULTI]->GetCol().a == ALPHA_ZERO)
			{m_apSelect[SELECT::SELECT_MULTI]->SetCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));}
			
			SelectYesNO();
		}
		//無ければ
		else
		{
			SelectSingleMulti();
		}
	}
	//終っていなかったら、サイズ調整を行う
	else
	{
		Sizing();
	}
}
//<===============================================
//選択肢(シングルかマルチ)
//<===============================================
void CTitle::SelectSingleMulti(void)
{
	constexpr int nChangeColTime = 10;										//色を変更する際のタイマー

	//描画をオフにする
	if (m_pObject2D[OBJ2D::OBJ2D_CHECK]->GetDraw()) { m_pObject2D[OBJ2D::OBJ2D_CHECK]->SetDraw(false); }

	//はいかいいえの選択肢
	for (int nCnt = 0; nCnt < SELECT_YN_MAX; nCnt++)
	{
		//描画をオフにする
		if (m_apYesNoObj[nCnt]->GetDraw()) { m_apYesNoObj[nCnt]->SetDraw(false); }

		//無駄なfor文を避けるため
		else { break; }
	}

	//押下情報と選択情報をYESに設定
	if (TriggerEnter()) {m_bPush = true; m_nSelectYN = SELECT_YN::SELECT_YN_YES;}

	//右ボタン
	if (CInputKeyboard::GetInstance()->GetTrigger(DIK_RIGHTARROW) || 
		CInputPad::GetInstance()->GetTrigger(CInputPad::BUTTON_RIGHT, 0))
	{ m_nSelect = (m_nSelect + 1) % SELECT_MAX; }

	//左ボタン
	else if (CInputKeyboard::GetInstance()->GetTrigger(DIK_LEFTARROW) || 
		CInputPad::GetInstance()->GetTrigger(CInputPad::BUTTON_LEFT, 0))
	{ m_nSelect = (m_nSelect + (SELECT_MAX - 1)) % SELECT_MAX; }

	//"YES"を選択している時
	if (m_nSelect == SELECT::SELECT_SINGLE)
	{
		//"YES"を選択状態、"NO"を非選択状態
		ColChange(m_apSelect[SELECT::SELECT_SINGLE], nChangeColTime);
		m_apSelect[SELECT::SELECT_MULTI]->SetCol(ONE_COL);
	}
	//"NO"を選択している時
	else if (m_nSelect == SELECT::SELECT_MULTI)
	{
		//"YES"を非選択状態、"NO"を選択状態
		ColChange(m_apSelect[SELECT::SELECT_MULTI], nChangeColTime);
		m_apSelect[SELECT::SELECT_SINGLE]->SetCol(ONE_COL);
	}
}
//<===============================================
//選択肢(YesかNo)
//<===============================================
void CTitle::SelectYesNO(void)
{
	//描画をオンにする
	if (!m_pObject2D[OBJ2D::OBJ2D_CHECK]->GetDraw()) { m_pObject2D[OBJ2D::OBJ2D_CHECK]->SetDraw(true); }

	//はいかいいえの選択肢
	for (int nCnt = 0; nCnt < SELECT_YN_MAX; nCnt++)
	{
		//描画をオンにする
		if (!m_apYesNoObj[nCnt]->GetDraw()) { m_apYesNoObj[nCnt]->SetDraw(true); }

		//無駄なfor文を避けるため
		else { break; }
	}

	//人数選択をする
	if (CInputKeyboard::GetInstance()->GetTrigger(DIK_RIGHTARROW) 
		|| CInputPad::GetInstance()->GetTrigger(CInputPad::BUTTON_RIGHT, 0)) 
	{ m_nSelectYN = (m_nSelectYN + 1) % SELECT_YN_MAX; }

	else if (CInputKeyboard::GetInstance()->GetTrigger(DIK_LEFTARROW) 
		|| CInputPad::GetInstance()->GetTrigger(CInputPad::BUTTON_LEFT, 0))
	{ m_nSelectYN = (m_nSelectYN + (SELECT_YN_MAX - 1)) % SELECT_YN_MAX; }

	//入力があれば
	if (TriggerEnter())
	{
		
		//"YES"を選択したとき、ゲーム画面に移行する
		if (m_nSelectYN == SELECT_YN::SELECT_YN_YES){m_bSelected = true;}

		//"NO"を選択したとき、押下情報を元に戻す
		else if (m_nSelectYN == SELECT_YN::SELECT_YN_NO){m_bPush = false;}
	}

	SelectCol();
}
//<===============================================
//選択している際の色
//<===============================================
void CTitle::SelectCol(void)
{	
	constexpr int nChangeColTime = 7;										//色を変更する際のタイマー

	//"YES"を選択している時
	if (m_nSelect == SELECT::SELECT_SINGLE) 
	{
		//選択されていないマルチを透明にする
		m_apSelect[SELECT::SELECT_SINGLE]->SetCol(ONE_COL);
		m_apSelect[SELECT::SELECT_MULTI]->SetCol(ZERO_COL);
	
	}

	//"NO"を選択している時
	else if (m_nSelect == SELECT::SELECT_MULTI)
	{
		//選択されていないシングルを透明にする
		m_apSelect[SELECT::SELECT_SINGLE]->SetCol(ZERO_COL);
		m_apSelect[SELECT::SELECT_MULTI]->SetCol(ONE_COL);
	}

	//"YES"を選択している時
	if (m_nSelectYN == SELECT_YN::SELECT_YN_YES)
	{
		//"YES"を選択状態、"NO"を非選択状態
		ColChange(m_apYesNoObj[SELECT_YN::SELECT_YN_YES], nChangeColTime);
		m_apYesNoObj[SELECT_YN::SELECT_YN_NO]->SetCol(ONE_COL);
	}
	//"NO"を選択している時
	else if (m_nSelectYN == SELECT_YN::SELECT_YN_NO)
	{
		//"YES"を非選択状態、"NO"を選択状態
		ColChange(m_apYesNoObj[SELECT_YN::SELECT_YN_NO], nChangeColTime);
		m_apYesNoObj[SELECT_YN::SELECT_YN_YES]->SetCol(ONE_COL);
	}

}
//<===============================================
//デバッグ時のカメラの動き
//<===============================================
void CTitle::DebugCam(void)
{
#ifdef _DEBUG
	D3DXVECTOR3 CamRot = m_pCam->GetRotation();	//カメラ向き
	constexpr float fRotMove = 0.01f;				//カメラ向きの移動値

	//<*******************************************************************************
	//カメラ移動
	//X軸
	if (CInputKeyboard::GetInstance()->GetPress(DIK_H)) { CamRot.x -= fRotMove; }
	if (CInputKeyboard::GetInstance()->GetPress(DIK_B)) { CamRot.x += fRotMove; }
	//Y軸
	if (CInputKeyboard::GetInstance()->GetPress(DIK_J)) { CamRot.y -= fRotMove; }
	if (CInputKeyboard::GetInstance()->GetPress(DIK_N)) { CamRot.y += fRotMove; }
	//Z軸
	if (CInputKeyboard::GetInstance()->GetPress(DIK_K)) { CamRot.z -= fRotMove; }
	if (CInputKeyboard::GetInstance()->GetPress(DIK_M)) { CamRot.z += fRotMove; }

	//カメラ情報設定
	m_pCam->SetRotation(CamRot);
#endif

}
//<===============================================
//アイスステート時の初期化
//<===============================================
void CTitle::InitingIce(void)
{
	//初期化されていたら処理を抜ける
	if (m_bIniting) { return; }

	const D3DXVECTOR3 PLAYER_POS = { 2630.0f, 50.0f, -200.0f };			//プレイヤーの位置
	constexpr float fDis = 150.0f;										//警察の距離
	constexpr float fLength = 1350.0f;									//カメラの長さ

	//初期化したことにする
	m_bIniting = true;
	m_bPush = false;

	//位置情報初期化
	m_pPlayer->SetPosition(PLAYER_POS);
	m_pPlayer->SetRotation(DEST_ROT);

	//警察の生成
	for (int nCnt = 0; nCnt < INITIAL::POLICE_MAX; nCnt++)
	{
		//位置情報初期化
		m_apPolice[nCnt]->SetPosition(D3DXVECTOR3(PolicePos.x + fDis * nCnt, PolicePos.y, PolicePos.z));
		m_apPolice[nCnt]->SetRotation(DEST_ROT);
	}

	//カメラの設定
	m_pCam->SetRotation(D3DXVECTOR3(0.0f, 0.0f, 1.57f));
	m_pCam->SetLength(fLength);

	//ブラックカバーを外す(透明にする)
	m_pObject2D[OBJ2D::OBJ2D_BLACKCOVER]->SetCol(INV_COL);
}
//<===============================================
//アイスステート時の動き
//<===============================================
void CTitle::IceMovement(void)
{
	D3DXVECTOR3 PlayerPos = VECTOR3_ZERO;							//プレイヤー位置
	constexpr int FADE_TIME = 200;									//ゲーム画面に移行するまでの時間
	constexpr int FADE_TIME_HARF = 65;								//ゲーム画面に移行するまでの時間の半減値

	//<*************************************************************
	//float型
	//<*************************************************************
	constexpr float PlayerMove = 50.0f;								//プレイヤーの動く値
	constexpr float CamHeightDis = 150.0f;							//高さの差
	constexpr float fDisMax = 900.0f;								//距離の最大値
	constexpr float fMoveValue = 7.5f;								//距離移動の際の値

	InitingIce();

	m_pPlayer->BaggageMove();

	//プレイヤー位置設定
	PlayerPos = m_pPlayer->GetPosition();
	PlayerPos.z += PlayerMove;
	m_pPlayer->SetPosition(PlayerPos);

	//警察の生成
	for (int nCnt = 0; nCnt < INITIAL::POLICE_MAX; nCnt++)
	{
		//追跡
		m_apPolice[nCnt]->Chasing(PlayerMove);
	}

	//カメラの位置を設定
	m_pCam->SetPositionR(D3DXVECTOR3
	(m_pPlayer->GetTitleGoal()->GetPos().x - m_fDis,
		m_pPlayer->GetTitleGoal()->GetPos().y+ CamHeightDis,
		m_pPlayer->GetTitleGoal()->GetPos().z));

	//フェードステートが何もなしだったら
	if (CManager::GetInstance()->GetFade()->GetState() == CFade::STATE_NONE)
	{
		//超えていたらゲーム画面に遷移する
		if (m_nCounter >= FADE_TIME)
		{
			//シングルプレイが選択されていたら、そのままゲーム画面に遷移
			if (GetSelectSingleMulti() == SELECT::SELECT_SINGLE)
			{CManager::GetInstance()->GetFade()->Set(CScene::MODE_GAME);}

			//マルチプレイが選択されていたら、エントリー画面に遷移
			else if (GetSelectSingleMulti() == SELECT::SELECT_MULTI)
			{CManager::GetInstance()->GetFade()->Set(CScene::MODE_ENTRY);}
		}

		//超えていなかったらカウント増加
		else { m_nCounter++; }
	}

	//半分ぐらいの値になっていたら
	if (m_nCounter >= FADE_TIME_HARF)
	{
		//距離が一定値になったらその値にし、なっていなかったら近づける
		if (m_fDis >= fDisMax) { m_fDis = fDisMax; }
		else { m_fDis += fMoveValue;}

		//サイレン音を止める
		CPoliceTitle::SetSiren(false);
	}

	//スキップ用の入力処理
	if (TriggerEnter())
	{
		//シングルプレイが選択されていたら、そのままゲーム画面に遷移
		if (GetSelectSingleMulti() == SELECT::SELECT_SINGLE)
		{CManager::GetInstance()->GetFade()->Set(CScene::MODE_GAME);}

		//マルチプレイが選択されていたら、エントリー画面に遷移
		else if (GetSelectSingleMulti() == SELECT::SELECT_MULTI)
		{CManager::GetInstance()->GetFade()->Set(CScene::MODE_ENTRY);}
	}
}
//<===============================================
//エンターを押したかどうか
//<===============================================
bool CTitle::TriggerEnter(void) 
{
	//エンター押したら
	if (CInputKeyboard::GetInstance()->GetTrigger(DIK_RETURN) ||
		CInputPad::GetInstance()->GetTrigger(CInputPad::BUTTON_START, 0) ||
		CInputPad::GetInstance()->GetTrigger(CInputPad::BUTTON_A, 0))
	{
		return true;
	}

	return false;
}
//<===============================================
//オブジェクト2Dの初期化
//<===============================================
CObject2D* CTitle::InitObj2D(const D3DXVECTOR3 rPos,
	const D3DXVECTOR3 rRot,
	const int nPri,
	const float fWidth,
	const float fHeight,
	const bool bDraw,
	const char* pTexName,
	const D3DXCOLOR rCol)
{
	//生成用のオブジェクト
	CObject2D* pObj2D = nullptr;

	//オブジェクト2Dの初期化を行う
	pObj2D = CObject2D::Create(rPos, rRot, nPri);
	pObj2D->SetSize(fWidth, fHeight);
	pObj2D->SetDraw(bDraw);

	//テクスチャの名前があったら
	if (pTexName)
	{
		pObj2D->BindTexture(CManager::GetInstance()->GetTexture()->Regist(pTexName));
	}

	//色の設定
	pObj2D->SetCol(rCol);	

	return pObj2D;
}
