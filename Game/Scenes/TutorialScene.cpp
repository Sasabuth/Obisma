/// <summary>
/// Gameplayシーンに関するソースファイル
/// </summary>
/// <author>仲森智史</author>


// ヘッダファイルの読み込み
#include "pch.h"
#include "TutorialScene.h"

#include "Game/Scenes/ResultScene.h"
#include "Game/Scenes/TitleScene.h"
#include "Game/Commons/Factory.h"
#include "Game/Commons/Resources.h"



/// <summary>
/// コンストラクタ
/// </summary>
TutorialScene::TutorialScene()
	: m_pUserResources(nullptr)
	, m_pResources(nullptr)
	, m_interval(0)
	, m_count(0)
	, m_tutorialIndex(MOUSE_MOVE)
	, m_explainIndex(EXPLAINORDER::SCORE_UP)
	, m_isCheck(false)
	, m_isRightBall(false)
{
}



/// <summary>
/// デストラクタ
/// </summary>
TutorialScene::~TutorialScene()
{
}



/// <summary>
/// 初期化処理
/// </summary>
void TutorialScene::Initialize()
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
	m_field = Factory::CreateTutorialField(m_camera.get(), 0);

	// カメラの上向きベクトルの初期化
	m_cameraUp = Factory::CreateCameraUp(m_field.get(), DirectX::SimpleMath::Vector3{ 2.0f,2.0f,2.0f });

	// 矢印の生成
	m_arrow = Factory::CreateArrow(m_field->GetPlayer(), DirectX::SimpleMath::Vector3{ 2.0f,2.0f,2.0f });


	// スコアマネージャーの初期化
	m_scoreManager = Factory::CreateScoreManager();
	m_scoreManager->Add(m_field->GetPlayer()->GetScore());
	m_scoreManager->Add(m_field->GetEnemy()->GetScore());

	// ゲーム時間の初期化
	m_interval = 0.0f;

	// テクスチャの初期化
	m_frameTexture.SetTexture(m_pResources->GetTexture(L"ScoreFrame2.png"));
	m_timerTexture.SetTexture(m_pResources->GetTexture(L"ScoreFont2.png"));
	
	m_checkMarkTexture.SetTexture(m_pResources->GetTexture(L"CheckMark.png"));

	// リスナーの設定
	m_pResources->SetListener(m_field->GetPlayer()->GetPosition(),
		DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitX, m_field->GetPlayer()->GetRotation()),
		DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitY, m_field->GetPlayer()->GetRotation())
	);

	// カウントの初期化
	m_count = -1;

	// チュートリアル番号の初期化
	m_tutorialIndex = ORDER::MOUSE_MOVE;
	m_tutorialTexture.SetTexture(m_pResources->GetTexture(L"Tutorial" + std::to_wstring(m_tutorialIndex) + L".png"));

	// 説明番号の初期化
	m_explainIndex = EXPLAINORDER::SCORE_UP;
	m_explainTexture.SetTexture(nullptr);

	// 警告テクスチャの設定
	m_warningTexture.SetTexture(nullptr);

	// チェックできない
	m_isCheck = false;

	// 右のボールを取っているか
	m_isRightBall = false;

	// BGM
	m_bgm = m_pResources->GetBGMSound(L"GameBgm.wav", m_field->GetPlayer()->GetPosition(), true);

	// オーディオUIの初期化
	m_audioUI.Initialize();

	// ゲームメニューUIの初期化
	m_gameMenuUI.Initialize(&m_audioUI);

	// コライダーの設定
	m_collider.SetSize(DirectX::SimpleMath::Vector2(20.0f));

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
void TutorialScene::Update(float elapsedTime)
{
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

	// カメラの上向きベクトルの更新
	m_cameraUp->Update(elapsedTime);

	// カメラの更新
	m_camera->Update(m_field.get(), m_cameraUp->GetPosition());
	/*m_camera->DebugMode();*/

	m_field->TutorialUpdate(this, elapsedTime);

	SetPlayerInputState();

	// 矢印の更新
	m_arrow->Update(elapsedTime);

	m_field->IsHitEntityToField(m_cameraUp.get());
	m_field->IsHitEntityToField(m_arrow.get());

	// 警告のテクスチャの設定
	DirectX::SimpleMath::Vector3 hitPos;
	if (!m_field->GetPlayer()->CalcRaySphere(m_field->GetCollider().GetPosition(), m_field->GetCollider().GetRadius(), hitPos) && !m_field->GetPlayer()->GetIsLockOn())
	{
		m_warningTexture.SetTexture(m_pResources->GetTexture(L"Warning.png"));
	}
	else
	{
		m_warningTexture.SetTexture(nullptr);
	}

	// チュートリアルシーンに変更
	if (transitionMask->IsClose() && transitionMask->IsEnd())
	{
		ChangeScene<TitleScene>();
	}

	// BGMの音量の設定
	m_bgm->SetVolume(m_pResources->GetBGMVolume());
}



