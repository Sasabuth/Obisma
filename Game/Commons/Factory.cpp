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

std::unique_ptr<Player> Factory::CreatePlayer(GameplayScene* pScene, Camera* pCamera, const DirectX::SimpleMath::Vector3& initialPosition)
{
	// プレイヤーを宣言
	std::unique_ptr<Player> player;
	// プレイヤーを生成
	player = std::make_unique<Player>(pScene, pCamera);
	// プレイヤーを初期化
	player->Initialize();
	// プレイヤーを返す
	return std::move(player);
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
