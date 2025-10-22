/// <summary>
/// Titleシーンに関するヘッダファイル
/// </summary>
/// <author>仲森智史</author>


// 多重インクルードの防止
#pragma once


// ヘッダファイルの読み込み
#include "Game/Commons/SceneManager.h"
#include "Game/Commons/UserResources.h"
#include "Game/Commons/Sprite.h"
#include "Game/GameObjects/Camera/Camera.h"
#include "Game/GameObjects/Field/Field.h"
#include "Game/Commons/Collision.h"
#include "Game/GameObjects/Button/Button.h"
#include "Game/GameObjects/UI/AudioUI.h"


// クラスの定義
class TitleScene : public Scene
{
private:
	static constexpr int MENU_COUNT = 3;

	static constexpr DirectX::SimpleMath::Vector2 MENU_POSITIONS[MENU_COUNT] =
	{
		DirectX::SimpleMath::Vector2(350.0f, 500.0f),
		DirectX::SimpleMath::Vector2(350.0f, 580.0f),
		DirectX::SimpleMath::Vector2(350.0f, 660.0f),
	};

	static constexpr DirectX::SimpleMath::Vector2 MENU_SIZES[MENU_COUNT] =
	{
		DirectX::SimpleMath::Vector2(730.0f, 230.0f),
		DirectX::SimpleMath::Vector2(940.0f, 261.0f),
		DirectX::SimpleMath::Vector2(450.0f, 261.0f),
	};

private:
	UserResources* m_pUserResources;

	Sprite m_titleTexture;

	Button m_button[3];

	// カメラ
	std::unique_ptr<Camera> m_camera;

	// フィールド
	std::unique_ptr<Field> m_field;

	// オーディオUI
	AudioUI m_audioUI;

	// BGM
	std::unique_ptr<DirectX::SoundEffectInstance> m_bgm;

	BoxCollider2D m_collider;



// 関数
public:
	// コンストラクタ
	TitleScene();

	// デストラクタ
	~TitleScene() override;;

	// 初期化
	void Initialize() override;

	// 更新
	void Update(float elapsedTime) override;

	// 描画
	void Render() override;

	// 終了処理
	void Finalize() override;

	// デバイスに依存するリソースを作成する関数
	void CreateDeviceDependentResources() override;

	// ウインドウサイズに依存するリソースを作成する関数
	void CreateWindowSizeDependentResources() override;

	// デバイスロストした時に呼び出される関数
	void OnDeviceLost() override;

};

