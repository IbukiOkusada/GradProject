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
#include "TitleMap.h"
#include "meshfield.h"
#include "PlayerTitle.h"
#include "PoliceTitle.h"
#include "goal.h"
#include "camera_manager.h"
#include "number.h"

//===============================================
// 無名名前空間
//===============================================
namespace 
{
	//<************************************************
	//D3DXVECTOR3型
	//<************************************************ 
	const D3DXVECTOR3 PRESSENTER_POS = { SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.9f, 0.0f };		//プレスエンターの座標位置
	const D3DXVECTOR3 TITLELOGO_POS = { SCREEN_WIDTH, SCREEN_HEIGHT * 0.1f, 0.0f };				//タイトルロゴの座標位置
	const D3DXVECTOR3 TEAMLOGO_POS = { SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f ,0.0f };		//チームロゴの座標位置
	const D3DXVECTOR3 FRAME_DEST = { 500.0f,320.0f,0.0f };										//フレームの目標値

	//<************************************************
	//int型
	//<************************************************ 
	const int AUTOMOVE_RANKING = 1000;															//ランキング自動遷移時間
	const int MOVE_LOGO = 120;																	//次のステートに遷移するまでの時間

	//<************************************************
	//float型
	//<************************************************ 
	const float MAX_ALPHA = 1.0f;																//透明度の最大値
	const float MIN_ALPHA = 0.3f;																//透明度の最小値
	const float ALPHA_ZERO = 0.0f;																//透明の時のα値
	const float TITLELOGO_DEST = SCREEN_WIDTH * 0.5f;											//タイトルロゴの目標位置

	//<************************************************
	//テクスチャの名前関連
	//<************************************************ 
	//オブジェクト2Dに使うテクスチャの名前
	const char* TEX_NAME[CTitle::OBJ2D_MAX] = 
	{
		"data\\TEXTURE\\logo_thaw.jpg",			//チームロゴ
		"",										//黒カバー
		"data\\TEXTURE\\Pre_char000.png",		//タイトルロゴ
		"data\\TEXTURE\\T_PressEnter000.png",	//プレスエンター
		"",										//選択肢
		"data\\TEXTURE\\T_PressEnter001.png",	//"何人選択しますか"の文字
		"data\\TEXTURE\\T_PressEnter002.png",	//確認メッセージ
	};				
	//選択肢に使うテクスチャの名前
	const char* SELECT_NAME[CTitle::SELECT_MAX] =
	{
		"data\\TEXTURE\\GameOver-Yes.png",	//はい
		"data\\TEXTURE\\GameOver-No.png",	//いいえ
	};
}

//<===============================================
//コンストラクタ
//<===============================================
CTitle::CTitle()
{
	//int関係
	m_nCounterRanking = 0;
	m_nCounter = 0;
	m_nLogoAlpgha = 0;
	m_nNumSelect = 1;
	m_nSelect = SELECT_YES;
	m_TitlePos = VECTOR3_ZERO;

	m_eState = STATE::STATE_TEAMLOGO;

	//bool系
	m_bPush = false;
	m_bNext = false;
	m_bDisplay = false;
	m_bSkipped = false;
	m_bCol = false;
	m_bIniting = false;
	m_bSizing = false;

	//ポインタ系
	m_pPlayer = nullptr;
	m_pCam = nullptr;
	m_pGoal = nullptr;
	m_pNum = nullptr;

	//配列の初期化
	for (int nCnt = 0; nCnt < OBJ2D_MAX; nCnt++)
	{
		m_pObject2D[nCnt] = nullptr;
	}

	//配列の初期化
	for (int nCnt = 0; nCnt < SELECT::SELECT_MAX; nCnt++)
	{
		m_apYesNoObj[nCnt] = nullptr;
	}

	//配列の初期化
	for (int nCnt = 0; nCnt < POLICE_MAX; nCnt++)
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
	//遷移タイマー設定
	m_nCounter = MOVE_LOGO;
	m_nCounterRanking = AUTOMOVE_RANKING;

	//タイトルBGM再生とチームロゴオブジェクトの生成
	CManager::GetInstance()->GetSound()->Play(CSound::LABEL_BGM_TITLE);
	m_pObject2D[OBJ2D::OBJ2D_TeamLogo] = CObject2D::Create(TEAMLOGO_POS, VECTOR3_ZERO);
	m_pObject2D[OBJ2D::OBJ2D_TeamLogo]->SetSize(320, 160.0f);
	m_pObject2D[OBJ2D::OBJ2D_TeamLogo]->BindTexture(CManager::GetInstance()->GetTexture()->Regist(TEX_NAME[OBJ2D::OBJ2D_TeamLogo]));

	return S_OK;
}

