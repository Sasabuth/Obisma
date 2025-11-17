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
#include "Game/GameObjects/Score/Score.h"
#include "Game/GameObjects/Score/ScoreManager.h"
#include "Game/GameObjects/Tutorial/Arrow.h"


// クラスの定義
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
		Field* pField,
		AirTarget* pAirTarget,
		BallManager* pBallManager,
		const DirectX::SimpleMath::Vector3& initialPosition
	);

	// 敵を生成する
	static std::unique_ptr<Enemy> CreateEnemy(
		Player* pPlayer,
		Field* pField,
		AirTarget* pAirTarget,
		BallManager* pBallManager,
		const DirectX::SimpleMath::Vector3& initialPosition
	);

	// カメラの上向きベクトルを生成する
	static std::unique_ptr<CameraUp> CreateCameraUp(
		Player* pPlayer,
		const DirectX::SimpleMath::Vector3& initialPosition
	);

	// フィールド生成する
	static std::unique_ptr<Field> CreateField();

	// ボールを生成する
	static std::unique_ptr<Ball> CreateBall(
		Field* pField,
		const DirectX::SimpleMath::Vector3& initialPosition
	);

	// ボールマネージャーを生成する
	static std::unique_ptr<BallManager> CreateBallManager(
		Field* pField,
		int ballCount
	);

	// 空中の的を生成する
	static std::unique_ptr<AirTarget> CreateAirTarget(
		Field* pField,
		Camera* pCamera,
		const DirectX::SimpleMath::Vector3& initialPosition
	);

	// 矢印を生成する
	static std::unique_ptr<Arrow> CreateArrow(
		Player* pPlayer,
		const DirectX::SimpleMath::Vector3& initialPosition
	);

	// スコアを生成する
	static std::unique_ptr<Score> CreateScore(int index);

	// スコアマネージャーを生成する
	static std::unique_ptr<ScoreManager> CreateScoreManager();
};

