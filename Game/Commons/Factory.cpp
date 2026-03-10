/// <summary>
/// Factoryに関するソースファイル
/// </summary>
/// <author>仲森智史</author>
/// <date></date>

// 多重インクルードの防止
#pragma once

// ヘッダファイルの読み込み
#include "pch.h"
#include "Factory.h"

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



/// <summary>
/// プレイヤーの作成
/// </summary>
/// <param name="pField">フィールドのポインタ</param>
/// <param name="initialPosition">初期化座標</param>
/// <returns>プレイヤー</returns>
std::unique_ptr<Player> Factory::CreatePlayer(Field* pField, const DirectX::SimpleMath::Vector3& initialPosition)
{
	// プレイヤーを宣言
	std::unique_ptr<Player> player;
	// プレイヤーを生成
	player = std::make_unique<Player>();
	// プレイヤーを初期化
	player->Initialize(initialPosition);
	player->SetGravity(pField->CorrectUp(player.get()));
	// プレイヤーを返す
	return std::move(player);
}



/// <summary>
/// 敵の作成
/// </summary>
/// <param name="pField">フィールドのポインタ</param>
/// <param name="initialPosition">初期化座標</param>
/// <returns>敵</returns>
std::unique_ptr<Enemy> Factory::CreateEnemy(Field* pField, const DirectX::SimpleMath::Vector3& initialPosition)
{
	// 敵を宣言
	std::unique_ptr<Enemy> enemy;
	// 敵を生成
	enemy = std::make_unique<Enemy>();
	// 敵を初期化
	enemy->Initialize(initialPosition);
	enemy->SetGravity(pField->CorrectUp(enemy.get()));
	// 敵を返す
	return std::move(enemy);
}



/// <summary>
/// カメラの上向きベクトルの作成
/// </summary>
/// <param name="pField">フィールドのポインタ</param>
/// <param name="initialPosition">初期化座標</param>
/// <returns>カメラの上向きベクトル</returns>
std::unique_ptr<CameraUp> Factory::CreateCameraUp(Field* pField, const DirectX::SimpleMath::Vector3& initialPosition)
{
	// カメラの上向きベクトルを宣言
	std::unique_ptr<CameraUp> cameraUp;
	// カメラの上向きベクトルを生成
	cameraUp = std::make_unique<CameraUp>();
	// カメラの上向きベクトルを初期化
	cameraUp->Initialize(initialPosition);
	cameraUp->SetGravity(pField->CorrectUp(cameraUp.get()));
	// カメラの上向きベクトルを返す
	return std::move(cameraUp);
}



/// <summary>
/// フィールドの作成
/// </summary>
/// <param name="stageIndex">ステージ番号</param>
/// <param name="isSkyDome">スカイドームを入れるか</param>
/// <returns>フィールド</returns>
std::unique_ptr<Field> Factory::CreateField(int stageIndex, bool isSkyDome)
{
	// フィールドの宣言
	std::unique_ptr<Field> field;
	// フィールドの生成
	field = std::make_unique<Field>();
	// フィールドの初期化
	field->Initialize(stageIndex, isSkyDome);
	// フィールドを返す
	return std::move(field);
}



/// <summary>
/// チュートリアルフィールドの作成
/// </summary>
/// <param name="stageIndex">ステージ番号</param>
/// <param name="isSkyDome">スカイドームを入れるか</param>
/// <returns>フィールド</returns>
std::unique_ptr<Field> Factory::CreateTutorialField(int stageIndex, bool isSkyDome)
{
	// フィールドの宣言
	std::unique_ptr<Field> field;
	// フィールドの生成
	field = std::make_unique<Field>();
	// フィールドの初期化
	field->TutorialInitialize(stageIndex, isSkyDome);
	// フィールドを返す
	return std::move(field);
}



/// <summary>
/// ボールの作成
/// </summary>
/// <param name="pField">フィールドのポインタ</param>
/// <param name="initialPosition">初期化座標</param>
/// <param name="objectID">オブジェクトID</param>
/// <returns>ボール</returns>
std::unique_ptr<Ball> Factory::CreateBall(Field* pField, const DirectX::SimpleMath::Vector3& initialPosition, int objectID)
{
	// ボールの宣言
	std::unique_ptr<Ball> ball;
	// ボールの生成
	ball = std::make_unique<Ball>(objectID);
	// ボールの初期化
	ball->Initialize(initialPosition);
	ball->SetGravity(pField->CorrectUp(ball.get()));
	// ボールを返す
	return std::move(ball);
}



/// <summary>
/// ボールマネージャーの作成
/// </summary>
/// <param name="pField">フィールドのポインタ</param>
/// <param name="ballCount">ボールカウント</param>
/// <returns>ボールマネージャー</returns>
std::unique_ptr<BallManager> Factory::CreateBallManager(Field* pField, int ballCount)
{
	// ボールマネージャーの宣言
	std::unique_ptr<BallManager> ballManager;
	// ボールマネージャーの生成
	ballManager = std::make_unique<BallManager>(pField);
	// ボールマネージャーの初期化
	ballManager->Initialize(ballCount);
	// ボールマネージャーを返す
	return std::move(ballManager);
}



/// <summary>
/// 空中の的の作成
/// </summary>
/// <param name="initialPosition">初期化座標</param>
/// <returns>空中の的</returns>
std::unique_ptr<AirTarget> Factory::CreateAirTarget(const DirectX::SimpleMath::Vector3& initialPosition)
{
	// 空中の的の宣言
	std::unique_ptr<AirTarget> airTarget;
	// 空中の的の生成
	airTarget = std::make_unique<AirTarget>();
	// 空中の的の初期化
	airTarget->Initialize(initialPosition);
	// 空中の的を返す
	return std::move(airTarget);
}



/// <summary>
/// 矢印の作成
/// </summary>
/// <param name="initialPosition">初期化座標</param>
/// <returns>矢印</returns>
std::unique_ptr<Arrow> Factory::CreateArrow(const DirectX::SimpleMath::Vector3& initialPosition)
{
	// 空中の的の宣言
	std::unique_ptr<Arrow> arrow;
	// 空中の的の生成
	arrow = std::make_unique<Arrow>();
	// 空中の的の初期化
	arrow->Initialize(initialPosition);
	// 空中の的を返す
	return std::move(arrow);
}




/// <summary>
/// スコアの作成
/// </summary>
/// <param name="index">番号</param>
/// <returns>スコア</returns>
std::unique_ptr<Score> Factory::CreateScore(int index)
{
	// スコアの宣言
	std::unique_ptr<Score> score;
	// スコアの生成
	score = std::make_unique<Score>();
	// スコアの初期化
	score->Initialize(index);
	// スコアを返す
	return std::move(score);
}



/// <summary>
/// スコアマネージャーの作成
/// </summary>
/// <returns>スコアマネージャー</returns>
std::unique_ptr<ScoreManager> Factory::CreateScoreManager()
{
	// スコアマネージャーの宣言
	std::unique_ptr<ScoreManager> scoreManager;
	// スコアマネージャーの生成
	scoreManager = std::make_unique<ScoreManager>();
	// スコアマネージャーの初期化
	scoreManager->Initialize();
	// スコアマネージャーを返す
	return std::move(scoreManager);
}
