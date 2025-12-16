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

	// フィールドの初期化
	m_field = Factory::CreateField(0);

	// カメラの初期化
	m_camera = std::make_unique<Camera>(m_pUserResources->GetDeviceResources()->GetOutputSize().bottom, m_pUserResources->GetDeviceResources()->GetOutputSize().right);

	// ボールマネージャーの初期化
	m_ballManager = Factory::CreateBallManager(m_field.get(), Resources::GetInstance()->GetJson(L"Ball.json")["TutorialCount"]);
	for (int i = 0; i < m_ballManager->GetObjectCount(); i++)
	{
		m_ballManager->GetBall(i)->SetPosition(DirectX::SimpleMath::Vector3{
		m_pResources->GetJson(L"Ball.json")["TutorialPos"]["x"],
		m_pResources->GetJson(L"Ball.json")["TutorialPos"]["y"],
		m_pResources->GetJson(L"Ball.json")["TutorialPos"]["z"]
			}
		);
	}

	// 空中の的の初期化
	m_airTarget = Factory::CreateAirTarget(m_field.get(), m_camera.get(), DirectX::SimpleMath::Vector3{
		m_pResources->GetJson(L"AirTarget.json")["TutorialPos"]["x"],
		m_pResources->GetJson(L"AirTarget.json")["TutorialPos"]["y"],
		m_pResources->GetJson(L"AirTarget.json")["TutorialPos"]["z"]
		}
	);

	// プレイヤーの初期化
	m_player = Factory::CreatePlayer(m_field.get(), m_airTarget.get(), m_ballManager.get(), DirectX::SimpleMath::Vector3{
		m_pResources->GetJson(L"Player.json")["Position"]["x"],
		m_pResources->GetJson(L"Player.json")["Position"]["y"],
		m_pResources->GetJson(L"Player.json")["Position"]["z"]
		}
	);

	// 敵の初期化
	m_enemy = Factory::CreateEnemy(m_player.get(), m_field.get(), m_airTarget.get(), m_ballManager.get(), DirectX::SimpleMath::Vector3{
		m_pResources->GetJson(L"Enemy.json")["Position"]["x"],
		m_pResources->GetJson(L"Enemy.json")["Position"]["y"],
		m_pResources->GetJson(L"Enemy.json")["Position"]["z"]
		}
	);

	// カメラの上向きベクトルの初期化
	m_cameraUp = Factory::CreateCameraUp(m_player.get(), DirectX::SimpleMath::Vector3{ 2.0f,2.0f,2.0f });

	// 矢印の生成
	m_arrow = Factory::CreateArrow(m_player.get(), DirectX::SimpleMath::Vector3{ 2.0f,2.0f,2.0f });


	// スコアマネージャーの初期化
	m_scoreManager = Factory::CreateScoreManager();
	m_scoreManager->Add(m_player->GetScore());
	m_scoreManager->Add(m_enemy->GetScore());

	// ゲーム時間の初期化
	m_interval = 0.0f;

	// テクスチャの初期化
	m_frameTexture.SetTexture(m_pResources->GetTexture(L"ScoreFrame2.png"));
	m_timerTexture.SetTexture(m_pResources->GetTexture(L"ScoreFont2.png"));
	
	m_checkMarkTexture.SetTexture(m_pResources->GetTexture(L"CheckMark.png"));

	// リスナーの設定
	m_pResources->SetListener(m_player->GetPosition(),
		DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitX, m_player->GetRotation()),
		DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitY, m_player->GetRotation())
	);

	// カウントの初期化
	m_count = -1;

	// チュートリアル番号の初期化
	m_tutorialIndex = ORDER::MOUSE_MOVE;
	m_tutorialTexture.SetTexture(m_pResources->GetTexture(L"Tutorial" + std::to_wstring(m_tutorialIndex) + L".png"));

	// 説明番号の初期化
	m_explainIndex = EXPLAINORDER::SCORE_UP;
	m_explainTexture.SetTexture(nullptr);

	// チェックできない
	m_isCheck = false;

	// BGM
	m_bgm = m_pResources->GetBGMSound(L"GameBgm.wav", m_player->GetPosition(), true);

	// プレイ人数を初期化
	GetSceneManager()->SetPlayerCount(PLAYER_COUNT);
}



