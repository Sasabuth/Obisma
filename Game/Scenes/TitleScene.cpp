/// <summary>
/// Titleシーンに関するソースファイル
/// </summary>
/// <author>仲森智史</author>


// ヘッダファイルの読み込み
#include "pch.h"
#include "TitleScene.h"

#include "Game/Scenes/GameplayScene.h"
#include "Game/Scenes/TutorialScene.h"
#include "Game/Commons/Factory.h"
#include "Game/Commons/Messenger.h"
#include "Game/Commons/Resources.h"


/// <summary>
/// コンストラクタ
/// </summary>
TitleScene::TitleScene()
	: m_pUserResources(nullptr)
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

	// ゲームの初期化
	InitializeGame();

	// リソースの初期化
	InitializeResource();

	// ボタンの初期化
	InitializeButton();
}



/// <summary>
/// 更新処理
/// </summary>
/// <param name="elapsedTime"></param> 経過時間
void TitleScene::Update(float elapsedTime)
{
	// UIの更新
	UpdateUI();

	// カメラの更新
	m_camera->Update(m_cameraPosition, m_eyePosition);
	
	// フィールドの更新
	for (int i = 0; i < FieldSelectUI::MAXSTAGE_COUNT; i++)
	{
		m_field[i]->TitleUpdate();
		static float rotate = 0.0f;
		rotate += 30.0f * elapsedTime;
		m_field[i]->SetRotate(rotate);
	}

	// BGMの音量の設定
	m_bgm->SetVolume(Resources::GetInstance()->GetBGMVolume());
}



/// <summary>
/// 描画処理
/// </summary>
void TitleScene::Render()
{
	// フィールドの描画
	for (int i = 0; i < FieldSelectUI::MAXSTAGE_COUNT; i++)
	{
		m_field[i]->TitleRender();
	}
	
	// オーディオUIの描画
	if (m_audioUI.IsOpen())
	{
		m_audioUI.Draw(m_collider);
	}
	// フィールド選択UIの更新
	else if (m_fieldSelectUI.IsOpen())
	{
		m_fieldSelectUI.Draw(m_collider);
	}
	// メニューUIの描画
	else if (m_menuUI.IsOpen())
	{
		m_menuUI.Draw(m_collider);
	}
	// タイトルの描画
	else
	{
		m_titleTexture.Draw(TITLE.pos, TITLE.size, TITLE.scale);

		// ボタンに触れていたら赤色にする
		for (int i = 0; i < MENU_COUNT; i++)
		{
			if (IsHit(m_collider, m_button[i].GetCollider()))
			{
				m_button[i].Draw(DirectX::Colors::Red);
			}
			else
			{
				m_button[i].Draw();
			}
		}
	}

	// デバックフォント
	/*auto* debugFont = UserResources::GetUserResource()->GetDebugFont();
	debugFont->Render(L"TitleScene");*/


	// デバック用
	/*m_collider.Draw();*/
}



