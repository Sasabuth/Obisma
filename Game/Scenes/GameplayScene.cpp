/// <summary>
/// Gameplayシーンに関するソースファイル
/// </summary>
/// <author>仲森智史</author>


// ヘッダファイルの読み込み
#include "pch.h"
#include "GameplayScene.h"

#include "Game/Scenes/ResultScene.h"
#include "Game/Commons/Factory.h"
#include "Game/Commons/Resources.h"


// 名前の省略
using namespace DirectX;


/// <summary>
/// コンストラクタ
/// </summary>
GameplayScene::GameplayScene()
	: m_userResources(nullptr)
	, m_gameTimer(0)
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
	m_userResources = UserResources::GetUserResource();

	// デバックフォントの初期化(シーンのみ)
	auto* debugFont = m_userResources->GetDebugFont();
	debugFont->Initialize();

	// フィールドの初期化
	m_field = Factory::CreateField(this);

	// カメラの初期化
	m_camera = std::make_unique<Camera>(m_userResources->GetDeviceResources()->GetOutputSize().bottom, m_userResources->GetDeviceResources()->GetOutputSize().right);

	// ボールマネージャーの初期化
	m_ballManager = Factory::CreateBallManager(this);

	// プレイヤーの初期化
	m_player = Factory::CreatePlayer(this, m_ballManager.get(), SimpleMath::Vector3{ 0.1f,3.0f,0.1f });
	
	// 敵の初期化
	m_enemy = Factory::CreateEnemy(this, m_ballManager.get(), SimpleMath::Vector3{ 0.1f,-3.0f,0.1f });

	// カメラの上向きベクトルの初期化
	m_cameraUp = Factory::CreateCameraUp(m_player.get(), SimpleMath::Vector3{ 2.0f,2.0f,2.0f });

	// 空中の的の初期化
	m_airTarget = Factory::CreateAirTarget(this, SimpleMath::Vector3{ -2.0f,2.0f,3.0f });

	// スコアマネージャーの初期化
	m_scoreManager = Factory::CreateScoreManager();
	m_scoreManager->Add(m_player->GetScore());
	m_scoreManager->Add(m_enemy->GetScore());

	// ゲーム時間の初期化
	m_gameTimer = MAX_TIME;

	m_frameSprite.SetTexture(Resources::GetInstance()->GetTexture(L"ScoreFrame2.png"));
	m_timerSprite.SetTexture(Resources::GetInstance()->GetTexture(L"ScoreFont2.png"));

	Resources::GetInstance()->SetVolume(0.0f);
	m_bgm = Resources::GetInstance()->GetSound(L"Bgm.wav");
	m_bgm->Play(true);

	// プレイ人数を初期化
	GetSceneManager()->SetPlayerCount(2);
}



/// <summary>
/// 更新処理
/// </summary>
/// <param name="elapsedTime"></param> 経過時間
void GameplayScene::Update(float elapsedTime)
{	
	// カメラの上向きベクトルの更新
	m_cameraUp->Update(elapsedTime);

	// カメラの更新
	m_camera->Update(m_player.get(), m_cameraUp->GetPosition(), m_field->GetCollider().GetPosition());
	/*m_camera->DebugMode();*/

	// フィールドの更新
	m_field->Update(elapsedTime);

	// プレイヤーの更新
	m_player->Update(elapsedTime);

	// 敵の更新
	m_enemy->Update(elapsedTime);

	// ボールマネージャの更新
	m_ballManager->Update(elapsedTime);

	// 空中の的の更新
	m_airTarget->Update(elapsedTime);

	// 実体とフィールドの当たり判定
	IsHitEntityToField(m_player.get(), m_field.get());
	IsHitEntityToField(m_enemy.get(), m_field.get());
	IsHitEntityToField(m_cameraUp.get(), m_field.get());
	IsHitEntityToField(m_airTarget.get(), m_field.get());
	for (int i = 0; i < m_ballManager->GetObjectCount(); i++)
	{
		IsHitEntityToField(m_ballManager->GetBall(i), m_field.get());

		if (IsHit(m_ballManager->GetBall(i)->GetCollider(), m_airTarget->GetCollider()))
		{
			m_airTarget->ChangeState(m_airTarget->GetHitting());
			m_scoreManager->GetScore(m_ballManager->GetBall(i)->GetBallColorNum())->ScoreUp();
		}
	}

	// ゲーム時間の初期化
	m_gameTimer -= elapsedTime;

	// シーン変更
	if (m_gameTimer <= 0.0f)
	{
		m_gameTimer = MAX_TIME;

		m_scoreManager->SortRank();
		for (int i = 0; i < GetSceneManager()->GetPlayerCount(); i++)
		{
			GetSceneManager()->SetRank(i,m_scoreManager->GetRank(i));
		}
		
		ChangeScene<ResultScene>();
	}
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

	m_frameSprite.Draw(SimpleMath::Vector2(640, 52), SimpleMath::Vector2(415, 239), 0.28f);
	m_timerSprite.DigitsDraw(571, 25, NUMBER_WIDTH, NUMBER_HEIGHT, (int)m_gameTimer, 1.0f);
	
	// デバック用
	// カメラの上向きベクトルの描画
	//m_cameraUp->Render();

	/*debugFont->Render(L"Timer",m_gameTimer);*/
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
