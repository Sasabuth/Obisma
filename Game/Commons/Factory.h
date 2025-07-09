/// <summary>
/// Factoryに関するヘッダファイル
/// </summary>
/// <author>仲森智史</author>
/// <date></date>

// 多重インクルードの防止
#pragma once

// ヘッダファイルの読み込み
#include "Game/GameObjects/Player/Player.h"
#include "Game/GameObjects/Field/Field.h"


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
	// サッカーボールを生成する
	static std::unique_ptr<Player> CreatePlayer(
		GameplayScene* pScene,
		Camera* pCamera,
		const DirectX::SimpleMath::Vector3& initialPosition
	);

	// サッカーボールを生成する
	static std::unique_ptr<Field> CreateField(
		GameplayScene* pScene
	);
};

