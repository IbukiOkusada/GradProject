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

//===============================================
// 無名名前空間
//===============================================
namespace 
{
	const D3DXVECTOR3 PRESSENTER_POS = { SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.9f, 0.0f };		//プレスエンターの座標位置
	const D3DXVECTOR3 TITLELOGO_POS = { SCREEN_WIDTH, SCREEN_HEIGHT * 0.1f, 0.0f };				//タイトルロゴの座標位置
	const D3DXVECTOR3 TEAMLOGO_POS = { SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f ,0.0f };		//チームロゴの座標位置

	const float TITLELOGO_DEST = SCREEN_WIDTH * 0.5f;											//タイトルロゴの目標位置

	const int AUTOMOVE_RANKING = 1000;	// ランキング自動遷移時間
	const int MOVE_TUTORIAL = 120;		// チュートリアルに遷移するまでの時間
	const int ENEMY_NUM = 3;					//演出用敵(パトカー)出現数

	const float MAX_ALPHA = 1.0f;		//透明度の最大値
	const float MIN_ALPHA = 0.3f;		//透明度の最小値
	const float ALPHA_ZERO = 0.0f;		//透明の時のα値


	//const char* FILEPASS = "data\\TXT\\player";	// ファイルのパス
	//const char* FILEEXT = ".txt";				// ファイルの拡張子
	//const int FILEPASS_SIZE = (200);			// ファイルのパスサイズ
}