/// <summary>
/// 更新処理
/// </summary>
/// <param name="elapsedTime"></param> 経過時間
void TutorialScene::Update(float elapsedTime)
{
	// リスナーの設定
	SetListener();

	// カメラの上向きベクトルの更新
	m_cameraUp->Update(elapsedTime);

	// カメラの更新
	m_camera->Update(m_player.get(), m_cameraUp->GetPosition(), m_field->GetCollider().GetPosition());
	/*m_camera->DebugMode();*/

	// フィールドの更新
	m_field->Update(elapsedTime);

	// チュートリアルの更新
	Tutorial(elapsedTime);

	SetPlayerInputState();
	m_player->Update(elapsedTime);

	// 敵の更新
	if (m_tutorialIndex == ORDER::BALL_CATCH)
	{
		m_enemy->Update(elapsedTime);

		// ボールが止まっていたら敵の手にボールを持たせる
		if (m_ballManager->GetBall(0)->GetCurrentState() == m_ballManager->GetBall(0)->GetStopping())
		{
			m_ballManager->GetBall(0)->SetPosition(m_enemy->GetPosition());
		}
	}

	// ボールマネージャーの更新
	m_ballManager->Update(elapsedTime);

	// 矢印の更新
	m_arrow->Update(elapsedTime);

	// 空中の的の更新
	m_airTarget->Update(elapsedTime);

	// 実体とフィールドの当たり判定
	IsHitEntityToField(m_player.get(), m_field.get());
	IsHitEntityToField(m_enemy.get(), m_field.get());
	IsHitEntityToField(m_cameraUp.get(), m_field.get());
	IsHitEntityToField(m_airTarget.get(), m_field.get());
	IsHitEntityToField(m_arrow.get(), m_field.get());
	for (int i = 0; i < m_ballManager->GetObjectCount(); i++)
	{
		IsHitEntityToField(m_ballManager->GetBall(i), m_field.get());

		if (IsHit(m_ballManager->GetBall(i)->GetCollider(), m_airTarget->GetCollider()))
		{
			m_player->SetIsLockOn(false);
			m_airTarget->ChangeState(m_airTarget->GetHitting());
			m_scoreManager->GetScore(m_ballManager->GetBall(i)->GetBallColorNum())->ScoreUp();
		}
	}

	//// 0になったら終了
	//if (m_gameTimer <= 0.0f)
	//{
	//	// ゲーム時間を戻す
	//	m_gameTimer = MAX_TIME;

	//	// ランキングの更新
	//	m_scoreManager->SortRank();
	//	for (int i = 0; i < GetSceneManager()->GetPlayerCount(); i++)
	//	{
	//		GetSceneManager()->SetRank(i, m_scoreManager->GetRank(i));
	//	}

	//	// シーンの変更
	//	ChangeScene<TitleScene>();
	//}

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

	// 空中の的の描画
	if (m_airTarget->GetPosition().y <= 10.0f)
	{
		m_airTarget->Render();
	}

	// プレイヤーの描画
	m_player->Render();

	// マウスを動かすチュートリアルだったらロックオンの描画
	if (m_tutorialIndex == ORDER::MOUSE_TO_STER)
	{
		if (m_player->CalcRaySphere(m_airTarget->GetPosition(), m_airTarget->GetCollider().GetRadius(), m_player->GetMouseRayHitPos()))
		{
			m_player->DrawLockOn(m_airTarget->GetPosition());
		}
	}

	// 敵の描画
	m_enemy->Render();

	// ボールマネージャーの描画
	for (int i = 0; i < m_ballManager->GetObjectCount(); i++)
	{
		if (m_ballManager->GetBall(i)->GetPosition().y <= 10.0f)
		{
			m_ballManager->GetBall(i)->Render();
		}
	}

	// スコアマネージャーの描画
	m_scoreManager->Render();

	// タイマーの描画
	m_frameTexture.Draw(FREAM.pos, FREAM.size, FREAM.scale);
	m_timerTexture.DigitsDraw(TIMER.pos.x, TIMER.pos.y, TIMER.size.x, TIMER.size.y, MAX_TIME, TIMER.scale);

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
	

	// デバック用
	// カメラの上向きベクトルの描画
	/*m_cameraUp->Render();*/

	//debugFont->Render(L"Count", m_count);
}



