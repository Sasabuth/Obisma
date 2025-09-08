/// <summary>
/// Factoryに関するヘッダファイル
/// </summary>
/// <author>仲森智史</author>

// 多重インクルードの防止
#pragma once

// ヘッダファイルの読み込み
#include "Game/GameObjects/Player/Player.h"
#include "Game/GameObjects/Enemy/Enemy.h"
#include "Game/GameObjects/Field/Field.h"
#include "Game/GameObjects/Camera/CameraUp.h"
#include "Game/GameObjects/Ball/Ball.h"
#include "Game/GameObjects/Ball/BallManager.h"
#include "Game/GameObjects/AirTarget/AirTarget.h"
#include "Game/GameObjects/ScoreManager/ScoreManager.h"


// クラスの定義
class GameplayScene;
class Camera;


// クラスの定義
class Factory
{
// 変数
private:


// 関数
public:
	// プレイヤーを生成する
	static std::unique_ptr<Player> CreatePlayer(
		GameplayScene* pScene,
		BallManager* ballManager,
		const DirectX::SimpleMath::Vector3& initialPosition
	);

	// 敵を生成する
	static std::unique_ptr<Enemy> CreateEnemy(
		GameplayScene* pScene,
		BallManager* ballManager,
		const DirectX::SimpleMath::Vector3& initialPosition
	);

	// カメラの上向きベクトルを生成する
	static std::unique_ptr<CameraUp> CreateCameraUp(
		Player* player,
		const DirectX::SimpleMath::Vector3& initialPosition
	);

	// フィールド生成する
	static std::unique_ptr<Field> CreateField(
		GameplayScene* pScene
	);

	// ボールを生成する
	static std::unique_ptr<Ball> CreateBall(
		GameplayScene* pScene, 
		const DirectX::SimpleMath::Vector3& initialPosition
	);

	// ボールマネージャーを生成する
	static std::unique_ptr<BallManager> CreateBallManager(
		GameplayScene* pScene
	);

	// 空中の的を生成する
	static std::unique_ptr<AirTarget> CreateAirTarget(
		GameplayScene* pScene,
		const DirectX::SimpleMath::Vector3& initialPosition
	);

	// スコアマネージャーを生成する
	static std::unique_ptr<ScoreManager> CreateScoreManager();
};

