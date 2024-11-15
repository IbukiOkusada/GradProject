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
	const int AMO_VALUE = 20;			//色変更時の値

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
	m_bPush = false;
	m_bNext = false;
	m_bDisplay = false;
	m_eState = STATE::STATE_TEAMLOGO;
	m_bCol = false;
	m_pPlayer = nullptr;
	m_pCam = nullptr;

	//配列の初期化
	for (int nCnt = 0; nCnt < OBJ2D_MAX; nCnt++)
	{
		m_pObject2D[nCnt] = nullptr;
	}

	m_bIniting = false;

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
	m_pObject2D[OBJ2D::OBJ2D_TeamLogo] = CObject2D::Create(D3DXVECTOR3(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, 0.0f), VECTOR3_ZERO);
	m_pObject2D[OBJ2D::OBJ2D_TeamLogo]->SetSize(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f);
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

		//
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
	case STATE::STATE_PRESSENTER:

		InitingP_E();
		StateP_E();

		break;

		//プレスエンターを押した後のステートだった場合
	case STATE::STATE_CHASING:

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
	const int COUNT_MAX = 10;											// カウンターの固定値

	//ステートがチームロゴだった場合
	if (m_eState == STATE::STATE_TEAMLOGO)
	{
		if(!m_bCol)
		{
			TeamCol.a -= MAX_ALPHA / AMO_VALUE;	//透明に近づけていく

			if (TeamCol.a <= 0.0f)
			{//完全に透明になった場合
				TeamCol.a = 0.0f;	//透明度を透明に

				//超えていたらカウントの初期化と透明終了合図を送る
				if (m_nCounter >= COUNT_MAX)
				{
					//次のステートに移行する
					m_eState = STATE::STATE_PRESSENTER;
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
	const int COUNT_MAX = 10;	

	//その場所についていてプレスエンターの文字が表示されていなければ
	if (m_pPlayer->GetReached()&&!m_pObject2D[OBJ2D::OBJ2D_PressEnter]->GetDraw())
	{
		//超えていたら
		if (m_nCounter >= COUNT_MAX)
		{
			MovingLogo();
		}
		//超えていなかったらカウント増加
		else{m_nCounter++;}

	}
	//プレスエンターの文字が表示されていたら
	else if (m_pObject2D[OBJ2D::OBJ2D_PressEnter]->GetDraw())
	{
		TitleLogo();

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
}
//<===============================================
//タイトルロゴの動き
//<===============================================
void CTitle::MovingLogo(void)
{
	const float SPEED = 0.09f;							//タイトルロゴが動くスピード

	//描画設定をオンにし、移動用の変数にタイトルロゴの位置を取得させる
	m_pObject2D[OBJ2D::OBJ2D_TITLELOGO]->SetDraw(true);
	m_TitlePos = m_pObject2D[OBJ2D::OBJ2D_TITLELOGO]->GetPosition();

	//目的地まで移動させる
	m_TitlePos.x += (TITLELOGO_DEST - m_TitlePos.x - 0.0f) * 0.09f;//X軸

	//目的地に着いたら
	if (Function::MoveToDest(m_pObject2D[OBJ2D::OBJ2D_TITLELOGO]->GetPosition(),
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
	m_pObject2D[OBJ2D::OBJ2D_TITLELOGO]->SetPosition(m_TitlePos);
	m_pObject2D[OBJ2D::OBJ2D_TITLELOGO]->SetVtx();
}
//<===============================================
//プレスエンターステートでの動き
//<===============================================
void CTitle::InitingP_E(void)
{
	D3DXCOLOR TeamCol = m_pObject2D[OBJ2D::OBJ2D_TeamLogo]->GetCol();	// チームロゴの色情報を取得

	//初期化済みではなければ
	if (!m_bIniting)
	{
		//チームロゴを描画しない
		m_pObject2D[OBJ2D::OBJ2D_TeamLogo]->SetDraw(false);

		//2つの情報を初期化
		m_bPush = false;
		m_bNext = false;

		//カメラ初期状態
		m_pCam = CManager::GetInstance()->GetCamera();
		m_pCam->SetPositionR(D3DXVECTOR3(-4000.0f, 95.0f, 260.0f));
		m_pCam->SetLength(100.0f);
		m_pCam->SetRotation(D3DXVECTOR3(0.0f, -0.0f, 1.79f));
		//m_pCam->SetRotation(D3DXVECTOR3(0.0f, -2.1f, 1.79f));
		m_pCam->SetActive(false);

		//タイトルロゴ文字
		m_pObject2D[OBJ2D::OBJ2D_TITLELOGO] = CObject2D::Create(TITLELOGO_POS, VECTOR3_ZERO, 5);
		m_pObject2D[OBJ2D::OBJ2D_TITLELOGO]->SetLength(150.0f, 150.0f);
		m_pObject2D[OBJ2D::OBJ2D_TITLELOGO]->SetDraw(false);
		m_pObject2D[OBJ2D::OBJ2D_TITLELOGO]->BindTexture(CManager::GetInstance()->GetTexture()->Regist("data\\TEXTURE\\Pre_char000.png"));

		//プレスエンター文字
		m_pObject2D[OBJ2D::OBJ2D_PressEnter] = CObject2D::Create(PRESSENTER_POS, VECTOR3_ZERO,5);
		m_pObject2D[OBJ2D::OBJ2D_PressEnter]->SetLength(150.0f, 150.0f);
		m_pObject2D[OBJ2D::OBJ2D_PressEnter]->SetDraw(false);
		m_pObject2D[OBJ2D::OBJ2D_PressEnter]->BindTexture(CManager::GetInstance()->GetTexture()->Regist("data\\TEXTURE\\T_PressEnter000.png"));

		//必要なオブジェクトの生成
		CMapManager::GetInstance()->Load();
		CMeshField::Create(D3DXVECTOR3(0.0f, -10.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), 1000.0f, 1000.0f, "data\\TEXTURE\\field000.jpg", 30, 30);
		m_pPlayer = CPlayerTitle::Create(D3DXVECTOR3(-4734.0f, 50.0f, -1988.0f), D3DXVECTOR3(0.0f, 3.14f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f),nullptr,nullptr);

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
	const int COUNT_MAX = 50;													//カウンターの固定値

	//色変更完了していなければ
	if (!m_bCol) 
	{
		//透明に近づけていく
		TitleLogoCol.a += MAX_ALPHA / -AMO_VALUE;

		//その値まで行ったら
		if (TitleLogoCol.a <= MIN_ALPHA)
		{
			TitleLogoCol.a = MIN_ALPHA;	//透明度をその値にする

			//超えていたらカウントの初期化と透明終了合図を送る
			if (m_nLogoAlpgha >= COUNT_MAX)
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
		TitleLogoCol.a += MAX_ALPHA / AMO_VALUE;

		//最大値まで近づいたら
		if (TitleLogoCol.a >= MAX_ALPHA)
		{
			//透明度をその値にする
			TitleLogoCol.a = MAX_ALPHA;	

			//超えていたらカウントの初期化と終了合図を送る
			if (m_nLogoAlpgha >= COUNT_MAX)
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