//<===============================================
//終了処理
//<===============================================
void CTitle::Uninit(void)
{
	//カメラの視点の位置を初期化
	m_pCam->SetPositionR(VECTOR3_ZERO);
	CManager::GetInstance()->GetSound()->Stop();
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
			if (CInputKeyboard::GetInstance()->GetTrigger(DIK_RETURN) ||
				CInputPad::GetInstance()->GetTrigger(CInputPad::BUTTON_START, 0) ||
				CInputPad::GetInstance()->GetTrigger(CInputPad::BUTTON_A, 0)) {
				m_bPush = true;
			}

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
	const int nCountMax = 10;											// カウンターの固定値
	const int nAmoValue = 20;											//色変更時の値

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
	const int nCountMax = 10;	

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
	if (CInputKeyboard::GetInstance()->GetTrigger(DIK_RETURN) ||
		CInputPad::GetInstance()->GetTrigger(CInputPad::BUTTON_START, 0) ||
		CInputPad::GetInstance()->GetTrigger(CInputPad::BUTTON_A, 0))
	{
		m_bPush = true;
	}

	//反応があったら
	if (m_bPush)
	{
		//必要ないので終了処理を挟む(いるかは不明だけど念のためです)
		SAFE_UNINIT(m_pObject2D[OBJ2D::OBJ2D_TITLELOGO]);
		SAFE_UNINIT(m_pObject2D[OBJ2D::OBJ2D_PressEnter]);

		//次のステートに移行する
		m_eState = STATE::STATE_CHASING;
	}
	//押下反応がなかったら
	else
	{
		//m_nCounterRanking--;
		//if (m_nCounterRanking <= 0)
		//{
		//	//ランキング画面に移行
		//	CManager::GetInstance()->GetFade()->Set(CScene::MODE_RANKING);

		//}

	}
}
//<===============================================
//黒カバーの動き
//<===============================================
void CTitle::BlackCoverM(void)
{
	D3DXCOLOR BCoverCol = m_pObject2D[OBJ2D::OBJ2D_BLACKCOVER]->GetCol();	// チームロゴの色情報を取得
	const int nAmoValue = 20;												//色変更時の値
	BCoverCol.a -= MAX_ALPHA / nAmoValue;									//透明に近づけていく

	if (BCoverCol.a <= 0.0f){BCoverCol.a = ALPHA_ZERO;}

	m_pObject2D[OBJ2D::OBJ2D_BLACKCOVER]->SetCol(BCoverCol);
}
//<===============================================
//タイトルロゴの動き
//<===============================================
void CTitle::MovingLogo(void)
{
	const float fSpeed = 0.09f;													//タイトルロゴが動くスピード
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

	//
	m_pObject2D[OBJ2D::OBJ2D_TITLELOGO]->SetPosition(TitlePos);
	m_pObject2D[OBJ2D::OBJ2D_TITLELOGO]->SetVtx();
}
//<===============================================
//仮名ステートでの動き
//<===============================================
void CTitle::PreMove(void)
{
	float PlayerRot = m_pPlayer->GetRotation().y;		//プレイヤーの向きを変える
	const float fDestRot = -3.14f, fRotMove = 0.05f;		//目的の向きと向きの移動値

	ColChange(m_pObject2D[OBJ2D_PressEnter]);

	//一番目の目的地にプレイヤーを移動させる
	m_pPlayer->Moving(CPlayerTitle::DEST_SECOND);

	//その場所についているかつプレスエンターの文字が表示されていなければ
	if (m_pPlayer->GetReached())
	{
		//目的の向きになっていたら
		if (PlayerRot <= fDestRot) { PlayerRot = fDestRot; }

		//なっていなかったら回転し続ける
		else { PlayerRot -= fRotMove; }

		//プレイヤーの向きに反映
		m_pPlayer->SetRotation(D3DXVECTOR3(0.0f, PlayerRot, 0.0f));
	}
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
	const D3DXVECTOR3 PLAYER_POS = { 2630.0f, 50.0f, -1988.0f };			//プレイヤーの位置
	const D3DXVECTOR3 PolicePos = { 2530.0f, 0.0f, -550.0f };		//警察位置
	const float fLogoLength = (150.0f, 150.0f);								//ロゴの長さ(サイズ)
	const float fP_ELength = (350.0f, 350.0f);								//プレスエンターの長さ(サイズ)

	//初期化済みではなければ
	if (!m_bIniting)
	{
		//2つのbool情報を初期化
		m_bPush = false;
		m_bNext = false;

		//必要ないので終了処理を挟む(いるかは不明だけど念のためです)
		SAFE_UNINIT(m_pObject2D[OBJ2D::OBJ2D_TeamLogo]);

		//カメラ初期状態
		m_pCam = CCameraManager::GetInstance()->GetTop();
		m_pCam->SetPositionR(D3DXVECTOR3(3350.0f, 95.0f, 260.0f));
		m_pCam->SetLength(100.0f);
		m_pCam->SetRotation(D3DXVECTOR3(0.0f, -0.0f, 1.79f));
		m_pCam->SetActive(true);

		//<******************************************
		// 2Dオブジェクトの生成処理
		//<******************************************
		//・ブラックカバー
		m_pObject2D[OBJ2D::OBJ2D_BLACKCOVER] = CObject2D::Create(TEAMLOGO_POS, VECTOR3_ZERO,6);
		m_pObject2D[OBJ2D::OBJ2D_BLACKCOVER]->SetSize(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f);
		m_pObject2D[OBJ2D::OBJ2D_BLACKCOVER]->SetCol(D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f));

		//・タイトルロゴ
		m_pObject2D[OBJ2D::OBJ2D_TITLELOGO] = CObject2D::Create(TITLELOGO_POS, VECTOR3_ZERO, 5);
		m_pObject2D[OBJ2D::OBJ2D_TITLELOGO]->SetLength(fLogoLength);
		m_pObject2D[OBJ2D::OBJ2D_TITLELOGO]->SetDraw(false);
		m_pObject2D[OBJ2D::OBJ2D_TITLELOGO]->BindTexture(CManager::GetInstance()->GetTexture()->Regist(TEX_NAME[OBJ2D::OBJ2D_TITLELOGO]));

		//・プレスエンター
		m_pObject2D[OBJ2D::OBJ2D_PressEnter] = CObject2D::Create(PRESSENTER_POS, VECTOR3_ZERO,5);
		m_pObject2D[OBJ2D::OBJ2D_PressEnter]->SetSize(100.0f,100.0f);
		m_pObject2D[OBJ2D::OBJ2D_PressEnter]->SetDraw(false);
		m_pObject2D[OBJ2D::OBJ2D_PressEnter]->BindTexture(CManager::GetInstance()->GetTexture()->Regist(TEX_NAME[OBJ2D::OBJ2D_PressEnter]));

		//必要なオブジェクトの生成
		CTitleMap::GetInstance()->Load();
		CMeshField::Create(D3DXVECTOR3(0.0f, -10.0f, 0.0f), VECTOR3_ZERO, 1000.0f, 1000.0f, "data\\TEXTURE\\field000.jpg", 30, 30);
		m_pPlayer = CPlayerTitle::Create(PLAYER_POS, D3DXVECTOR3(0.0f, 3.14f, 0.0f), VECTOR3_ZERO,nullptr,nullptr);

		//警察の生成
		for (int nCnt = 0; nCnt < POLICE_MAX; nCnt++)
		{
			m_apPolice[nCnt] = CPoliceTitle::Create(D3DXVECTOR3(PolicePos.x + 150.0f *nCnt, PolicePos.y, PolicePos.z),
				D3DXVECTOR3(0.0f, 3.14f, 0.0f), VECTOR3_ZERO);
		}

		//初期化完了の合図をtrueにする
		m_bIniting = true;
	}
}
//<===============================================
//色変更処理
//<===============================================
void CTitle::ColChange(CObject2D* pObj2D)
{
	D3DXCOLOR TitleLogoCol = pObj2D->GetCol();									//そのオブジェクトの色情報を取得
	const int nCountMax = 25;													//カウンターの固定値
	const int nAmoValue = 10;													//色変化値

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
			if (m_nLogoAlpgha >= nCountMax)
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
			if (m_nLogoAlpgha >= nCountMax)
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
//追跡ステートに移行した際の動き
//<===============================================
void CTitle::ChaseMovement(void)
{
	//<*************************************************************
	//カメラに関する
	//<*************************************************************
	D3DXVECTOR3 PlayerPos = m_pPlayer->GetPosition();	//プレイヤー位置
	const float PlayerMove = 25.0f;						//プレイヤーの動く値
	const int FADE_TIME = 200;							//ゲーム画面に移行するまでの時間

	Selecting();
	ChaseCamera();

	//プレイヤーと警察を移動させる
	PlayerPos.z += PlayerMove;

	//カメラの向きとプレイヤーの位置の設定
	m_pPlayer->SetPosition(PlayerPos);

	//<******************************************
	//警察関連の処理
	for (int nCnt = 0; nCnt < POLICE_MAX; nCnt++)
	{
		m_apPolice[nCnt]->Chasing();
	}

	////超えていたらゲーム画面に遷移する
	//if (m_nCounter >= FADE_TIME) {CManager::GetInstance()->GetFade()->Set(CScene::MODE_GAME);}

	////超えていなかったらカウント増加
	//else { m_nCounter++; }
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
	const float fRotMoveY = 0.02f,
		fRotMoveZ = 0.005f,
		fDestRotY = -0.66f,
		fDestRotZ = 1.15f;

	const float CameraPosDif[2] = { 0.0f,210.0f };	//カメラの補正距離
	float CameraDis = 1150.0f;					//カメラの距離

	//カメラの向きの調整
	if (CameraRot.y <= fDestRotY) { CameraRot.y = fDestRotY; }
	else { CameraRot.y -= fRotMoveY; }

	//カメラの向きの調整
	if (CameraRot.z >= fDestRotZ) { CameraRot.z = fDestRotZ; }
	else { CameraRot.z += fRotMoveZ; }

	//カメラの設定
	m_pCam->SetPositionR(D3DXVECTOR3(m_pPlayer->GetPosition().x + CameraPosDif[0],
		m_pPlayer->GetPosition().y + CameraPosDif[1],
		m_pPlayer->GetPosition().z));

	m_pCam->SetLength(CameraDis);
	m_pCam->SetRotation(CameraRot);
}
//<===============================================
//スキップした際の際の動き
//<===============================================
void CTitle::SkipMovement(void)
{
	const D3DXVECTOR3 PLAYER_POS = { 2630.0f, 50.0f, -250.0f };	//プレイヤーの位置
	const float DEST_ROT = 0.40f;								//目的の向き

	//・タイトルロゴ
	m_pObject2D[OBJ2D::OBJ2D_TITLELOGO]->SetPosition(D3DXVECTOR3(TITLELOGO_DEST,TITLELOGO_POS.y, TITLELOGO_POS.z));
	m_pObject2D[OBJ2D::OBJ2D_TITLELOGO]->SetVtx();
	m_pObject2D[OBJ2D::OBJ2D_TITLELOGO]->SetDraw(true);

	//・プレスエンター
	m_pObject2D[OBJ2D::OBJ2D_PressEnter]->SetDraw(true);

	//・プレイヤー
	m_pPlayer->SetPosition(PLAYER_POS);
	m_pPlayer->SetRotation(D3DXVECTOR3(0.0f, -3.14f, 0.0f));
	m_pPlayer->SetReached(true);

	//ステートを変更する
	m_eState = STATE::STATE_PRESSENTER;
}
//<===============================================
//人数選択に使う変数の初期化
//<===============================================
void CTitle::InitingSelect(void)
{
	const D3DXVECTOR3 NUMCHAR_POS = D3DXVECTOR3(625.0f, 100.0f, 0.0f);		//"何人ですか"の位置
	const D3DXVECTOR3 CHECK_POS = D3DXVECTOR3(625.0f, 450.0f, 0.0f);		//"何人ですか"の位置
	const D3DXVECTOR3 YES_POS = D3DXVECTOR3(505.0f, 525.0f, 0.0f);		//数字の位置
	const D3DXVECTOR3 NO_POS = D3DXVECTOR3(YES_POS.x + 150.0f, YES_POS.y, 0.0f);		//"何人ですか"の位置

	//初期化されていなかったら
	if (!m_bIniting)
	{
		//bool型の情報定義
		m_bIniting = true;
		m_bPush = false;

		//生成し、情報を設定する
		m_pObject2D[OBJ2D::OBJ2D_FRAME] = CObject2D::Create(TEAMLOGO_POS, VECTOR3_ZERO, 6);
		m_pObject2D[OBJ2D::OBJ2D_FRAME]->SetSize(0.0f, 0.0f);
		m_pObject2D[OBJ2D::OBJ2D_FRAME]->SetDraw(true);
		m_pObject2D[OBJ2D::OBJ2D_FRAME]->SetCol(D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.5f));

		//生成し、情報を設定する
		m_pObject2D[OBJ2D::OBJ2D_NUMCHAR] = CObject2D::Create(NUMCHAR_POS, VECTOR3_ZERO, 6);
		m_pObject2D[OBJ2D::OBJ2D_NUMCHAR]->SetSize(150.0f, 75.0f);
		m_pObject2D[OBJ2D::OBJ2D_NUMCHAR]->SetDraw(false);
		m_pObject2D[OBJ2D::OBJ2D_NUMCHAR]->BindTexture(CManager::GetInstance()->GetTexture()->Regist(TEX_NAME[OBJ2D::OBJ2D_NUMCHAR]));

		//生成し、情報を設定する
		m_pObject2D[OBJ2D::OBJ2D_CHECK] = CObject2D::Create(CHECK_POS, VECTOR3_ZERO, 6);
		m_pObject2D[OBJ2D::OBJ2D_CHECK]->SetSize(125.0f, 75.0f);
		m_pObject2D[OBJ2D::OBJ2D_CHECK]->SetDraw(false);
		m_pObject2D[OBJ2D::OBJ2D_CHECK]->BindTexture(CManager::GetInstance()->GetTexture()->Regist(TEX_NAME[OBJ2D::OBJ2D_CHECK]));

		//選択肢(はい)
		m_apYesNoObj[SELECT_YES] = CObject2D::Create(YES_POS, VECTOR3_ZERO, 6);
		m_apYesNoObj[SELECT_YES]->SetSize(100.0f, 50.0f);
		m_apYesNoObj[SELECT_YES]->SetDraw(false);
		m_apYesNoObj[SELECT_YES]->BindTexture(CManager::GetInstance()->GetTexture()->Regist(SELECT_NAME[SELECT::SELECT_YES]));
		//選択肢(いいえ)
		m_apYesNoObj[SELECT_NO] = CObject2D::Create(NO_POS, VECTOR3_ZERO, 6);
		m_apYesNoObj[SELECT_NO]->SetSize(100.0f, 50.0f);
		m_apYesNoObj[SELECT_NO]->SetDraw(false);
		m_apYesNoObj[SELECT_NO]->BindTexture(CManager::GetInstance()->GetTexture()->Regist(SELECT_NAME[SELECT::SELECT_NO]));

	}
}
//<===============================================
//フレームのサイズ調整
//<===============================================
void CTitle::Sizing(void)
{
	D3DXVECTOR3 FrameSize = m_pObject2D[OBJ2D::OBJ2D_FRAME]->GetSize();		//フレームサイズ
	const float fSpeed = 0.09f;												//スピード

	//X軸
	if (FrameSize.x >= FRAME_DEST.x) { FrameSize.x = FRAME_DEST.x; }
	else { FrameSize.x += (FRAME_DEST.x - FrameSize.x - 0.0f) * fSpeed; }

	//Y軸
	if (FrameSize.y >= FRAME_DEST.y) { FrameSize.y = FRAME_DEST.y; }
	else { FrameSize.y += (FRAME_DEST.y - FrameSize.y - 0.0f) * fSpeed; }

	//目的地に着いたら
	if (Function::BoolToDest(m_pObject2D[OBJ2D::OBJ2D_FRAME]->GetSize(),
		D3DXVECTOR3(FRAME_DEST), 1.0f, false))
	{
		//サイズ調整完了合図を設定
		m_bSizing = true;
	}

	//サイズを設定
	m_pObject2D[OBJ2D::OBJ2D_FRAME]->SetSize(FrameSize.x,FrameSize.y);
}
//<===============================================
//人数選択の際の処理
//<===============================================
void CTitle::Selecting(void)
{
	const D3DXVECTOR3 NUMBER_POS = D3DXVECTOR3(625.0f, 325.0f, 0.0f);		//数字の位置
	const int ONE_PLAYER = 1,MAX_PLAYER = 4;								//プレイヤーの数

	//初期化
	InitingSelect();

	//サイズ調整が終わっていたら
	if (m_bSizing)
	{
		//ナンバー生成と"何人か"の文字生成
		if (!m_pNum) { m_pNum = CNumber::Create(NUMBER_POS, 150.0f, 100.0f); }
		if (!m_pObject2D[OBJ2D::OBJ2D_NUMCHAR]->GetDraw()) { m_pObject2D[OBJ2D::OBJ2D_NUMCHAR]->SetDraw(true); }

		//反応があったら
		if (m_bPush)
		{
			m_pObject2D[OBJ2D::OBJ2D_CHECK]->SetDraw(true);
			m_apYesNoObj[SELECT_YES]->SetDraw(true);
			m_apYesNoObj[SELECT_NO]->SetDraw(true);

			SelectYesNO();
		}
		//無ければ
		else
		{
			//キーボード入力かパッド入力があれば
			if (CInputKeyboard::GetInstance()->GetTrigger(DIK_RETURN) ||
				CInputPad::GetInstance()->GetTrigger(CInputPad::BUTTON_START, 0) ||
				CInputPad::GetInstance()->GetTrigger(CInputPad::BUTTON_A, 0))
			{
				m_bPush = true;
			}

			//人数選択をする
			if (CInputKeyboard::GetInstance()->GetTrigger(DIK_RIGHTARROW)) { m_nNumSelect += 1; }
			else if (CInputKeyboard::GetInstance()->GetTrigger(DIK_LEFTARROW)) { m_nNumSelect -= 1; }

			//もし人数の最大値まで行っていたら
			if (m_nNumSelect >= MAX_PLAYER) { m_nNumSelect = MAX_PLAYER; }

			//人数の最小値まで行っていたら
			else if (m_nNumSelect <= 0) { m_nNumSelect = ONE_PLAYER; }

			//描画設定を変更
			m_pObject2D[OBJ2D::OBJ2D_CHECK]->SetDraw(false);
			m_apYesNoObj[SELECT_YES]->SetDraw(false);
			m_apYesNoObj[SELECT_NO]->SetDraw(false);

			//今の人数番号を設定
			m_pNum->SetIdx(m_nNumSelect);
		}
	}
	//終っていなかったら、サイズ調整を行う
	else
	{
		Sizing();
	}
}
//<===============================================
//選択肢(YesNo洗濯)
//<===============================================
void CTitle::SelectYesNO(void)
{
	//人数選択をする
	if (CInputKeyboard::GetInstance()->GetTrigger(DIK_RIGHTARROW)) { m_nSelect = (m_nSelect + 1) % SELECT_MAX; }
	else if (CInputKeyboard::GetInstance()->GetTrigger(DIK_LEFTARROW)) { m_nSelect = (m_nSelect + (SELECT_MAX - 1)) % SELECT_MAX; }

	//キーボード入力かパッド入力があれば
	if (CInputKeyboard::GetInstance()->GetTrigger(DIK_RETURN) ||
		CInputPad::GetInstance()->GetTrigger(CInputPad::BUTTON_START, 0) ||
		CInputPad::GetInstance()->GetTrigger(CInputPad::BUTTON_A, 0))
	{
		//"YES"を選択したとき、ゲーム画面に移行する
		if (m_nSelect == SELECT::SELECT_YES){CManager::GetInstance()->GetFade()->Set(CScene::MODE_GAME);}

		//"NO"を選択したとき、押下情報を元に戻す
		else if (m_nSelect == SELECT::SELECT_NO){m_bPush = false;}
	}

	SelectCol();
}
//<===============================================
//選択している際の色
//<===============================================
void CTitle::SelectCol(void)
{	
	//"YES"を選択している時
	if (m_nSelect == SELECT::SELECT_YES)
	{
		//"YES"を選択状態、"NO"を非選択状態
		ColChange(m_apYesNoObj[SELECT::SELECT_YES]);
		m_apYesNoObj[SELECT::SELECT_NO]->SetCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	}
	//"NO"を選択している時
	else if (m_nSelect == SELECT::SELECT_NO)
	{
		//"YES"を非選択状態、"NO"を選択状態
		ColChange(m_apYesNoObj[SELECT::SELECT_NO]);
		m_apYesNoObj[SELECT::SELECT_YES]->SetCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	}

}
//<===============================================
//デバッグ時のカメラの動き
//<===============================================
void CTitle::DebugCam(void)
{
#ifdef _DEBUG
	D3DXVECTOR3 CamRot = m_pCam->GetRotation();	//カメラ向き
	const float fRotMove = 0.01f;				//カメラ向きの移動値

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