/// <summary>
/// 終了処理
/// </summary>
void TutorialScene::Finalize()
{
	// フィールドの終了
	m_field->Finalize();

	// ボールマネージャーの終了
	m_ballManager->Finalize();

	// プレイヤーの終了
	m_player->Finalize();

	// 敵の終了
	m_enemy->Finalize();

	// カメラの上向きベクトルの終了
	m_cameraUp->Finalize();

	// 空中の的の終了
	m_airTarget->Finalize();

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
		m_player->SetMouseRay(m_player->CreatePickingRay(mouse.x, mouse.y, r.right, r.bottom, *view, *proj));

		// フィールドとマウスレイが当たっていたらプレイヤーを回転
		if (m_player->CalcRaySphere(m_field->GetCollider().GetPosition(), m_field->GetCollider().GetRadius(), m_player->GetMouseRayHitPos()))
		{
			m_player->RotateToMouse();
		}

		// プレイヤーの設定
		m_player->SetVelocity(m_player->GetGravity());
		m_player->SetPosition(m_player->GetPosition() + m_player->GetVelocity() * elapsedTime);
		m_player->GetCollider().SetPosition(m_player->GetPosition());

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
		if (IsHit(m_player->GetCollider(), m_arrow->GetCollider()))
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

			m_ballManager->GetBall(0)->SetPosition(DirectX::SimpleMath::Vector3{
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
		static bool isRightBall = false;;

		// 左手にボールを持ったらチェックマークをつける
		if (m_player->GetCatchBall(Player::HAND::RIGHT) && !m_player->GetCatchBall(Player::HAND::LEFT) && !isRightBall)
		{
			m_ballManager->GetBall(1)->SetPosition(DirectX::SimpleMath::Vector3{
					m_pResources->GetJson(L"Ball.json")["Position"]["0"]["1"]["x"],
					m_pResources->GetJson(L"Ball.json")["Position"]["0"]["1"]["y"],
					m_pResources->GetJson(L"Ball.json")["Position"]["0"]["1"]["z"]
				}
			);

			isRightBall = true;
		}

		// 左手にボールを持ったらチェックマークをつける
		if (m_player->GetCatchBall(Player::HAND::LEFT))
		{
			static DirectX::SimpleMath::Vector3 pos = m_player->GetCatchBall(Player::HAND::LEFT)->GetPosition();
			m_isCheck = true;
			m_player->SetPosition(pos);
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
			m_tutorialIndex = MOUSE_TO_STER;
			m_tutorialTexture.SetTexture(m_pResources->GetTexture(L"Tutorial" + std::to_wstring(m_tutorialIndex) + L".png"));
			m_interval = 0.0f;
		}
	}
	break;

	// マウスを星に近づける
	case TutorialScene::MOUSE_TO_STER:
	{
		// 空中の的の座標の設定
		m_airTarget->SetPosition(DirectX::SimpleMath::Vector3{
			m_pResources->GetJson(L"AirTarget.json")["TutorialPos2"]["x"],
			m_pResources->GetJson(L"AirTarget.json")["TutorialPos2"]["y"],
			m_pResources->GetJson(L"AirTarget.json")["TutorialPos2"]["z"]
			}
		);

		// マウスの取得
		auto mouse = DirectX::Mouse::Get().GetState();
		// 行列の取得
		auto proj = m_pUserResources->GetProject();
		auto view = m_pUserResources->GetView();

		// レイの設定
		auto const r = m_pUserResources->GetDeviceResources()->GetOutputSize();
		m_player->SetMouseRay(m_player->CreatePickingRay(mouse.x, mouse.y, r.right, r.bottom, *view, *proj));

		// マウスレイが空中の的に当たったらチェックマークをつける
		if (m_player->CalcRaySphere(m_airTarget->GetPosition(), m_airTarget->GetCollider().GetRadius(), m_player->GetMouseRayHitPos()))
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
		for (int i = 0; i < m_ballManager->GetObjectCount(); i++)
		{
			if (IsHit(m_ballManager->GetBall(i)->GetCollider(), m_airTarget->GetCollider()))
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
			m_player->SetCatchBall(Player::HAND::LEFT, nullptr);
			m_player->SetCatchBall(Player::HAND::RIGHT, nullptr);
			m_player->ChangeState(m_player->GetStanding());

			// ボールを触れないように高い所に置く
			for (int i = 0; i < m_ballManager->GetObjectCount(); i++)
			{
				m_ballManager->GetBall(i)->SetPosition(DirectX::SimpleMath::Vector3{
                        m_pResources->GetJson(L"Ball.json")["TutorialPos"]["x"],
                        m_pResources->GetJson(L"Ball.json")["TutorialPos"]["y"],
                        m_pResources->GetJson(L"Ball.json")["TutorialPos"]["z"]
					}
				);

				m_ballManager->GetBall(i)->GetCollider().SetPosition(m_ballManager->GetBall(i)->GetPosition());
			}

			// ボールを止める状態にして敵に持たせる
			m_ballManager->GetBall(0)->ChangeState(m_ballManager->GetBall(0)->GetStopping());
			m_ballManager->GetBall(0)->SetPosition(m_enemy->GetPosition());

			// 描画されないように空中の的を高い所に置く
			m_airTarget->SetPosition(DirectX::SimpleMath::Vector3{
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
		if (m_player->GetCatchBall(Player::HAND::RIGHT))
		{
			m_isCheck = true;
		}
		else
		{
			// 敵のボールに当たったら当たった判定をつける
			if (IsHit(m_ballManager->GetBall(0)->GetCollider(), m_player->GetCollider()) && m_ballManager->GetBall(0)->GetCurrentState() == m_ballManager->GetBall(0)->GetMoving())
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

		// インターバルの時間が上限に行ったら次のチュートリアルに進む
		if (m_interval >= EXPLAIN_INTERVAL)
		{
			// もし当たった判定が付いたら説明を消して戻す
			if (isHit)
			{
				isHit = false;
				m_tutorialTexture.SetTexture(m_pResources->GetTexture(L"Tutorial" + std::to_wstring(m_tutorialIndex) + L".png"));
				m_interval = 0.0f;
				m_explainTexture.SetTexture(nullptr);
				return;
			}

			/*m_isCheck = false;
			m_tutorialIndex = BALL_CATCH;
			m_tutorialTexture.SetTexture(m_pResources->GetTexture(L"Tutorial" + std::to_wstring(m_tutorialIndex) + L".png"));
			m_interval = 0.0f;
			m_explainTexture.SetTexture(nullptr);*/

			// タイトルシーンに戻す
			ChangeScene<TitleScene>();
		}
	}
	break;
	}
	
}



/// <summary>
/// 実体とフィールドが当たっていたら
/// </summary> 
/// <param name="pIEntity">実体</param>
/// <param name="pField">フィールド</param>
void TutorialScene::IsHitEntityToField(IEntity* pIEntity, Field* pField)
{
	// 重力の設定
	pIEntity->SetGravity(pField->CorrectUp(pIEntity));

	// 当たっていたら重なりの補填
	if (IsHit(pIEntity->GetCollider(), pField->GetCollider()))
	{
		pIEntity->CorrectOverlap(*pField);
	}
}



/// <summary>
/// リスナーの設定
/// </summary>
void TutorialScene::SetListener()
{
	// 方向
	DirectX::SimpleMath::Vector3 dir = m_player->GetPosition() - m_cameraUp->GetPosition();
	dir.Normalize();

	// 方向ベクトルの反転
	DirectX::SimpleMath::Vector3 targetUp;
	targetUp = -dir;

	// 現在の姿勢制御
	DirectX::SimpleMath::Vector3 currentUp = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitX, m_player->GetRotation());

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

	q = m_player->GetRotation() * q;

	// リスナーの設定
	m_pResources->SetListener(m_player->GetPosition(),
		DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitX, q),
		DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitY, m_player->GetRotation())
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
	m_player->OnEvents(e);
}