//<===============================================
//コンストラクタ
//<===============================================
CTitle::CTitle()
{
	m_nCounterRanking = 0;
	m_nCounter = 0;
	m_nLogoAlpgha = 0;
	m_TitlePos = VECTOR3_ZERO;

	m_eState = STATE::STATE_TEAMLOGO;

	//bool系
	m_bPush = false;
	m_bNext = false;
	m_bDisplay = false;
	m_bSkipped = false;
	m_bCol = false;
	m_bIniting = false;

	//ポインタ系
	m_pPlayer = nullptr;
	m_pCam = nullptr;

	//配列の初期化
	for (int nCnt = 0; nCnt < OBJ2D_MAX; nCnt++)
	{
		m_pObject2D[nCnt] = nullptr;
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
	// 遷移タイマー設定
	m_nCounter = MOVE_TUTORIAL;
	m_nCounterRanking = AUTOMOVE_RANKING;

	// サウンド再生
	CManager::GetInstance()->GetSound()->Play(CSound::LABEL_BGM_TITLE);
	m_pObject2D[OBJ2D::OBJ2D_TeamLogo] = CObject2D::Create(TEAMLOGO_POS, VECTOR3_ZERO);
	m_pObject2D[OBJ2D::OBJ2D_TeamLogo]->SetSize(320, 160.0f);
	m_pObject2D[OBJ2D::OBJ2D_TeamLogo]->BindTexture(CManager::GetInstance()->GetTexture()->Regist("data\\TEXTURE\\logo_thaw.jpg"));

	return S_OK;
}

//<===============================================
//終了処理
//<===============================================
void CTitle::Uninit(void)
{
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
		//次のステートに移行する
		m_eState = STATE::STATE_CHASING;

		////ランキング画面に移行
		//CManager::GetInstance()->GetFade()->Set(CScene::MODE_GAME);
	}
	//押下反応がなかったら
	else
	{
		m_nCounterRanking--;
		if (m_nCounterRanking <= 0)
		{
			//ランキング画面に移行
			CManager::GetInstance()->GetFade()->Set(CScene::MODE_RANKING);

		}

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

	TitleLogo();

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
	const float fLogoLength = (150.0f, 150.0f);								//ロゴの長さ(サイズ)
	const float fP_ELength = (350.0f, 350.0f);								//プレスエンターの長さ(サイズ)

	//初期化済みではなければ
	if (!m_bIniting)
	{
		//2つのbool情報を初期化
		m_bPush = false;
		m_bNext = false;

		//カメラ初期状態
		m_pCam = CManager::GetInstance()->GetCamera();
		m_pCam->SetPositionR(D3DXVECTOR3(-4000.0f, 95.0f, 260.0f));
		m_pCam->SetLength(100.0f);
		m_pCam->SetRotation(D3DXVECTOR3(0.0f, -0.0f, 1.79f));
		m_pCam->SetActive(false);

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
		m_pObject2D[OBJ2D::OBJ2D_TITLELOGO]->BindTexture(CManager::GetInstance()->GetTexture()->Regist("data\\TEXTURE\\Pre_char000.png"));

		//・プレスエンター
		m_pObject2D[OBJ2D::OBJ2D_PressEnter] = CObject2D::Create(PRESSENTER_POS, VECTOR3_ZERO,5);
		m_pObject2D[OBJ2D::OBJ2D_PressEnter]->SetSize(100.0f,100.0f);
		m_pObject2D[OBJ2D::OBJ2D_PressEnter]->SetDraw(false);
		m_pObject2D[OBJ2D::OBJ2D_PressEnter]->BindTexture(CManager::GetInstance()->GetTexture()->Regist("data\\TEXTURE\\T_PressEnter000.png"));

		//必要なオブジェクトの生成
		CMapManager::GetInstance()->Load();
		CMeshField::Create(D3DXVECTOR3(0.0f, -10.0f, 0.0f), VECTOR3_ZERO, 1000.0f, 1000.0f, "data\\TEXTURE\\field000.jpg", 30, 30);
		m_pPlayer = CPlayerTitle::Create(D3DXVECTOR3(-4734.0f, 50.0f, -1988.0f), D3DXVECTOR3(0.0f, 3.14f, 0.0f), VECTOR3_ZERO,nullptr,nullptr);

		for (int nCnt = 0; nCnt < POLICE_MAX; nCnt++)
		{
			m_apPolice[nCnt] = CPoliceTitle::Create(D3DXVECTOR3(-4734.0f + 150.0f * nCnt, 0.0f, -1300.0f), D3DXVECTOR3(0.0f, 3.14f, 0.0f), VECTOR3_ZERO);
		}

		//初期化完了の合図をtrueにする
		m_bIniting = true;
	}
}
//<===============================================
//タイトルロゴの動き
//<===============================================
void CTitle::TitleLogo(void)
{
	D3DXCOLOR TitleLogoCol = m_pObject2D[OBJ2D::OBJ2D_PressEnter]->GetCol();	//チームロゴの色情報を取得
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
	m_pObject2D[OBJ2D::OBJ2D_PressEnter]->SetCol(TitleLogoCol);
}
//<===============================================
//追跡ステートに移行した際の動き
//<===============================================
void CTitle::ChaseMovement(void)
{
	D3DXVECTOR3 CameraRot = m_pCam->GetRotation();		//カメラ向き
	D3DXVECTOR3 PlayerPos = m_pPlayer->GetPosition();	//プレイヤー位置
	D3DXVECTOR3 aPolicePos[POLICE_MAX] = {};			//警察の位置
	const float PlayerMove = 25.0f;						//プレイヤーの動く値
	const float fRotMove = 0.01f, fDestRot = -1.11f;	//向き移動の際の移動値と目的向き

	m_pCam->Pursue(m_pPlayer->GetPosition(), D3DXVECTOR3(0.0f, -0.0f, 1.79f), 1500.0f);
	//プレイヤーと警察を移動させる
	PlayerPos.z += PlayerMove;
	
	//カメラの向きとプレイヤーの位置の設定
	m_pPlayer->SetPosition(PlayerPos);

	//<******************************************
	//警察関連の処理
	for (int nCnt = 0; nCnt < POLICE_MAX; nCnt++)
	{
		//警察の位置を取得
		aPolicePos[nCnt].x = m_apPolice[nCnt]->GetPosition().x;
		aPolicePos[nCnt].y = m_apPolice[nCnt]->GetPosition().y;
		aPolicePos[nCnt].z = m_apPolice[nCnt]->GetPosition().z;

		///警察の位置を移動させ、位置を設定する
		aPolicePos[nCnt].z += PlayerMove;
		m_apPolice[nCnt]->SetPosition(aPolicePos[nCnt]);
	}
}
//<===============================================
//スキップした際の際の動き
//<===============================================
void CTitle::SkipMovement(void)
{
	const float DEST_ROT = 0.40f;						//目的の向き

	//・タイトルロゴ
	m_pObject2D[OBJ2D::OBJ2D_TITLELOGO]->SetPosition(D3DXVECTOR3(TITLELOGO_DEST,TITLELOGO_POS.y, TITLELOGO_POS.z));
	m_pObject2D[OBJ2D::OBJ2D_TITLELOGO]->SetVtx();
	m_pObject2D[OBJ2D::OBJ2D_TITLELOGO]->SetDraw(true);

	//・プレスエンター
	m_pObject2D[OBJ2D::OBJ2D_PressEnter]->SetDraw(true);

	//・プレイヤー
	m_pPlayer->SetPosition(D3DXVECTOR3(-4734.0f, 50.0f, -250.0f));
	m_pPlayer->SetRotation(D3DXVECTOR3(0.0f, -3.14f, 0.0f));
	m_pPlayer->SetReached(true);

	//ステートを変更する
	m_eState = STATE::STATE_PRESSENTER;
}