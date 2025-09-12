/// <summary>
/// Titleシーンに関するソースファイル
/// </summary>
/// <author>仲森智史</author>


// ヘッダファイルの読み込み
#include "pch.h"
#include "TitleScene.h"

#include "Game/Scenes/GameplayScene.h"
#include "Game/Commons/Resources.h"
#include "Game/Commons/Factory.h"


// 名前の省略
using namespace DirectX;


/// <summary>
/// コンストラクタ
/// </summary>
TitleScene::TitleScene()
	: m_pUserResources(nullptr)
	, m_speed(0.0f)
{
}



/// <summary>
/// デストラクタ
/// </summary>
TitleScene::~TitleScene()
{
}


/// <summary>
/// 初期化処理
/// </summary>
void TitleScene::Initialize()
{
	// ユーザーリソースの取得
	m_pUserResources = UserResources::GetUserResource();

	// デバックフォントの初期化(シーンのみ)
	auto * debugFont = m_pUserResources->GetDebugFont();
	debugFont->Initialize();

	// テクスチャの初期化
	m_titleTexture.SetTexture(Resources::GetInstance()->GetTitleTexture());
	m_startTexture.SetTexture(Resources::GetInstance()->GetStartTexture());

	// フィールドの初期化
	m_field = Factory::CreateField(this);

	// カメラの初期化
	m_camera = std::make_unique<Camera>(m_pUserResources->GetDeviceResources()->GetOutputSize().bottom, m_pUserResources->GetDeviceResources()->GetOutputSize().right);

	// 速度の初期化
	m_speed = 0.0f;
}



/// <summary>
/// 更新処理
/// </summary>
/// <param name="elapsedTime"></param> 経過時間
void TitleScene::Update(float elapsedTime)
{
	UNREFERENCED_PARAMETER(elapsedTime);

	// カメラの更新
	m_camera->Update();

	// フィールドの更新
	m_field->Update(elapsedTime);
	static float rotate = 0.0f;
	rotate += 30.0f * elapsedTime;
	m_field->SetRotate(rotate);

	m_speed += 6.0f * elapsedTime;

	// キーボードの取得
	auto mouseTk = m_pUserResources->GetMouseStateTracker();

	if (mouseTk->leftButton == mouseTk->PRESSED)
	{
		ChangeScene<GameplayScene>();
	}
}



/// <summary>
/// 描画処理
/// </summary>
void TitleScene::Render()
{
	auto* debugFont = UserResources::GetUserResource()->GetDebugFont();
	debugFont->Render(L"TitleScene");

	m_field->Render();



	m_startTexture.Draw(SimpleMath::Vector2(120,550 + sin(m_speed)), SimpleMath::Vector2::Zero, 0.25f);
	m_titleTexture.Draw(SimpleMath::Vector2(400,240), SimpleMath::Vector2(1024,641), 0.7f);
}



/// <summary>
/// 終了処理
/// </summary>
void TitleScene::Finalize()
{
	// フィールドの終了
	m_field->Finalize();
}



/// <summary>
/// デバイスに依存するリソースを作成する関数
/// </summary>
void TitleScene::CreateDeviceDependentResources()
{
}



/// <summary>
/// ウインドウサイズに依存するリソースを作成する関数
/// </summary>
void TitleScene::CreateWindowSizeDependentResources()
{
}



/// <summary>
/// デバイスロストした時に呼び出される関数
/// </summary>
void TitleScene::OnDeviceLost()
{
}
