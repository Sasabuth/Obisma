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
#include "DebugDraw.h"



/// <summary>
/// コンストラクタ
/// </summary>
GameplayScene::GameplayScene()
	: m_pUserResources(nullptr)
	, m_pResources(nullptr)
	, m_gameTimer(0)
	, m_fadeTimer(0)
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

	// フィールドの初期化
	m_field = Factory::CreateField(Resources::GetInstance()->GetJson(L"FieldSelect.json")["FieldIndex"]);

	// カメラの初期化
	m_camera = std::make_unique<Camera>(m_pUserResources->GetDeviceResources()->GetOutputSize().bottom, m_pUserResources->GetDeviceResources()->GetOutputSize().right);

	// ボールマネージャーの初期化
	m_ballManager = Factory::CreateBallManager(m_field.get(), Resources::GetInstance()->GetJson(L"Ball.json")["Count"]);

	for (int i = 0; i < m_ballManager->GetObjectCount(); i++)
	{
		m_ballManager->GetBall(i)->SetGravity(m_field->CorrectUp(m_ballManager->GetBall(i)));
	}

	// 空中の的の初期化
	m_airTarget = Factory::CreateAirTarget(m_field.get(), m_camera.get(), DirectX::SimpleMath::Vector3{
		m_pResources->GetJson(L"AirTarget.json")["Position"]["x"],
		m_pResources->GetJson(L"AirTarget.json")["Position"]["y"],
		m_pResources->GetJson(L"AirTarget.json")["Position"]["z"]
		}
	);
	// ランダムに座標を設定
	m_airTarget->RandomPosition();

	// プレイヤーの初期化
	m_player = Factory::CreatePlayer(m_field.get(), m_airTarget.get(), m_ballManager.get(), DirectX::SimpleMath::Vector3{
		m_pResources->GetJson(L"Player.json")["Position"]["x"],
		m_pResources->GetJson(L"Player.json")["Position"]["y"],
		m_pResources->GetJson(L"Player.json")["Position"]["z"]
		}
	);
	m_player->SetGravity(m_field->CorrectUp(m_player.get()));

	// 敵の初期化
	m_enemy = Factory::CreateEnemy(m_player.get(), m_field.get(), m_airTarget.get(), m_ballManager.get(), DirectX::SimpleMath::Vector3{
		m_pResources->GetJson(L"Enemy.json")["Position"]["x"],
		m_pResources->GetJson(L"Enemy.json")["Position"]["y"],
		m_pResources->GetJson(L"Enemy.json")["Position"]["z"]
		}
	);
	m_enemy->SetGravity(m_field->CorrectUp(m_enemy.get()));

	// カメラの上向きベクトルの初期化
	m_cameraUp = Factory::CreateCameraUp(m_player.get(), DirectX::SimpleMath::Vector3{ 4.0f,4.0f,4.0f });
	m_cameraUp->SetGravity(m_field->CorrectUp(m_cameraUp.get()));

	// スコアマネージャーの初期化
	m_scoreManager = Factory::CreateScoreManager();
	m_scoreManager->Add(m_player->GetScore());
	m_scoreManager->Add(m_enemy->GetScore());

	// ゲーム時間の初期化
	m_gameTimer = MAX_TIME;

	m_fadeTimer = 0.0f;

	// テクスチャの初期化
	m_frameTexture.SetTexture(m_pResources->GetTexture(L"ScoreFrame2.png"));
	m_timerTexture.SetTexture(m_pResources->GetTexture(L"ScoreFont2.png"));
	m_finishTexture.SetTexture(m_pResources->GetTexture(L"Finish.png"));

	// リスナーの設定
	m_pResources->SetListener(m_player->GetPosition(),
		DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitX, m_player->GetRotation()),
		DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitY, m_player->GetRotation())
	);

	// BGM
	m_bgm = m_pResources->GetBGMSound(L"GameBgm.wav", m_player->GetPosition(), true);

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
	// シーンの変更
	auto transitionMask = m_pUserResources->GetTransitionMask();
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

	// リスナーの設定
	SetListener();

	// カメラの更新
	m_camera->Update(m_player.get(), m_cameraUp->GetPosition(), m_field->GetCollider().GetPosition());
	/*m_camera->DebugMode();*/

	// フィールドの更新
	m_field->Update(elapsedTime);

	// レイを保存しないと各更新でX軸の姿勢が変わって計算がおかしくなる
	DirectX::SimpleMath::Ray ray{ m_player->GetPosition(),  -DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitY,m_player->GetRotation()) };
	DirectX::SimpleMath::Ray ray2{ m_enemy->GetPosition(),  -DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitY,m_enemy->GetRotation()) };
	std::vector<DirectX::SimpleMath::Ray> ray3;
	for (int i = 0; i < m_ballManager->GetObjectCount(); i++)
	{
		DirectX::SimpleMath::Ray ballRay = { m_ballManager->GetBall(i)->GetPosition(), -DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitY, m_ballManager->GetBall(i)->GetRotation())};
		ray3.push_back(ballRay);
	}
	DirectX::SimpleMath::Ray ray4{ m_cameraUp->GetPosition(),  -DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitY,m_cameraUp->GetRotation()) };
	DirectX::SimpleMath::Ray ray5{ m_airTarget->GetPosition(),  -DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitY,m_airTarget->GetRotation()) };


	// プレイヤーの更新
	SetPlayerInputState();
	m_player->Update(elapsedTime);

	// 敵の更新
	m_enemy->Update(elapsedTime);

	// ボールマネージャの更新
	m_ballManager->Update(elapsedTime);

	// 空中の的の更新
	m_airTarget->Update(elapsedTime);

	// カメラの上向きベクトルの更新
	m_cameraUp->Update(elapsedTime);

	// ポリゴンの当たり判定
	IsHitEntityToField(ray, m_player.get(), m_field.get());
	IsHitEntityToField(ray2, m_enemy.get(), m_field.get());
	IsHitEntityToField(ray4, m_cameraUp.get(), m_field.get());
	//IsHitEntityToField(ray5, m_airTarget.get(), m_field.get());

	for (int i = 0; i < m_ballManager->GetObjectCount(); i++)
	{
		IsHitEntityToField(ray3[i], m_ballManager->GetBall(i), m_field.get());

		if (IsHit(m_ballManager->GetBall(i)->GetCollider(), m_airTarget->GetCollider()))
		{
			m_player->SetIsLockOn(false);
			m_airTarget->ChangeState(m_airTarget->GetHitting());
			m_scoreManager->GetScore(m_ballManager->GetBall(i)->GetBallColorNum())->ScoreUp();
		}
	}

	// ゲーム時間の更新
	m_gameTimer -= elapsedTime;

	// 0になったら終了
	if (m_gameTimer <= FINISH_TIME)
	{
		// ランキングの更新
		m_scoreManager->SortRank();
		GetSceneManager()->SetIsDraw(m_scoreManager->GetIsDraw());
		for (int i = 0; i < GetSceneManager()->GetPlayerCount(); i++)
		{
			GetSceneManager()->SetRank(i, m_scoreManager->GetRank(i));
		}
	}

	// BGMの音量の設定
	m_bgm->SetVolume(m_pResources->GetBGMVolume());
}



