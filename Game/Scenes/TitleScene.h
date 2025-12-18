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
#include "Game/GameObjects/UI/MenuUI.h"
#include "Game/GameObjects/UI/FieldSelectUI.h"


// クラスの定義
class TitleScene : public Scene
{
private:
	static constexpr int MENU_COUNT = 3;

	static constexpr Sprite::Format MENU[MENU_COUNT] =
	{
		{
			DirectX::SimpleMath::Vector2(350.0f, 500.0f),
			DirectX::SimpleMath::Vector2(812.0f, 260.0f),
			0.2f,
		},
		
		{
			DirectX::SimpleMath::Vector2(350.0f, 580.0f),
			DirectX::SimpleMath::Vector2(940.0f, 261.0f),
			0.2f
		},
		
		{
			DirectX::SimpleMath::Vector2(350.0f, 660.0f),
			DirectX::SimpleMath::Vector2(450.0f, 261.0f),
			0.2f
		},
	};

	static constexpr Sprite::Format TITLE =
	{
		DirectX::SimpleMath::Vector2(400, 240),
		DirectX::SimpleMath::Vector2(1024, 641),
		0.7f
	};

private:
	// ユーザーリソース
	UserResources* m_pUserResources;

	// タイトルテクスチャ
	Sprite m_titleTexture;

	// ボタンの配列
	Button m_button[3];

	// カメラ
	std::unique_ptr<Camera> m_camera;
	DirectX::SimpleMath::Vector3 m_position;
	DirectX::SimpleMath::Vector3 m_eyePosition;

	// フィールド
	std::unique_ptr<Field> m_field[FieldSelectUI::MAXSTAGE_COUNT];

	// オーディオUI
	AudioUI m_audioUI;

	// メニューUI
	MenuUI m_menuUI;

	// フィールド選択UI
	FieldSelectUI m_fieldSelectUI;

	// BGM
	std::unique_ptr<DirectX::SoundEffectInstance> m_bgm;

	// コライダー
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

