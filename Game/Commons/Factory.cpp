/// <summary>
/// XXXXに関するソースファイル
/// </summary>
/// <author>仲森智史</author>
/// <date></date>

// 多重インクルードの防止
#pragma once

// ヘッダファイルの読み込み
#include "pch.h"
#include "Factory.h"

// 名前の省略
using namespace DirectX;

std::unique_ptr<Player> Factory::CreatePlayer(GameplayScene* pScene, BallManager* ballManager, const DirectX::SimpleMath::Vector3& initialPosition)
{
	// プレイヤーを宣言
	std::unique_ptr<Player> player;
	// プレイヤーを生成
	player = std::make_unique<Player>(pScene, ballManager);
	// プレイヤーを初期化
	player->Initialize(initialPosition);
	// プレイヤーを返す
	return std::move(player);
}

std::unique_ptr<Enemy> Factory::CreateEnemy(GameplayScene* pScene, BallManager* ballManager, const DirectX::SimpleMath::Vector3& initialPosition)
{
	// 敵を宣言
	std::unique_ptr<Enemy> enemy;
	// 敵を生成
	enemy = std::make_unique<Enemy>(pScene, ballManager);
	// 敵を初期化
	enemy->Initialize(initialPosition);
	// 敵を返す
	return std::move(enemy);
}

std::unique_ptr<CameraUp> Factory::CreateCameraUp(Player* player, const DirectX::SimpleMath::Vector3& initialPosition)
{
	// カメラの上向きベクトルを宣言
	std::unique_ptr<CameraUp> cameraUp;
	// カメラの上向きベクトルを生成
	cameraUp = std::make_unique<CameraUp>(player);
	// カメラの上向きベクトルを初期化
	cameraUp->Initialize(initialPosition);
	// カメラの上向きベクトルを返す
	return std::move(cameraUp);
}

std::unique_ptr<Field> Factory::CreateField(GameplayScene* pScene)
{
	// フィールドの宣言
	std::unique_ptr<Field> field;
	// フィールドの生成
	field = std::make_unique<Field>(pScene);
	// フィールドの初期化
	field->Initialize();
	// フィールドを返す
	return std::move(field);
}

std::unique_ptr<Ball> Factory::CreateBall(GameplayScene* pScene, const DirectX::SimpleMath::Vector3& initialPosition)
{
	// ボールの宣言
	std::unique_ptr<Ball> ball;
	// ボールの生成
	ball = std::make_unique<Ball>(pScene);
	// ボールの初期化
	ball->Initialize(initialPosition);
	// ボールを返す
	return std::move(ball);
}

std::unique_ptr<BallManager> Factory::CreateBallManager(GameplayScene* pScene)
{
	// ボールマネージャーの宣言
	std::unique_ptr<BallManager> ballManager;
	// ボールマネージャーの生成
	ballManager = std::make_unique<BallManager>(pScene);
	// ボールマネージャーの初期化
	ballManager->Initialize();
	// ボールマネージャーを返す
	return std::move(ballManager);
}
