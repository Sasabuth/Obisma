/// <summary>
/// Resultシーンに関するソースファイル
/// </summary>
/// <author>仲森智史</author>


// ヘッダファイルの読み込み
#include "pch.h"
#include "ResultScene.h"

#include "Game/Scenes/GameplayScene.h"
#include "Game/Scenes/TitleScene.h"
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
	m_resultTexture.SetTexture(nullptr);
	m_winTexture.SetTexture(nullptr);
}



/// <summary>
/// 更新処理
/// </summary>
/// <param name="elapsedTime"></param> 経過時間
void ResultScene::Update(float elapsedTime)
{
	UNREFERENCED_PARAMETER(elapsedTime);

	// キーボードの取得
	auto mouseTK = m_pUserResources->GetMouseStateTracker();

	if(!m_resultTexture.GetTexture())
	{
		m_resultTexture.SetTexture(Resources::GetInstance()->GetFaceTexture(GetSceneManager()->GetWinner()));
	}
	if(!m_winTexture.GetTexture())
	{
		m_winTexture.SetTexture(Resources::GetInstance()->GetWinTexture(GetSceneManager()->GetWinner()));
	}

	//// デバック
	//m_resultTexture.SetTexture(Resources::GetInstance()->GetFaceTexture(0));
	//m_winTexture.SetTexture(Resources::GetInstance()->GetWinTexture(0));

	// シーンの変更
	if (mouseTK->leftButton == mouseTK->PRESSED)
	{
		ChangeScene<TitleScene>();
	}
}



/// <summary>
/// 描画処理
/// </summary>
void ResultScene::Render()
{
	auto* debugFont = UserResources::GetUserResource()->GetDebugFont();
	debugFont->Render(L"ResultScene");

	if (GetSceneManager()->GetWinner() == 0)
	{
		debugFont->Render(L"PLAYER1_WIN");
	}
	if (GetSceneManager()->GetWinner() == 1)
	{
		debugFont->Render(L"PLAYER2_WIN");
	}
	
	auto const r = UserResources::GetUserResource()->GetDeviceResources()->GetOutputSize();

	m_resultTexture.Draw(SimpleMath::Vector2(640, 200), SimpleMath::Vector2(860, 660), 0.4f);
	m_winTexture.Draw(SimpleMath::Vector2(640, 350), SimpleMath::Vector2(932, 167), 0.4f);
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
