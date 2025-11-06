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
#include "Game/GameObjects/Player/Player.h"
#include "Game/GameObjects/Enemy/Enemy.h"
#include "Game/GameObjects/Ball/BallManager.h"
#include "Game/GameObjects/Camera/CameraUp.h"
#include "Game/GameObjects/AirTarget/AirTarget.h"
#include "Game/GameObjects/Score/ScoreManager.h"
#include "Game/GameObjects/Tutorial/Arrow.h"
#include "Game/Commons/Sprite.h"


// クラスの定義
class TutorialScene : public Scene
{
// 定数
private:
	// 順番
	enum ORDER
	{
		MOUSE_MOVE = 0,
		PLAYER_MOVE,
		BALL_PICKUP,
		MOUSE_TO_STER,
		BALL_THROW,
		MAX_ORDERCOUNT
	};

	// 説明順番
	enum EXPLAINORDER
	{
		SCORE_UP = 0,
		SCORE_DOWN,
		MAX_EXPLAINCOUNT
	};

	static constexpr float INTERVAL = 2.0f;
	static constexpr float EXPLAIN_INTERVAL = 3.5f;

	static constexpr float MAX_LENGTH = 100.0f;
	static constexpr int MAX_COUNT = 50;

	static constexpr int MAX_TIME = 99;
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
	
	static constexpr Sprite::Format TUTORIAL[ORDER::MAX_ORDERCOUNT] =
	{
		// マウス動かす
		{
		   DirectX::SimpleMath::Vector2(230.0f, 150.0f),   // 座標
		   DirectX::SimpleMath::Vector2(1832.0f, 172.0f),  // サイズ
		   0.22f                                      	   // 拡大率
		},

		// プレイヤーが矢印のほうに動く
		{
		   DirectX::SimpleMath::Vector2(210.0f, 150.0f),   // 座標
		   DirectX::SimpleMath::Vector2(1635.0f, 172.0f),  // サイズ
		   0.22f                                      	   // 拡大率
		},

		// ボールを拾う
		{
		   DirectX::SimpleMath::Vector2(203.0f, 150.0f),   // 座標
		   DirectX::SimpleMath::Vector2(1577.0f, 172.0f),  // サイズ
		   0.22f                                      	   // 拡大率
		},

		// マウスを星に近づける
		{
		   DirectX::SimpleMath::Vector2(185.0f, 150.0f),   // 座標
		   DirectX::SimpleMath::Vector2(1402.0f, 171.0f),  // サイズ
		   0.22f                                      	   // 拡大率
		},

		// ボールを投げる
		{
		   DirectX::SimpleMath::Vector2(210.0f, 150.0f),   // 座標
		   DirectX::SimpleMath::Vector2(1635.0f, 305.0f),  // サイズ
		   0.22f                                      	   // 拡大率
		},
		
	};

	static constexpr Sprite::Format EXPLAIN[EXPLAINORDER::MAX_EXPLAINCOUNT] =
	{
		// 星に当てるとスコアが上がる
		{
		   DirectX::SimpleMath::Vector2(207.0f, 148.0f),   // 座標
		   DirectX::SimpleMath::Vector2(1610.0f, 172.0f),  // サイズ
		   0.22f                                      	   // 拡大率
		},

		// ボールに当たるとスコアが減る
		{
		   DirectX::SimpleMath::Vector2(217.0f, 150.0f),   // 座標
		   DirectX::SimpleMath::Vector2(1718.0f, 171.0f),  // サイズ
		   0.22f                                      	   // 拡大率
		}
	};
	
	static constexpr Sprite::Format CHECKMARK =
	{
		DirectX::SimpleMath::Vector2(45.0f, 145.0f),    // 座標
		DirectX::SimpleMath::Vector2(643.0f, 448.0f),   // サイズ
		0.06f                                 		    // 拡大率
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

	// プレイヤー
	std::unique_ptr<Player> m_player;

	// 敵
	std::unique_ptr<Enemy> m_enemy;

	// ボールマネージャー
	std::unique_ptr<BallManager> m_ballManager;

	// 空中の的
	std::unique_ptr<AirTarget> m_airTarget;

	// スコアマネージャー
	std::unique_ptr<ScoreManager> m_scoreManager;

	// 矢印
	std::unique_ptr<Arrow> m_arrow;

	// ゲーム時間
	float m_interval;

	// カウント
	int m_count;

	// チュートリアル番号
	ORDER m_tutorialIndex;

	// 説明番号
	EXPLAINORDER m_explainIndex;

	// チェックできるか
	bool m_isCheck;

	// テクスチャ
	Sprite m_frameTexture;
	Sprite m_timerTexture;
	Sprite m_tutorialTexture;
	Sprite m_checkMarkTexture;
	Sprite m_explainTexture;

	// BGM
	std::unique_ptr<DirectX::SoundEffectInstance> m_bgm;


	// 関数
public:
	// コンストラクタ
	TutorialScene();

	// デストラクタ
	~TutorialScene() override;;

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
	// チュートリアル
	void Tutorial(float elapsedTime);

	// 実体とフィールドの衝突判定
	void IsHitEntityToField(IEntity* pIEntity, Field* pField);

	// リスナーの設定
	void SetListener();

};

