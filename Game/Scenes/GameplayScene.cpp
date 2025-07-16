/// <summary>
/// Gameplayシーンに関するソースファイル
/// </summary>
/// <author>仲森智史</author>
/// <date></date>


// ヘッダファイルの読み込み
#include "pch.h"
#include "GameplayScene.h"

#include "Game/Scenes/TitleScene.h"
#include "Game/Commons/Factory.h"
#include "Game/Commons/Resources.h"


// 名前の省略
using namespace DirectX;


/// <summary>
/// コンストラクタ
/// </summary>
GameplayScene::GameplayScene()
	: m_userResources(nullptr)
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
	Resources::GetInstance()->LoadResource();
	// ユーザーリソースの取得
	m_userResources = UserResources::GetUserResource();

	// デバックフォントの初期化(シーンのみ)
	auto* debugFont = m_userResources->GetDebugFont();
	debugFont->Initialize();

	// フィールドの初期化
	m_field = Factory::CreateField(this);

	// カメラの初期化
	m_camera = std::make_unique<Camera>(m_userResources->GetDeviceResources()->GetOutputSize().bottom, m_userResources->GetDeviceResources()->GetOutputSize().right);


	// プレイヤーの初期化
	m_player = Factory::CreatePlayer(this, SimpleMath::Vector3{ 2.0f,3.0f,2.0f });

	m_cameraUp = std::make_unique<CameraUp>(m_player.get());
	m_cameraUp->Initialize(SimpleMath::Vector3{ 3.0f,3.0f,3.0f });

	// プレイヤーの初期化
	m_ball = Factory::CreateBall(this, SimpleMath::Vector3{ 1.0f,2.0f,4.0f });
}



/// <summary>
/// 更新処理
/// </summary>
/// <param name="elapsedTime"></param> 経過時間
void GameplayScene::Update(float elapsedTime)
{
	// キーボードの取得
	auto keyboard = m_userResources->GetKeyboardStateTracker();

	auto player = dynamic_cast<Player*>(m_player.get());
	m_camera->Update(player, m_cameraUp->GetPosition(), m_field->GetCollider().GetPosition());
	m_cameraUp->Update(elapsedTime);
	m_camera->DebugMode();

	m_field->Update(elapsedTime);
	m_player->Update(elapsedTime);
	m_ball->Update(elapsedTime);



	IsHitEntityToField(m_player.get(), m_field.get());
	IsHitEntityToField(m_ball.get(), m_field.get());
	IsHitEntityToField(m_cameraUp.get(), m_field.get());

	if (keyboard->IsKeyPressed(DirectX::Keyboard::Keys::Space))
	{
		ChangeScene<TitleScene>();
	}
}



/// <summary>
/// 描画処理
/// </summary>
void GameplayScene::Render()
{
	auto* debugFont = UserResources::GetUserResource()->GetDebugFont();
	debugFont->Render(L"GameplayScene");

	m_field->Render();
	m_ball->Render();
	m_player->Render();
}



/// <summary>
/// 終了処理
/// </summary>
void GameplayScene::Finalize()
{
	m_field->Finalize();
	m_ball->Finalize();
	m_player->Finalize();
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
/// 実体とフィールドが当たっていたら
/// </summary> 
/// <param name="pIEntity">実体</param>
/// <param name="pField">フィールド</param>
void GameplayScene::IsHitEntityToField(IEntity* pIEntity, Field* pField)
{
	// 当たっていたら重なりの補填
	if (IsHit(pIEntity->GetCollider(), pField->GetCollider()))
	{
		pIEntity->CorrectOverlap(*pField);
	}

	// 重力の設定
	pIEntity->SetGravity(pField->CorrectUp(pIEntity));
}
