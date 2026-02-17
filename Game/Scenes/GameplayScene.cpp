/// <summary>
/// Gameplayシーンに関するソースファイル
/// </summary>
/// <author>仲森智史</author>


// ヘッダファイルの読み込み
#include "pch.h"
#include "GameplayScene.h"

#include "Game/Scenes/ResultScene.h"
#include "Game/Scenes/TitleScene.h"
#include "Game/Commons/Factory.h"
#include "Game/Commons/Resources.h"
#include "Common/DebugDraw.h"



/// <summary>
/// コンストラクタ
/// </summary>
GameplayScene::GameplayScene()
	: m_pUserResources(nullptr)
	, m_pResources(nullptr)
	, m_gameTimer(0.0f)
	, m_fadeTimer(0.0f)
	, m_countDownTimer(0.0f)
{
}



/// <summary>
/// デストラクタ
/// </summary>
GameplayScene::~GameplayScene()
{
}



/// <summary>
/// 初期化処理
/// </summary>
void GameplayScene::Initialize()
{
	// ユーザーリソースの取得
	m_pUserResources = UserResources::GetUserResource();

	// リソースの取得
	m_pResources = Resources::GetInstance();

	// デバックフォントの初期化(シーンのみ)
	auto* debugFont = m_pUserResources->GetDebugFont();
	debugFont->Initialize();

	// カメラの初期化
	m_camera = std::make_unique<Camera>(m_pUserResources->GetDeviceResources()->GetOutputSize().bottom, m_pUserResources->GetDeviceResources()->GetOutputSize().right);

	// フィールドの初期化
	m_field = Factory::CreateField(m_camera.get(), Resources::GetInstance()->GetJson(L"FieldSelect.json")["FieldIndex"]);

	// カメラの上向きベクトルの初期化
	m_cameraUp = Factory::CreateCameraUp(m_field.get(), DirectX::SimpleMath::Vector3{ 4.0f,4.0f,4.0f });

	// スコアマネージャーの初期化
	m_scoreManager = Factory::CreateScoreManager();
	m_scoreManager->Add(m_field->GetPlayer()->GetScore());
	m_scoreManager->Add(m_field->GetEnemy()->GetScore());

	// ゲーム時間の初期化
	m_gameTimer = MAX_TIME;

	m_fadeTimer = 0.0f;

	m_countDownTimer = COUNTDOWN_TIME;

	// コライダーの設定
	m_collider.SetSize(DirectX::SimpleMath::Vector2(20.0f));

	// テクスチャの初期化
	m_frameTexture.SetTexture(m_pResources->GetTexture(L"ScoreFrame2.png"));
	m_timerTexture.SetTexture(m_pResources->GetTexture(L"ScoreFont2.png"));
	m_finishTexture.SetTexture(m_pResources->GetTexture(L"Finish.png"));
	m_countDownTexture.SetTexture(m_pResources->GetTexture(L"CountDown.png"));
	m_startTexture.SetTexture(m_pResources->GetTexture(L"GameStart.png"));

	// リスナーの設定
	m_pResources->SetListener(m_field->GetPlayer()->GetPosition(),
		DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitX, m_field->GetPlayer()->GetRotation()),
		DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitY, m_field->GetPlayer()->GetRotation())
	);

	// BGMの初期化
	m_bgm = m_pResources->GetBGMSound(L"GameBgm.wav", m_field->GetPlayer()->GetPosition(), true);

	// SEの初期化
	m_startSE = nullptr;
	m_finishSE = nullptr;

	// オーディオUIの初期化
	m_audioUI.Initialize();

	// ゲームメニューUIの初期化
	m_gameMenuUI.Initialize(&m_audioUI);

	// プレイ人数を初期化
	GetSceneManager()->SetPlayerCount(PLAYER_COUNT);

	// フェードをオープンする
	auto transitionMask = m_pUserResources->GetTransitionMask();
	transitionMask->Open();
}