/// <summary>
/// 描画処理
/// </summary>
void TutorialScene::Render()
{
	// デバックフォントの描画
	auto* debugFont = UserResources::GetUserResource()->GetDebugFont();
	debugFont->Render(L"TutorialScene");

	// フィールドの描画
	m_field->Render();

	// 矢印の描画
	if (m_arrow->GetIsDraw())
	{
		m_arrow->Render();
	}

	// スコアマネージャーの描画
	m_scoreManager->Render();

	// タイマーの描画
	m_frameTexture.Draw(FREAM.pos, FREAM.size, FREAM.scale);
	m_timerTexture.DigitsDraw(TIMER.pos.x, TIMER.pos.y, TIMER.size.x, TIMER.size.y, MAX_TIME, TIMER.scale, 2);

	// 説明のテクスチャがなかったらチュートリアルを描画
	if (!m_explainTexture.GetTexture())
	{
		if (m_tutorialIndex >= ORDER::MOUSE_MOVE && m_tutorialIndex < ORDER::MAX_ORDERCOUNT)
		{
			m_tutorialTexture.Draw(TUTORIAL[m_tutorialIndex].pos, TUTORIAL[m_tutorialIndex].size, TUTORIAL[m_tutorialIndex].scale);

			// 警告のテクスチャの描画
			if (m_warningTexture.GetTexture())
			{
				m_warningTexture.Draw(DirectX::SimpleMath::Vector2(WARNING.pos.x, TUTORIAL[m_tutorialIndex].pos.y + WARNING.pos.y), WARNING.size, WARNING.scale);
			}
		}
	}
	// 説明の描画
	else
	{
		m_explainTexture.Draw(EXPLAIN[m_explainIndex].pos, EXPLAIN[m_explainIndex].size, EXPLAIN[m_explainIndex].scale);

		// 警告のテクスチャの描画
		if (m_warningTexture.GetTexture())
		{
			m_warningTexture.Draw(DirectX::SimpleMath::Vector2(WARNING.pos.x, EXPLAIN[m_explainIndex].pos.y + WARNING.pos.y), WARNING.size, WARNING.scale);
		}
	}
	
	// チェックが付いたら描画
	if (m_isCheck)
	{
		m_checkMarkTexture.Draw(CHECKMARK.pos, CHECKMARK.size, CHECKMARK.scale);
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
	

	// デバック用
	// カメラの上向きベクトルの描画
	/*m_cameraUp->Render();*/
}



/// <summary>
/// 終了処理
/// </summary>
void TutorialScene::Finalize()
{
	// フィールドの終了
	m_field->Finalize();

	// カメラの上向きベクトルの終了
	m_cameraUp->Finalize();

	// 矢印の終了
	m_arrow->Finalize();
}



/// <summary>
/// デバイスに依存するリソースを作成する関数
/// </summary>
void TutorialScene::CreateDeviceDependentResources()
{
}



/// <summary>
/// ウインドウサイズに依存するリソースを作成する関数
/// </summary>
void TutorialScene::CreateWindowSizeDependentResources()
{
}



/// <summary>
/// デバイスロストした時に呼び出される関数
/// </summary>
void TutorialScene::OnDeviceLost()
{
}



/// <summary>
/// チュートリアル
/// </summary>
void TutorialScene::Tutorial(float elapsedTime)
{
	// プレイヤーの取得
	Player* player = m_field->GetPlayer();
	// 敵の取得
	Enemy* enemy = m_field->GetEnemy();
	// ボールマネージャーの取得
	BallManager* ballManager = m_field->GetBallManager();
	// 空中の的の取得
	AirTarget* airTarget = m_field->GetAirTarget();

	// チュートリアル番号で分ける
	switch (m_tutorialIndex)
	{
	// マウスを動かす
	case TutorialScene::MOUSE_MOVE:
	{
		// マウスの取得
		auto mouse = DirectX::Mouse::Get().GetState();
		// 最初のマウス座標を保存
		static DirectX::SimpleMath::Vector2 pos = DirectX::SimpleMath::Vector2((float)mouse.x, (float)mouse.y);

		// 行列の取得
		auto proj = m_pUserResources->GetProject();
		auto view = m_pUserResources->GetView();

		// レイの設定
		auto const r = m_pUserResources->GetDeviceResources()->GetOutputSize();
		player->SetMouseRay(player->CreatePickingRay(mouse.x, mouse.y, r.right, r.bottom, *view, *proj));

		// フィールドとマウスレイが当たっていたらプレイヤーを回転
		if (player->CalcRaySphere(m_field->GetCollider().GetPosition(), m_field->GetCollider().GetRadius(), player->GetMouseRayHitPos()))
		{
			player->RotateToMouse();

			// プレイヤーの設定
			player->SetVelocity(player->GetGravity());
			player->SetPosition(player->GetPosition() + player->GetVelocity() * elapsedTime);
			player->GetCollider().SetPosition(player->GetPosition());

			// マウスの移動距離の計算
			DirectX::SimpleMath::Vector2 dir = DirectX::SimpleMath::Vector2((float)mouse.x, (float)mouse.y) - pos;

			// 長さが上限になったら座標を更新してカウントを増やす
			if (dir.Length() >= MAX_LENGTH)
			{
				m_count += 1;
				pos = DirectX::SimpleMath::Vector2((float)mouse.x, (float)mouse.y);
			}
		}

		// カウントが上限に行ったらチェックマークをつける
		if (m_count >= MAX_COUNT)
		{
			m_interval += elapsedTime;
			m_isCheck = true;
		}

		// インターバルの時間が上限に行ったら次のチュートリアルに進む
		if (m_interval >= INTERVAL)
		{
			m_isCheck = false;
			m_arrow->SetIsDraw(true);
			m_tutorialIndex = PLAYER_MOVE;
			m_tutorialTexture.SetTexture(m_pResources->GetTexture(L"Tutorial" + std::to_wstring(m_tutorialIndex) + L".png"));
			m_count = 0;
			m_interval = 0.0f;
		}
	}
	break;

	// プレイヤーの移動
	case TutorialScene::PLAYER_MOVE:
	{
		// プレイヤーが矢印についたらチェックマークをつける
		if (IsHit(player->GetCollider(), m_arrow->GetCollider()))
		{
			m_isCheck = true;
		}

		// チェックマークがついたら矢印を消す
		if (m_isCheck)
		{
			m_arrow->SetIsDraw(false);
			m_interval += elapsedTime;
		}

		// インターバルの時間が上限に行ったら次のチュートリアルに進む
		if (m_interval >= INTERVAL)
		{
			m_isCheck = false;
			m_tutorialIndex = BALL_PICKUP;
			m_tutorialTexture.SetTexture(m_pResources->GetTexture(L"Tutorial" + std::to_wstring(m_tutorialIndex) + L".png"));
			m_interval = 0.0f;

			ballManager->GetBall(0)->SetPosition(DirectX::SimpleMath::Vector3{
				m_pResources->GetJson(L"Ball.json")["Position"]["0"]["0"]["x"],
				m_pResources->GetJson(L"Ball.json")["Position"]["0"]["0"]["y"],
				m_pResources->GetJson(L"Ball.json")["Position"]["0"]["0"]["z"]
				}
			);
		}
	}
	break;

	// ボールを拾う
	case TutorialScene::BALL_PICKUP:
	{
		// 左手にボールを持ったらチェックマークをつける
		if (player->GetCatchBall(Player::HAND::RIGHT) && !player->GetCatchBall(Player::HAND::LEFT) && !m_isRightBall)
		{
			ballManager->GetBall(1)->SetPosition(DirectX::SimpleMath::Vector3{
					m_pResources->GetJson(L"Ball.json")["Position"]["0"]["1"]["x"],
					m_pResources->GetJson(L"Ball.json")["Position"]["0"]["1"]["y"],
					m_pResources->GetJson(L"Ball.json")["Position"]["0"]["1"]["z"]
				}
			);
			
			m_isRightBall = true;
		}

		// 左手にボールを持ったらチェックマークをつける
		if (player->GetCatchBall(Player::HAND::LEFT))
		{
			static DirectX::SimpleMath::Vector3 pos = player->GetCatchBall(Player::HAND::LEFT)->GetPosition();
			m_isCheck = true;
			player->SetPosition(pos);
		}

		// チェックマークがついたらインターバルの更新
		if (m_isCheck)
		{
			m_interval += elapsedTime;
		}

		// インターバルの時間が上限に行ったら次のチュートリアルに進む
		if (m_interval >= INTERVAL)
		{
			m_isRightBall = false;
			m_isCheck = false;
			m_tutorialIndex = MOUSE_TO_STER;
			m_tutorialTexture.SetTexture(m_pResources->GetTexture(L"Tutorial" + std::to_wstring(m_tutorialIndex) + L".png"));
			m_interval = 0.0f;

			// 空中の的の座標の設定
			airTarget->SetPosition(DirectX::SimpleMath::Vector3{
				m_pResources->GetJson(L"AirTarget.json")["TutorialPos2"]["x"],
				m_pResources->GetJson(L"AirTarget.json")["TutorialPos2"]["y"],
				m_pResources->GetJson(L"AirTarget.json")["TutorialPos2"]["z"]
				}
			);
			airTarget->SetGravity(m_field->CorrectUp(airTarget));
			m_field->IsHitEntityToField(airTarget);
		}
	}
	break;

	// マウスを星に近づける
	case TutorialScene::MOUSE_TO_STER:
	{
		

		// マウスの取得
		auto mouse = DirectX::Mouse::Get().GetState();
		// 行列の取得
		auto proj = m_pUserResources->GetProject();
		auto view = m_pUserResources->GetView();

		// レイの設定
		auto const r = m_pUserResources->GetDeviceResources()->GetOutputSize();
		player->SetMouseRay(player->CreatePickingRay(mouse.x, mouse.y, r.right, r.bottom, *view, *proj));

		// マウスレイが空中の的に当たったらチェックマークをつける
		if (player->CalcRaySphere(airTarget->GetPosition(), airTarget->GetCollider().GetRadius(), player->GetMouseRayHitPos()))
		{
			m_isCheck = true;
		}

		// チェックマークがついたらインターバルの更新
		if (m_isCheck)
		{
			m_interval += elapsedTime;
		}

		// インターバルの時間が上限に行ったら次のチュートリアルに進む
		if (m_interval >= INTERVAL)
		{
			m_isCheck = false;
			m_tutorialIndex = BALL_THROW;
			m_tutorialTexture.SetTexture(m_pResources->GetTexture(L"Tutorial" + std::to_wstring(m_tutorialIndex) + L".png"));
			m_interval = 0.0f;
		}
	}
	break;

	// ボールを投げる
	case TutorialScene::BALL_THROW:
	{
		// いずれかのボールが空中の的に当たったらチェックマークをつける
		for (int i = 0; i < ballManager->GetObjectCount(); i++)
		{
			if (IsHit(ballManager->GetBall(i)->GetCollider(), airTarget->GetCollider()))
			{
				m_isCheck = true;
			}
		}

		// チェックマークがついたら説明のテクスチャをつける
		if (m_isCheck)
		{
			m_interval += elapsedTime;
			m_explainIndex = EXPLAINORDER::SCORE_UP;
			m_explainTexture.SetTexture(m_pResources->GetTexture(L"Explain" + std::to_wstring(m_explainIndex) + L".png"));
		}

		// インターバルの時間が上限に行ったら次のチュートリアルに進む
		if (m_interval >= EXPLAIN_INTERVAL)
		{
			m_isCheck = false;
			m_tutorialIndex = BALL_CATCH;
			m_tutorialTexture.SetTexture(m_pResources->GetTexture(L"Tutorial" + std::to_wstring(m_tutorialIndex) + L".png"));
			m_interval = 0.0f;
			m_explainTexture.SetTexture(nullptr);

			// プレイヤーはボールを持たないようにする
			player->SetCatchBall(Player::HAND::LEFT, nullptr);
			player->SetCatchBall(Player::HAND::RIGHT, nullptr);
			player->ChangeState(player->GetStanding());

			// ボールを触れないように高い所に置く
			for (int i = 0; i < ballManager->GetObjectCount(); i++)
			{
				ballManager->GetBall(i)->SetPosition(DirectX::SimpleMath::Vector3{
						m_pResources->GetJson(L"Ball.json")["TutorialPos"]["x"],
						m_pResources->GetJson(L"Ball.json")["TutorialPos"]["y"],
						m_pResources->GetJson(L"Ball.json")["TutorialPos"]["z"]
					}
				);

				ballManager->GetBall(i)->GetCollider().SetPosition(ballManager->GetBall(i)->GetPosition());
			}

			// ボールを止める状態にして敵に持たせる
			ballManager->GetBall(0)->ChangeState(ballManager->GetBall(0)->GetStopping());
			ballManager->GetBall(0)->SetPosition(enemy->GetPosition());

			// 描画されないように空中の的を高い所に置く
			airTarget->SetPosition(DirectX::SimpleMath::Vector3{
				   m_pResources->GetJson(L"AirTarget.json")["TutorialPos"]["x"],
				   m_pResources->GetJson(L"AirTarget.json")["TutorialPos"]["y"],
				   m_pResources->GetJson(L"AirTarget.json")["TutorialPos"]["z"]
				}
			);
		}
	}
	break;

	// ボールをキャッチ
	case TutorialScene::BALL_CATCH:
	{
		// 当たったかの判定
		static bool isHit = false;

		// プレイヤーがボールをキャッチしたらチェックマークをつける
		if (player->GetCatchBall(Player::HAND::RIGHT))
		{
			m_isCheck = true;
		}
		else
		{
			// 敵のボールに当たったら当たった判定をつける
			if (IsHit(ballManager->GetBall(0)->GetCollider(), player->GetCollider()) && ballManager->GetBall(0)->GetCurrentState() == ballManager->GetBall(0)->GetMoving())
			{
				isHit = true;
			}
		}

		// チェックマークか当たった判定がついたら説明のテクスチャをつける
		if (m_isCheck || isHit)
		{
			m_interval += elapsedTime;
			m_explainIndex = EXPLAINORDER::SCORE_DOWN;
			m_explainTexture.SetTexture(m_pResources->GetTexture(L"Explain" + std::to_wstring(m_explainIndex) + L".png"));
		}

		// インターバルの時間が上限に行ったら
		if (m_interval >= EXPLAIN_INTERVAL)
		{
			// チェックが付いたらフェードする
			if (m_isCheck)
			{
				auto transitionMask = m_pUserResources->GetTransitionMask();
				// フェードアウトする
				if (transitionMask->IsOpen())
				{
					transitionMask->Close();
				}
				return;
			}

			isHit = false;
			m_tutorialTexture.SetTexture(m_pResources->GetTexture(L"Tutorial" + std::to_wstring(m_tutorialIndex) + L".png"));
			m_interval = 0.0f;
			m_explainTexture.SetTexture(nullptr);
		}
	}
	break;
	}
	
}



/// <summary>
/// リスナーの設定
/// </summary>
void TutorialScene::SetListener()
{
	// プレイヤーの取得
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
void TutorialScene::SetPlayerInputState()
{
	auto kb = DirectX::Keyboard::Get().GetState();
	auto mouseTK = m_pUserResources->GetMouseStateTracker();

	// イベントのキー
	std::vector<IState::Event> e;

	// Wキーで走る
	if (kb.W)
	{
		// プレイヤーの更新
		if (m_tutorialIndex != ORDER::MOUSE_MOVE && m_tutorialIndex != ORDER::MOUSE_TO_STER)
		{
			e.push_back(IState::Event::RUN);
		}
	}
	// 右クリックでキャッチ
	if (mouseTK->rightButton == mouseTK->PRESSED && m_tutorialIndex == ORDER::BALL_CATCH)
	{
		e.push_back(IState::Event::CATCH);
	}
	// 左クリックで投げる
	if (mouseTK->leftButton == mouseTK->PRESSED && m_tutorialIndex == TutorialScene::BALL_THROW)
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