/// <summary>
/// 終了処理
/// </summary>
void TitleScene::Finalize()
{
	// フィールドの終了
	for (int i = 0; i < FieldSelectUI::MAXSTAGE_COUNT; i++)
	{
		m_field[i]->Finalize();
	}

	// UIの終了
	m_audioUI.Finalize();
	m_menuUI.Finalize();
	m_fieldSelectUI.Finalize();
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



/// <summary>
/// ゲームの初期化
/// </summary>
void TitleScene::InitializeGame()
{
	// カメラの初期化
	m_camera = std::make_unique<Camera>(m_pUserResources->GetDeviceResources()->GetOutputSize().bottom, m_pUserResources->GetDeviceResources()->GetOutputSize().right);

	// フィールドの初期化
	for (int i = 0; i < FieldSelectUI::MAXSTAGE_COUNT; i++)
	{
		// 最初はスカイドームつける
		if (i == 0)
		{
			m_field[i] = Factory::CreateField(i);
		}
		else
		{
			m_field[i] = Factory::CreateField(i, false);
		}

		// 惑星の座標を設定
		m_field[i]->SetPosition(DirectX::SimpleMath::Vector3(i * FIELD_POSITION, 0, 0));
	}

	// カメラ座標の初期化
	m_cameraPosition = CAMERA_POSITION;
	// 目の座標の初期化
	m_eyePosition = EYE_POSITION;

	// コライダーの設定
	m_collider.SetSize(DirectX::SimpleMath::Vector2(Resources::GetInstance()->GetJson(L"Mouse.json")["Collider"]));
}



/// <summary>
/// リソースの初期化
/// </summary>
void TitleScene::InitializeResource()
{
	// デバックフォントの初期化(シーンのみ)
	auto* debugFont = m_pUserResources->GetDebugFont();
	debugFont->Initialize();

	// テクスチャの初期化
	m_titleTexture.SetTexture(Resources::GetInstance()->GetTexture(L"Title.png"));

	// オーディオUIの初期化
	m_audioUI.Initialize();

	// フィールドUIの初期化
	m_fieldSelectUI.Initialize(this);

	// メニューUIの初期化
	m_menuUI.Initialize(&m_fieldSelectUI);

	// BGMの初期化
	m_bgm = Resources::GetInstance()->GetBGMSound(L"TitleBgm.wav", DirectX::SimpleMath::Vector3::Zero, true);
}



/// <summary>
/// ボタンの初期化
/// </summary>
void TitleScene::InitializeButton()
{
	// ボタンの設定(シーン変更)
	m_button[0].SetTexture(Resources::GetInstance()->GetTexture(L"Start.png"));
	m_button[0].SetFunc([this]() { m_menuUI.Click(); });

	// ボタンの設定(UIを開く)
	m_button[1].SetTexture(Resources::GetInstance()->GetTexture(L"Audio.png"));
	m_button[1].SetFunc([this]() { m_audioUI.Click(); });

	auto transitionMask = m_pUserResources->GetTransitionMask();
	// ボタンの設定(終了)
	m_button[2].SetTexture(Resources::GetInstance()->GetTexture(L"End.png"));
	m_button[2].SetFunc([=]()
		{
			// フェードアウトする
			if (transitionMask->IsOpen())
			{
				transitionMask->Close();
			}
		}
	);

	// ボタンの設定
	for (int i = 0; i < MENU_COUNT; i++)
	{
		m_button[i].SetPosition(MENU[i].pos);
		m_button[i].SetSize(MENU[i].size);
		m_button[i].SetScale(MENU[i].scale);
	}

	// フェードをオープンする
	if (!transitionMask->IsOpen()) transitionMask->Open();
}



/// <summary>
/// UIの更新
/// </summary>
void TitleScene::UpdateUI()
{
	// マウスの座標に合わせる
	auto mouse = DirectX::Mouse::Get().GetState();
	// 現在のウィンドウサイズを取得
	auto const outputSize = m_pUserResources->GetDeviceResources()->GetOutputSize();
	float windowWidth = static_cast<float>(outputSize.right - outputSize.left);
	float windowHeight = static_cast<float>(outputSize.bottom - outputSize.top);

	// トランジションマスクの取得
	auto transitionMask = m_pUserResources->GetTransitionMask();
	// フェードアウト中じゃなかったら更新
	if (!transitionMask->IsClose()) m_collider.SetPosition(DirectX::SimpleMath::Vector2((mouse.x / windowWidth) * Sprite::BASE_WIDTH, (mouse.y / windowHeight) * Sprite::BASE_HEIGHT));

	// キーボードの取得
	auto mouseTk = m_pUserResources->GetMouseStateTracker();

	// オーディオUIの更新
	if (m_audioUI.IsOpen())
	{
		m_audioUI.Update(m_collider);
	}
	// フィールド選択UIの更新
	else if (m_fieldSelectUI.IsOpen())
	{
		// フェードアウト中じゃなかったら更新
		if (!transitionMask->IsClose()) m_fieldSelectUI.Update(m_collider);

		m_cameraPosition = m_field[m_fieldSelectUI.GetFieldIndex()]->GetPosition();
		m_eyePosition = DirectX::SimpleMath::Vector3(m_field[m_fieldSelectUI.GetFieldIndex()]->GetPosition().x, m_field[m_fieldSelectUI.GetFieldIndex()]->GetPosition().y, -10);

		// ゲームプレイシーンに変更
		if (transitionMask->IsClose() && transitionMask->IsEnd())
		{
			ChangeScene<GameplayScene>();
		}
	}
	// メニューUIの更新
	else if (m_menuUI.IsOpen())
	{
		// フェードアウト中じゃなかったら更新
		if (!transitionMask->IsClose()) m_menuUI.Update(m_collider);

		m_cameraPosition = DirectX::SimpleMath::Vector3(5, 2, 0);
		m_eyePosition = DirectX::SimpleMath::Vector3(5, 2, -10);

		// チュートリアルシーンに変更
		if (transitionMask->IsClose() && transitionMask->IsEnd())
		{
			ChangeScene<TutorialScene>();
		}
	}
	// タイトルの更新
	else
	{
		// ボタンの上で左クリックをするとクリック処理をする
		if (!transitionMask->IsClose())
		{
			for (int i = 0; i < MENU_COUNT; i++)
			{
				if (IsHit(m_collider, m_button[i].GetCollider()))
				{
					if (mouseTk->leftButton == mouseTk->PRESSED)
					{
						m_button[i].Click();
					}
				}
			}
		}

		// チュートリアルシーンに変更
		if (transitionMask->IsClose() && transitionMask->IsEnd())
		{
			PostQuitMessage(0);
		}
	}
}