/// <summary>
/// 更新処理
/// </summary>
/// <param name="elapsedTime"></param> 経過時間
void GameplayScene::Update(float elapsedTime)
{
	// BGMの音量の設定
	m_bgm->SetVolume(Resources::GetInstance()->GetBGMVolume());

	// マウスの座標に合わせる
	auto mouse = DirectX::Mouse::Get().GetState();
	// 現在のウィンドウサイズを取得
	auto const outputSize = m_pUserResources->GetDeviceResources()->GetOutputSize();
	float windowWidth = static_cast<float>(outputSize.right - outputSize.left);
	float windowHeight = static_cast<float>(outputSize.bottom - outputSize.top);

	// シーンの変更
	auto transitionMask = m_pUserResources->GetTransitionMask();

	// フェードアウト中じゃなかったら更新
	if (!transitionMask->IsClose()) m_collider.SetPosition(DirectX::SimpleMath::Vector2((mouse.x / windowWidth) * 1280.0f, (mouse.y / windowHeight) * 720.0f));

	if (m_gameTimer <= FINISH_TIME)
	{
		m_fadeTimer += elapsedTime;

		// フェードアウトする
		if (transitionMask->IsOpen() && m_fadeTimer >= FADE_TIME)
		{
			transitionMask->Close();
		}

		if (transitionMask->IsClose() && transitionMask->IsEnd())
		{
			ChangeScene<ResultScene>();
		}
		
		return;
	}

	// カウントダウンの更新
	m_countDownTimer -= elapsedTime;

	// カウントダウンが0～3秒以内なら更新させない
	if (m_countDownTimer > 0.0f && m_countDownTimer < 3.0f)
	{
		// SEをつける
        if(!m_startSE)	m_startSE = m_pResources->GetSESound(L"CountDown.wav", m_field->GetPlayer()->GetPosition(), false);

		return;
	}

	// キーボードトラッカーの取得
	auto kbTracker = m_pUserResources->GetKeyboardStateTracker();

	// エスケープキーが押されたらゲームメニューを開く
	if (kbTracker->pressed.Escape) m_gameMenuUI.Click();

	// オーディオUIの更新
	if (m_audioUI.IsOpen())
	{
		m_audioUI.Update(m_collider);
		return;
	}
	else if (m_gameMenuUI.IsOpen())
	{
		m_gameMenuUI.Update(m_collider);

		if (transitionMask->IsClose() && transitionMask->IsEnd())
		{
			ChangeScene<TitleScene>();
		}

		return;
	}

	// リスナーの設定
	SetListener();

	// カメラの更新
	m_camera->Update(m_field.get(), m_cameraUp->GetPosition());
	/*m_camera->DebugMode();*/

	// フィールドの更新
	m_field->Update(m_scoreManager.get(), elapsedTime);

	// プレイヤーの更新
	SetPlayerInputState();

	// カメラの上向きベクトルの更新
	m_cameraUp->Update(elapsedTime);

	// ポリゴンの当たり判定
	m_field->IsHitEntityToField(m_cameraUp.get());

	// ゲーム時間の更新
	m_gameTimer -= elapsedTime;

	// 0になったら終了
	if (m_gameTimer <= FINISH_TIME)
	{
		// SEをつける
		if (!m_finishSE)	m_finishSE = m_pResources->GetSESound(L"Finish.wav", m_field->GetPlayer()->GetPosition(), false);

		// ランキングの更新
		m_scoreManager->SortRank();
		GetSceneManager()->SetIsDraw(m_scoreManager->GetIsDraw());
		for (int i = 0; i < GetSceneManager()->GetPlayerCount(); i++)
		{
			GetSceneManager()->SetRank(i, m_scoreManager->GetRank(i));
		}
	}
}



