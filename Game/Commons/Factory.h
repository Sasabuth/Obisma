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
// 定数
public:
	// フィールドの番号
	enum FieldNumder : int
	{
		PLAYER = 0,
		ENEMY,
		AIRTARGET,
		CAMERAUP,
		ARROW,
		FIELD,
		CAMERA,

		BALL = 10,
	};


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
	static std::unique_ptr<Field> CreateField(int stageIndex, bool isSkyDome = true);
	static std::unique_ptr<Field> CreateTutorialField(int stageIndex, bool isSkyDome = true);

	// ボールを生成する
	static std::unique_ptr<Ball> CreateBall(
		Field* pField,
		const DirectX::SimpleMath::Vector3& initialPosition,
		int objectID
	);

	// ボールマネージャーを生成する
	static std::unique_ptr<BallManager> CreateBallManager(
		Field* pField,
		int ballCount
	);

	// 空中の的を生成する
	static std::unique_ptr<AirTarget> CreateAirTarget(
		const DirectX::SimpleMath::Vector3& initialPosition
	);

	// 矢印を生成する
	static std::unique_ptr<Arrow> CreateArrow(
		const DirectX::SimpleMath::Vector3& initialPosition
	);

	// スコアを生成する
	static std::unique_ptr<Score> CreateScore(int index);

	// スコアマネージャーを生成する
	static std::unique_ptr<ScoreManager> CreateScoreManager();
};

