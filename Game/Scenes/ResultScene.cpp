/// <summary>
/// Resultシーンに関するソースファイル
/// </summary>
/// <author>仲森智史</author>


// ヘッダファイルの読み込み
#include "pch.h"
#include "ResultScene.h"

#include "Game/Scenes/GameplayScene.h"
#include "Game/Commons/Resources.h"


// 名前の省略
using namespace DirectX;


/// <summary>
/// コンストラクタ
/// </summary>
ResultScene::ResultScene()
	: m_pUserResources(nullptr)
{
}



/// <summary>
/// デストラクタ
/// </summary>
ResultScene::~ResultScene()
{
}


/// <summary>
/// 初期化処理
/// </summary>
void ResultScene::Initialize()
{
	// ユーザーリソースの取得
	m_pUserResources = UserResources::GetUserResource();

	// デバックフォントの初期化(シーンのみ)
	auto * debugFont = m_pUserResources->GetDebugFont();
	debugFont->Initialize();

	// テクスチャの初期化
	m_resultTexture.SetTexture(Resources::GetInstance()->GetTitleTexture());
}



/// <summary>
/// 更新処理
/// </summary>
/// <param name="elapsedTime"></param> 経過時間
void ResultScene::Update(float elapsedTime)
{
	UNREFERENCED_PARAMETER(elapsedTime);

	// キーボードの取得
	auto keyboard = m_pUserResources->GetKeyboardStateTracker();

	if (keyboard->IsKeyPressed(DirectX::Keyboard::Keys::Space))
	{
		ChangeScene<GameplayScene>();
	}
}



/// <summary>
/// 描画処理
/// </summary>
void ResultScene::Render()
{
	auto* debugFont = UserResources::GetUserResource()->GetDebugFont();
	debugFont->Render(L"ResultScene");

	m_resultTexture.Draw(SimpleMath::Vector2(640,260), SimpleMath::Vector2(1024,641), 0.7f);
}



/// <summary>
/// 終了処理
/// </summary>
void ResultScene::Finalize()
{
}



/// <summary>
/// デバイスに依存するリソースを作成する関数
/// </summary>
void ResultScene::CreateDeviceDependentResources()
{
}



/// <summary>
/// ウインドウサイズに依存するリソースを作成する関数
/// </summary>
void ResultScene::CreateWindowSizeDependentResources()
{
}



/// <summary>
/// デバイスロストした時に呼び出される関数
/// </summary>
void ResultScene::OnDeviceLost()
{
}