/// <summary>
/// 描画処理
/// </summary>
void GameplayScene::Render()
{
	//// デバックフォントの描画
	//auto* debugFont = UserResources::GetUserResource()->GetDebugFont();
	//debugFont->Render(L"GameplayScene");

	// フィールドの描画
	m_field->Render();

	// スコアマネージャーの描画
	m_scoreManager->Render();

	// タイマーの描画
	m_frameTexture.Draw(FREAM.pos, FREAM.size, FREAM.scale);
	m_timerTexture.DigitsDraw(TIMER.pos.x, TIMER.pos.y, TIMER.size.x, TIMER.size.y, (int)m_gameTimer, TIMER.scale, 2);

	if (m_countDownTimer > 0.0f)
	{
		m_countDownTexture.DigitsDraw(COUNTDOWN.pos.x, COUNTDOWN.pos.y, COUNTDOWN.size.x, COUNTDOWN.size.y, (int)m_countDownTimer, COUNTDOWN.scale);
	}
	else if (m_countDownTimer >= -1.0f)
	{
		m_startTexture.Draw(START.pos, START.size, START.scale);
	}

	// オーディオUIの描画
	if (m_audioUI.IsOpen())
	{
		m_audioUI.Draw(m_collider);
	}
	// ゲームメニューUIの描画
	else if (m_gameMenuUI.IsOpen())
	{
		m_gameMenuUI.Draw(m_collider);
	}

	// フェード時間が増えていたら
	if (m_fadeTimer >= 0.1f)
	{
		m_finishTexture.Draw(FINISH.pos, FINISH.size, FINISH.scale);
	}

	// デバック用
	// カメラの上向きベクトルの描画
	/*m_cameraUp->Render();*/
}



/// <summary>
/// 終了処理
/// </summary>
void GameplayScene::Finalize()
{
	// フィールドの終了
	m_field->Finalize();

	// カメラの上向きベクトルの終了
	m_cameraUp->Finalize();
}



/// <summary>
/// デバイスに依存するリソースを作成する関数
/// </summary>
void GameplayScene::CreateDeviceDependentResources()
{
}



/// <summary>
/// ウインドウサイズに依存するリソースを作成する関数
/// </summary>
void GameplayScene::CreateWindowSizeDependentResources()
{
}



/// <summary>
/// デバイスロストした時に呼び出される関数
/// </summary>
void GameplayScene::OnDeviceLost()
{
}



/// <summary>
/// リスナーの設定
/// </summary>
void GameplayScene::SetListener()
{
	Player* player = m_field->GetPlayer();

	// 方向
	DirectX::SimpleMath::Vector3 dir = player->GetPosition() - m_cameraUp->GetPosition();
	dir.Normalize();

	// 方向ベクトルの反転
	DirectX::SimpleMath::Vector3 targetUp;
	targetUp = -dir;

	// 現在の姿勢制御
	DirectX::SimpleMath::Vector3 currentUp = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitX, player->GetRotation());

	// 回転軸の計算
	DirectX::SimpleMath::Vector3 axis = currentUp.Cross(targetUp);
	axis.Normalize();

	// 回転角の計算
	float dot = currentUp.Dot(targetUp);
	float angle = acosf(dot);

	// クォータニオンの作成
	DirectX::SimpleMath::Quaternion q;

	// 角度が少しでもあれば軸を作る
	if (angle > 0.01f)
	{
		q = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(axis, angle);
	}
	// なければ何もしない
	else
	{
		q = DirectX::SimpleMath::Quaternion::Identity;
	}

	q = player->GetRotation() * q;

	// リスナーの設定
	m_pResources->SetListener(player->GetPosition(),
		DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitX, q),
		DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitY, player->GetRotation())
	);
}



/// <summary>
/// 入力ステートの設定
/// </summary>
void GameplayScene::SetPlayerInputState()
{
	auto kb = DirectX::Keyboard::Get().GetState();
	auto mouseTK = m_pUserResources->GetMouseStateTracker();

	// イベントのキー
	std::vector<IState::Event> e;

	// Wキーで走る
	if (kb.W)
	{
		e.push_back(IState::Event::RUN);
	}
	// 右クリックでキャッチ
	if (mouseTK->rightButton == mouseTK->PRESSED)
	{
		e.push_back(IState::Event::CATCH);
	}
	// 左クリックで投げる
	if (mouseTK->leftButton == mouseTK->PRESSED)
	{
		e.push_back(IState::Event::THROW);
	}

	// 何もなかったら立ち状態にする
	if (e.size() == 0)
	{
		e.push_back(IState::Event::STAND);
	}

	// イベントを渡す
	m_field->GetPlayer()->OnEvents(e);
}
