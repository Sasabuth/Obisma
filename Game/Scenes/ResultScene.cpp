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

	// 座標の初期化
	m_position = DirectX::SimpleMath::Vector2(0, 360);
	m_position2 = DirectX::SimpleMath::Vector2(1280, 360);

	// 速度の初期化
	m_speed = 0.0f;

	// フェードをオープンする
	auto transitionMask = m_pUserResources->GetTransitionMask();
	transitionMask->Open();

	// BGMの初期化
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

	if (GetSceneManager()->GetIsDraw())
	{
		std::wstring filename = L"Draw.png";
		m_winTextures[0]->SetTexture(Resources::GetInstance()->GetTexture(filename.c_str()));
	}

	// テクスチャの更新
	for (int i = 0; i < GetSceneManager()->GetPlayerCount(); i++)
	{
		// 勝った人から順にテクスチャを入れる
		if (!m_faceTextures[i]->GetTexture())
		{
			std::wstring filename = L"Face" + std::to_wstring(GetSceneManager()->GetRank(i)) + L".png";
			m_faceTextures[i]->SetTexture(Resources::GetInstance()->GetTexture(filename.c_str()));
		}
		if(!GetSceneManager()->GetIsDraw())
		{
			if (!m_winTextures[i]->GetTexture())
			{
				std::wstring filename = L"Result" + std::to_wstring(i) + L".png";
				m_winTextures[i]->SetTexture(Resources::GetInstance()->GetTexture(filename.c_str()));
			}
		}
	}

	// 座標の更新
	m_position.x -= 100.0f * elapsedTime;
	m_position2.x -= 100.0f * elapsedTime;

	// 座標の上限
	if (m_position.x < -1280)
	{
		m_position.x = 1280;
	}
	if (m_position2.x < -1280)
	{
		m_position2.x = 1280;
	}

	// 速度の更新
	m_speed += 6.0f * elapsedTime;

	auto transitionMask = m_pUserResources->GetTransitionMask();
	if (mouseTK->leftButton == mouseTK->PRESSED)
	{
		// フェードアウトする
		if (transitionMask->IsOpen())
		{
			transitionMask->Close();
		}
	}

	// タイトルシーンに変更
	if (transitionMask->IsClose() && transitionMask->IsEnd())
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
	// テクスチャの描画
	m_spaceTexture.Draw(m_position, SPACE.size, SPACE.scale, DirectX::Colors::DarkGray);
	m_spaceTexture.Draw(m_position2, SPACE.size, SPACE.scale, DirectX::Colors::DarkGray);

	// 人数分回す
	for (int i = 0; i < GetSceneManager()->GetPlayerCount(); i++)
	{
		// 勝った順から顔を描画する
		m_faceTextures[i]->Draw(DirectX::SimpleMath::Vector2(FACE.pos.x / (GetSceneManager()->GetPlayerCount() + 1) * (i + 1), FACE.pos.y), FACE.size, FACE.scale);

		// 引き分けじゃなかったら何位か描画する
		if (!GetSceneManager()->GetIsDraw())
		{
			m_winTextures[i]->Draw(DirectX::SimpleMath::Vector2(WIN.pos.x / (GetSceneManager()->GetPlayerCount() + 1) * (i + 1), WIN.pos.y), WIN.size, WIN.scale);
		}
	}

	// 引き分けだったら引き分けを描画する
	if (GetSceneManager()->GetIsDraw())
	{
		m_winTextures[0]->Draw(DirectX::SimpleMath::Vector2(DRAW.pos.x, DRAW.pos.y), DRAW.size, DRAW.scale);
	}

	// 戻るテクスチャの描画
	m_backTexture.Draw(DirectX::SimpleMath::Vector2(BACK.pos.x, BACK.pos.y + sin(m_speed)), BACK.size, BACK.scale);
	
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
