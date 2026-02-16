/// <summary>
/// Gameplayシーンに関するヘッダファイル
/// </summary>
/// <author>仲森智史</author>


// 多重インクルードの防止
#pragma once


// ヘッダファイルの読み込み
#include "Game/Commons/SceneManager.h"
#include "Game/Commons/UserResources.h"
#include "Game/GameObjects/Field/Field.h"
#include "Game/GameObjects/Camera/Camera.h"
#include "Game/GameObjects/Camera/CameraUp.h"
#include "Game/GameObjects/Score/ScoreManager.h"
#include "Game/Commons/Sprite.h"
#include "Game/Commons/Collision.h"
#include "Game/GameObjects/UI/AudioUI.h"
#include "Game/GameObjects/UI/GameMenuUI.h"


// クラスの定義
class GameplayScene : public Scene
{
// 定数
private:
	static constexpr float MAX_TIME = 100.0f;
	static constexpr float FINISH_TIME = 0.0f;
	static constexpr float FADE_TIME = 3.0f;
	static constexpr float COUNTDOWN_TIME = 3.2f;
	static constexpr int PLAYER_COUNT = 2;

	static constexpr Sprite::Format FREAM =
	{
		DirectX::SimpleMath::Vector2(640.0f, 52.0f),   // 座標
		DirectX::SimpleMath::Vector2(415.0f, 239.0f),  // サイズ
		0.28f                                          // 拡大率
	};

	static constexpr Sprite::Format TIMER =
	{
		DirectX::SimpleMath::Vector2(571.0f, 25.0f),   // 座標
		DirectX::SimpleMath::Vector2(34.5f, 50.0f),    // サイズ
		1.0f                                           // 拡大率
	};

	static constexpr Sprite::Format FINISH =
	{
		DirectX::SimpleMath::Vector2(640.0f, 360.0f),   // 座標
		DirectX::SimpleMath::Vector2(712.0f, 231.0f),  // サイズ
		0.8f                                           // 拡大率
	};

	static constexpr Sprite::Format START =
	{
		DirectX::SimpleMath::Vector2(640.0f, 360.0f),   // 座標
		DirectX::SimpleMath::Vector2(732.0f, 252.0f),  // サイズ
		0.7f                                           // 拡大率
	};

	static constexpr Sprite::Format COUNTDOWN =
	{
		DirectX::SimpleMath::Vector2(300.0f, 250.0f),   // 座標
		DirectX::SimpleMath::Vector2(214.0f, 230.0f),  // サイズ
		1.0f                                           // 拡大率
	};


	// 変数
private:
	// ユーザーリソース
	UserResources* m_pUserResources;

	// リソース
	Resources* m_pResources;

	// フィールド
	std::unique_ptr<Field> m_field;

	// カメラ
	std::unique_ptr<Camera> m_camera;

	// カメラの上向きベクトル
	std::unique_ptr<CameraUp> m_cameraUp;

	// スコアマネージャー
	std::unique_ptr<ScoreManager> m_scoreManager;

	// オーディオUI
	AudioUI m_audioUI;

	// ゲームメニューUI
	GameMenuUI m_gameMenuUI;

	// ゲーム時間
	float m_gameTimer;
	float m_fadeTimer;
	float m_countDownTimer;

	// テクスチャ
	Sprite m_frameTexture;
	Sprite m_timerTexture;
	Sprite m_countDownTexture;
	Sprite m_startTexture;
	Sprite m_finishTexture;

	// BGM
	std::unique_ptr<DirectX::SoundEffectInstance> m_bgm;

	// 開始SE
	std::unique_ptr<DirectX::SoundEffectInstance> m_startSE;

	// 終了SE
	std::unique_ptr<DirectX::SoundEffectInstance> m_finishSE;

	// コライダー
	BoxCollider2D m_collider;


// 関数
public:
	// コンストラクタ
	GameplayScene();

	// デストラクタ
	~GameplayScene() override;;

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


	// 設定/取得
public:


	// 内部処理
private:
	// リスナーの設定
	void SetListener();

	// 入力ステートの設定
	void SetPlayerInputState();
};

