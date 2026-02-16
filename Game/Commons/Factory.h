/// <summary>
/// Factoryに関するヘッダファイル
/// </summary>
/// <author>仲森智史</author>

// 多重インクルードの防止
#pragma once

// ヘッダファイルの読み込み



// クラスの定義
class Camera;
class Player;
class Enemy;
class Field;
class CameraUp;
class Ball;
class BallManager;
class AirTarget;
class Score;
class ScoreManager;
class Arrow;


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
		const DirectX::SimpleMath::Vector3& initialPosition
	);

	// 敵を生成する
	static std::unique_ptr<Enemy> CreateEnemy(
		Field* pField,
		const DirectX::SimpleMath::Vector3& initialPosition
	);

	// カメラの上向きベクトルを生成する
	static std::unique_ptr<CameraUp> CreateCameraUp(
		Field* pField,
		const DirectX::SimpleMath::Vector3& initialPosition
	);

	// フィールド生成する
	static std::unique_ptr<Field> CreateField(Camera* pCamera, int stageIndex, bool isSkyDome = true);
	static std::unique_ptr<Field> CreateTutorialField(Camera* pCamera, int stageIndex, bool isSkyDome = true);

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