/// <summary>
/// 描画処理
/// </summary>
void GameplayScene::Render()
{
	// デバックフォントの描画
	auto* debugFont = UserResources::GetUserResource()->GetDebugFont();
	debugFont->Render(L"GameplayScene");

	// フィールドの描画
	m_field->Render();

	// 空中の的の描画
	m_airTarget->Render();

	// プレイヤーの描画
	m_player->Render();

	// 敵の描画
	m_enemy->Render();

	// ボールマネージャーの描画
	m_ballManager->Render();

	// スコアマネージャーの描画
	m_scoreManager->Render();

	// タイマーの描画
	m_frameTexture.Draw(FREAM.pos, FREAM.size, FREAM.scale);
	m_timerTexture.DigitsDraw(TIMER.pos.x, TIMER.pos.y, TIMER.size.x, TIMER.size.y, (int)m_gameTimer, TIMER.scale);

	// トランジションが閉じているなら
	auto transitionMask = m_pUserResources->GetTransitionMask();
	if (transitionMask->IsClose() || m_fadeTimer >= 0.1f)
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
/// 実体とフィールドが当たっていたら
/// </summary> 
/// <param name="pIEntity">実体</param>
/// <param name="pField">フィールド</param>
void GameplayScene::IsHitEntityToField(IEntity* pIEntity, Field* pField)
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
/// 実体とフィールドが当たっていたら
/// </summary>
/// <param name="ray">レイ</param>
/// <param name="pIEntity">実体</param>
/// <param name="pField">フィールド</param>
void GameplayScene::IsHitEntityToField(DirectX::SimpleMath::Ray ray, IEntity* pIEntity, Field* pField)
{
	// 座標
	DirectX::SimpleMath::Vector3 pos;
	// 方向ベクトル
	DirectX::SimpleMath::Vector3 vector;
	// 当たったか
	bool isHit = false;

	// ワールド座標
	DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::CreateScale(pField->GetStageCollider().GetScale()) *
		DirectX::SimpleMath::Matrix::CreateTranslation(pField->GetStageCollider().GetPosition());

	// 三角形の数分for文で回す
	for (size_t i = 0; i + 2 < pField->GetStageCollider().GetIndicesCount(); i += 3)
	{
		// 三角形の点のワールド座標を取得
		DirectX::SimpleMath::Vector3 p0 = DirectX::SimpleMath::Vector3::Transform(pField->GetStageCollider().GetVertices(pField->GetStageCollider().GetIndices((int)i)).position, world);
		DirectX::SimpleMath::Vector3 p1 = DirectX::SimpleMath::Vector3::Transform(pField->GetStageCollider().GetVertices(pField->GetStageCollider().GetIndices((int)i + 1)).position, world);
		DirectX::SimpleMath::Vector3 p2 = DirectX::SimpleMath::Vector3::Transform(pField->GetStageCollider().GetVertices(pField->GetStageCollider().GetIndices((int)i + 2)).position, world);

		// 三角形の中心から遠かったら当たってないことにする
		DirectX::SimpleMath::Vector3 center = (p0 + p1 + p2) / 3.0f;
		float length = (ray.position - center).Length();
		if (length > 5.0f)
		{
			continue;
		}

		// 当たった座標
		DirectX::SimpleMath::Vector3 pos1;
		// レイと三角形が当たっているか
		if (IsHit(ray.position, ray.direction, world, pField->GetStageCollider(), (int)i, pos1))
		{
			// 前と後に当たった座標の距離を求める
			DirectX::SimpleMath::Vector3 d0 = pIEntity->GetPosition() - pos;
			DirectX::SimpleMath::Vector3 d1 = pIEntity->GetPosition() - pos1;

			// 後に当たったほうが近かったら
			if (d0.Length() > d1.Length() )
			{
				// 後の座標を入れる
				pos = pos1;

				// 三角形の法線ベクトルを入れる
				vector = DirectX::SimpleMath::Vector3::Lerp(
					-pIEntity->GetGravity(),
					pField->GetStageCollider().GetNormalVector((int)i),
					0.3f
				);
			}
		}

		// 球体コライダーと三角形が当たっているか
		if (IsHit(pIEntity->GetCollider(), pField->GetStageCollider(), (int)i) && !isHit)
		{
			// 当たっている
			isHit = true;
		}
	}

	// コライダーが当たっていたら
	if (isHit)
	{
		// 押し出しをする
		pIEntity->CorrectOverlap(pos);
	}

	// 法線ベクトルがあったら
	if (vector.Length() >= 0.00001f)
	{
		// 重力の設定
		pIEntity->SetGravity(pField->CorrectUp(pIEntity, vector));
		// 影の座標を当たった座標にする
		pIEntity->SetShadowHitPos(pos);
	}
	else
	{
		// 重力の設定
		pIEntity->SetGravity(pField->CorrectUp(pIEntity));
	}
}



/// <summary>
/// リスナーの設定
/// </summary>
void GameplayScene::SetListener()
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
	m_player->OnEvents(e);
}
