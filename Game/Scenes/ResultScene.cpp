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



/// <summary>
/// コンストラクタ
/// </summary>
ResultScene::ResultScene()
	: m_pUserResources(nullptr)
	, m_speed(0.0f)
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

	// デバック用
	GetSceneManager()->SetPlayerCount(2);

	// テクスチャの初期化
	for (int i = 0; i < GetSceneManager()->GetPlayerCount(); i++)
	{
		std::unique_ptr<Sprite> faceSprite = std::make_unique<Sprite>();
		faceSprite->SetTexture(nullptr);
		m_faceTextures.push_back(std::move(faceSprite));

		std::unique_ptr<Sprite> winSprite = std::make_unique<Sprite>();
		winSprite->SetTexture(nullptr);
		m_winTextures.push_back(std::move(winSprite));
	}

	m_spaceTexture.SetTexture(Resources::GetInstance()->GetTexture(L"Space.png"));
	m_backTexture.SetTexture(Resources::GetInstance()->GetTexture(L"Back.png"));

	m_position = DirectX::SimpleMath::Vector2(0, 360);
	m_position2 = DirectX::SimpleMath::Vector2(1280, 360);

	m_speed = 0.0f;

	m_bgm = Resources::GetInstance()->GetBGMSound(L"ResultBgm.wav", DirectX::SimpleMath::Vector3::Zero, true);
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

	for (int i = 0; i < GetSceneManager()->GetPlayerCount(); i++)
	{
		if (!m_faceTextures[i]->GetTexture())
		{
			std::wstring filename = L"Face" + std::to_wstring(GetSceneManager()->GetRank(i)) + L".png";
			m_faceTextures[i]->SetTexture(Resources::GetInstance()->GetTexture(filename.c_str()));
		}

		if (!m_winTextures[i]->GetTexture())
		{
			std::wstring filename = L"Result" + std::to_wstring(i) + L".png";
			m_winTextures[i]->SetTexture(Resources::GetInstance()->GetTexture(filename.c_str()));
		}
	}

	m_position.x -= 100.0f * elapsedTime;
	m_position2.x -= 100.0f * elapsedTime;

	if (m_position.x < -1280)
	{
		m_position.x = 1280;
	}
	if (m_position2.x < -1280)
	{
		m_position2.x = 1280;
	}

	m_speed += 6.0f * elapsedTime;

	// シーンの変更
	if (mouseTK->leftButton == mouseTK->PRESSED)
	{
		ChangeScene<TitleScene>();
	}

	// BGMの音量の設定
	m_bgm->SetVolume(Resources::GetInstance()->GetBGMVolume());
}



/// <summary>
/// 描画処理
/// </summary>
void ResultScene::Render()
{
	m_spaceTexture.Draw(m_position, DirectX::SimpleMath::Vector2(0, 1024), 1.26f, DirectX::Colors::DarkGray);
	m_spaceTexture.Draw(m_position2, DirectX::SimpleMath::Vector2(0, 1024), 1.26f, DirectX::Colors::DarkGray);

	for (int i = 0; i < GetSceneManager()->GetPlayerCount(); i++)
	{
		m_faceTextures[i]->Draw(DirectX::SimpleMath::Vector2(1280.0f / (GetSceneManager()->GetPlayerCount() + 1) * (i + 1), 300.0f), DirectX::SimpleMath::Vector2(860, 660), 0.3f);
		m_winTextures[i]->Draw(DirectX::SimpleMath::Vector2(1280.0f / (GetSceneManager()->GetPlayerCount() + 1) * (i + 1), 170.0f), DirectX::SimpleMath::Vector2(504, 371), 0.25f);
	}

	m_backTexture.Draw(DirectX::SimpleMath::Vector2(340, 550 + sin(m_speed)), DirectX::SimpleMath::Vector2::Zero, 0.25f);
	
	//auto* debugFont = UserResources::GetUserResource()->GetDebugFont();
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
