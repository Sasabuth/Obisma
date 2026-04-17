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
#include "Game/Commons/Messenger.h"
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

	// ゲームの初期化
	InitializeGame();

	// リソースの初期化
	InitializeResource();
}



/// <summary>
/// 更新処理
/// </summary>
/// <param name="elapsedTime"></param> 経過時間
void TutorialScene::Update(float elapsedTime)
{
	// UIの更新で止めたいときがあったら更新しない
	if (UpdateUI())
	{
		return;
	}

	// プレイヤーの取得
	Player* player = dynamic_cast<Player*>(Messenger::GetInstance()->GetObject(Factory::PLAYER));

	// リスナーの設定
	SetListener(player);

	// カメラの上向きベクトルの更新
	m_cameraUp->Update(elapsedTime);

	// カメラの更新
	m_camera->Update(m_field.get(), m_cameraUp->GetPosition());
	/*m_camera->DebugMode();*/

	// チュートリアルの更新
	Tutorial(player, elapsedTime);

	// フィールドの更新
	m_field->TutorialUpdate(this, m_scoreManager.get(), elapsedTime);

	// 入力ステートの設定
	SetPlayerInputState(player);

	// 矢印の更新
	m_arrow->Update(elapsedTime);

	// フィールドとの衝突解決
	m_field->ResolveEntityFieldCollision(m_cameraUp.get());
	m_field->ResolveEntityFieldCollision(m_arrow.get());
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
		}
	}
	// 説明の描画
	else
	{
		m_explainTexture.Draw(EXPLAIN[m_explainIndex].pos, EXPLAIN[m_explainIndex].size, EXPLAIN[m_explainIndex].scale);
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
void TutorialScene::Tutorial(Player* player, float elapsedTime)
{
	// 敵の取得
	Enemy* enemy = dynamic_cast<Enemy*>(Messenger::GetInstance()->GetObject(Factory::ENEMY));

	// 空中の的の取得
	AirTarget* airTarget = dynamic_cast<AirTarget*>(Messenger::GetInstance()->GetObject(Factory::AIRTARGET));

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

		// マウスの移動距離の計算
		DirectX::SimpleMath::Vector2 dir = DirectX::SimpleMath::Vector2((float)mouse.x, (float)mouse.y) - pos;

		// 長さが上限になったら座標を更新してカウントを増やす
		if (dir.Length() >= MAX_LENGTH)
		{
			m_count += 1;
			pos = DirectX::SimpleMath::Vector2((float)mouse.x, (float)mouse.y);
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

			dynamic_cast<Ball*>(Messenger::GetInstance()->GetObject(Factory::BALL))->SetPosition(DirectX::SimpleMath::Vector3{
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
			dynamic_cast<Ball*>(Messenger::GetInstance()->GetObject(Factory::BALL + 1))->SetPosition(DirectX::SimpleMath::Vector3{
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
			m_field->ResolveEntityFieldCollision(airTarget);
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
		for (int i = 0; i < Resources::GetInstance()->GetJson(L"Ball.json")["BallCount"]; i++)
		{
			// ボールの取得
			Ball* ball = dynamic_cast<Ball*>(Messenger::GetInstance()->GetObject(Factory::BALL + i));
			if (IsHit(ball->GetCollider(), airTarget->GetCollider()))
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

			for (int i = 0; i < Resources::GetInstance()->GetJson(L"Ball.json")["BallCount"]; i++)
			{
				// ボールの取得
				Ball* ball = dynamic_cast<Ball*>(Messenger::GetInstance()->GetObject(Factory::BALL + i));
				ball->SetPosition(DirectX::SimpleMath::Vector3{
						m_pResources->GetJson(L"Ball.json")["TutorialPos"]["x"],
						m_pResources->GetJson(L"Ball.json")["TutorialPos"]["y"],
						m_pResources->GetJson(L"Ball.json")["TutorialPos"]["z"]
					}
				);

				ball->GetCollider().SetPosition(ball->GetPosition());
			}

			// ボールを止める状態にして敵に持たせる
			dynamic_cast<Ball*>(Messenger::GetInstance()->GetObject(Factory::BALL))->ChangeState(dynamic_cast<Ball*>(Messenger::GetInstance()->GetObject(Factory::BALL))->GetStopping());
			dynamic_cast<Ball*>(Messenger::GetInstance()->GetObject(Factory::BALL))->SetPosition(enemy->GetPosition());

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
			if (IsHit(dynamic_cast<Ball*>(Messenger::GetInstance()->GetObject(Factory::BALL))->GetCollider(), player->GetCollider()) && dynamic_cast<Ball*>(Messenger::GetInstance()->GetObject(Factory::BALL))->GetCurrentState() == dynamic_cast<Ball*>(Messenger::GetInstance()->GetObject(Factory::BALL))->GetMoving())
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
/// ゲームの初期化
/// </summary>
void TutorialScene::InitializeGame()
{
	// カメラの初期化
	m_camera = std::make_unique<Camera>(m_pUserResources->GetDeviceResources()->GetOutputSize().bottom, m_pUserResources->GetDeviceResources()->GetOutputSize().right);

	// フィールドの初期化
	m_field = Factory::CreateTutorialField(0);

	// カメラの上向きベクトルの初期化
	m_cameraUp = Factory::CreateCameraUp(m_field.get(), DirectX::SimpleMath::Vector3{
		Resources::GetInstance()->GetJson(L"CameraUp.json")["TutorialPos"]["x"],
		Resources::GetInstance()->GetJson(L"CameraUp.json")["TutorialPos"]["y"],
		Resources::GetInstance()->GetJson(L"CameraUp.json")["TutorialPos"]["z"]
		}
	);

	// プレイヤーの取得
	Player* player = dynamic_cast<Player*>(Messenger::GetInstance()->GetObject(Factory::PLAYER));
	// 敵の取得
	Enemy* enemy = dynamic_cast<Enemy*>(Messenger::GetInstance()->GetObject(Factory::ENEMY));

	// 矢印の生成
	m_arrow = Factory::CreateArrow(DirectX::SimpleMath::Vector3{
		Resources::GetInstance()->GetJson(L"Arrow.json")["Position"]["x"],
		Resources::GetInstance()->GetJson(L"Arrow.json")["Position"]["y"],
		Resources::GetInstance()->GetJson(L"Arrow.json")["Position"]["z"]
		}
	);

	// スコアマネージャーの初期化
	m_scoreManager = Factory::CreateScoreManager();
	// スコアマネージャーに追加
	m_scoreManager->Add(player->GetScore());
	m_scoreManager->Add(enemy->GetScore());

	// ゲーム時間の初期化
	m_interval = 0.0f;

	// カウントの初期化
	m_count = -1;

	// コライダーの設定
	m_collider.SetSize(DirectX::SimpleMath::Vector2(Resources::GetInstance()->GetJson(L"Mouse.json")["Collider"]));

	// プレイ人数を初期化
	GetSceneManager()->SetPlayerCount(PLAYER_COUNT);
}



/// <summary>
/// リソースの初期化
/// </summary>
void TutorialScene::InitializeResource()
{
	// デバックフォントの初期化(シーンのみ)
	auto* debugFont = m_pUserResources->GetDebugFont();
	debugFont->Initialize();

	// テクスチャの初期化
	m_frameTexture.SetTexture(m_pResources->GetTexture(L"ScoreFrame2.png"));
	m_timerTexture.SetTexture(m_pResources->GetTexture(L"ScoreFont2.png"));
	m_checkMarkTexture.SetTexture(m_pResources->GetTexture(L"CheckMark.png"));

	// プレイヤーの取得
	Player* player = dynamic_cast<Player*>(Messenger::GetInstance()->GetObject(Factory::PLAYER));

	// リスナーの設定
	m_pResources->SetListener(player->GetPosition(),
		DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitX, player->GetRotation()),
		DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitY, player->GetRotation())
	);

	// チュートリアル番号の初期化
	m_tutorialIndex = ORDER::MOUSE_MOVE;
	m_tutorialTexture.SetTexture(m_pResources->GetTexture(L"Tutorial" + std::to_wstring(m_tutorialIndex) + L".png"));

	// 説明番号の初期化
	m_explainIndex = EXPLAINORDER::SCORE_UP;
	m_explainTexture.SetTexture(nullptr);

	// チェックできない
	m_isCheck = false;

	// 右のボールを取っているか
	m_isRightBall = false;

	// BGM
	m_bgm = m_pResources->GetBGMSound(L"GameBgm.wav", player->GetPosition(), true);

	// オーディオUIの初期化
	m_audioUI.Initialize();

	// ゲームメニューUIの初期化
	m_gameMenuUI.Initialize(&m_audioUI);

	// フェードをオープンする
	auto transitionMask = m_pUserResources->GetTransitionMask();
	transitionMask->Open();
}



/// <summary>
/// UIの更新
/// </summary>
/// <param name="elapsedTime">経過時間</param>
/// <returns>止めたいか</returns>
bool TutorialScene::UpdateUI()
{
	// BGMの音量の設定
	m_bgm->SetVolume(m_pResources->GetBGMVolume());

	// マウスの座標に合わせる
	auto mouse = DirectX::Mouse::Get().GetState();
	// 現在のウィンドウサイズを取得
	auto const outputSize = m_pUserResources->GetDeviceResources()->GetOutputSize();
	float windowWidth = static_cast<float>(outputSize.right - outputSize.left);
	float windowHeight = static_cast<float>(outputSize.bottom - outputSize.top);

	// シーンの変更
	auto transitionMask = m_pUserResources->GetTransitionMask();

	// フェードアウト中じゃなかったら更新
	if (!transitionMask->IsClose()) m_collider.SetPosition(DirectX::SimpleMath::Vector2((mouse.x / windowWidth) * Sprite::BASE_WIDTH, (mouse.y / windowHeight) * Sprite::BASE_HEIGHT));

	// チュートリアルシーンに変更
	if (transitionMask->IsClose() && transitionMask->IsEnd())
	{
		ChangeScene<TitleScene>();
	}

	// キーボードトラッカーの取得
	auto kbTracker = m_pUserResources->GetKeyboardStateTracker();

	// エスケープキーが押されたらゲームメニューを開く
	if (kbTracker->pressed.Escape) m_gameMenuUI.Click();

	// オーディオUIの更新
	if (m_audioUI.IsOpen())
	{
		m_audioUI.Update(m_collider);
		return true;
	}
	// ゲームメニューUIの更新
	else if (m_gameMenuUI.IsOpen())
	{
		m_gameMenuUI.Update(m_collider);

		return true;
	}

	return false;
}



/// <summary>
/// リスナーの設定
/// </summary>
/// <param name="player">プレイヤー</param>
void TutorialScene::SetListener(Player* player)
{
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
/// <param name="player">プレイヤー</param>
void TutorialScene::SetPlayerInputState(Player* player)
{
	auto kb = DirectX::Keyboard::Get().GetState();
	auto mouseTK = m_pUserResources->GetMouseStateTracker();

	// Wキーを押したら
	if (kb.W && m_tutorialIndex != ORDER::MOUSE_MOVE && m_tutorialIndex != ORDER::MOUSE_TO_STER)
	{
		// プレイヤーに対して「走る」状態に遷移する
		Messenger::GetInstance()->Notify(Factory::PLAYER, Message::RUNNING);
	}
	// 何もなかったら
	else
	{
		// プレイヤーに対して「立つ」状態に遷移する
		Messenger::GetInstance()->Notify(Factory::PLAYER, Message::STANDING);
	}

	// 左クリックを押したら
	if (mouseTK->leftButton == mouseTK->PRESSED && player->IsThrow() && m_tutorialIndex == TutorialScene::BALL_THROW)
	{
		// プレイヤーに対して「投げる」状態に遷移する
		Messenger::GetInstance()->NotifyAfterDelay(Factory::PLAYER, Message::THROWING, m_pResources->GetJson(L"Player.json")["ThrowingEndTime"]);
	}
	// 右クリックを押したら
	if (mouseTK->rightButton == mouseTK->PRESSED && m_tutorialIndex == ORDER::BALL_CATCH)
	{
		// プレイヤーに対して「キャッチ」状態に遷移する
		Messenger::GetInstance()->NotifyAfterDelay(Factory::PLAYER, Message::CATCHING, m_pResources->GetJson(L"Player.json")["CatchingEndTime"]);
	}
}
