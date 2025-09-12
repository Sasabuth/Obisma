/// <summary>
/// Ballに関するヘッダファイル
/// </summary>
/// <author>仲森智史</author>

// 多重インクルードの防止
#pragma once

// ヘッダファイルの読み込み
#include "Game/Commons/Interface/IEntity.h"
#include "Game/Commons/Interface/IState.h"
#include "Game/Commons/Collision.h"
#include "Game/Commons/UserResources.h"
#include "Game/GameObjects/Ball/Ball.h"
#include <vector>


// クラスの定義
class GameplayScene;
class Camera;


// クラスの定義
class BallManager
{
public:
	static constexpr int BALL_COUNT = 4;  // ボールの数

	// ボールの座標
	static constexpr DirectX::SimpleMath::Vector3 BALL_POSITIONS[] =
	{ 
		DirectX::SimpleMath::Vector3{ 3.0f, 0.1f, 0.1f },
		DirectX::SimpleMath::Vector3{-3.0f, 0.1f, 0.1f },
		DirectX::SimpleMath::Vector3{ 0.1f, 0.1f, 3.0f },
		DirectX::SimpleMath::Vector3{ 0.1f, 0.1f,-3.0f },
	};

// 変数
private:
	UserResources* m_userResources;
	GameplayScene* m_pScene;

	std::vector<std::unique_ptr<Ball>> m_balls;

// 関数
public:
	// コンストラクタ
	BallManager(GameplayScene* pScene);

	// デストラクタ
	~BallManager();

	// 初期化
	void Initialize();

	// 更新
	void Update(float elapsedTime);

	// 描画
	void Render();

	// 終了処理
	void Finalize();

	// 追加
	void Add(std::unique_ptr<Ball>& ball) { m_balls.push_back(std::move(ball)); }


// 設定/取得
public:
	int GetObjectCount() const { return (int)m_balls.size(); }
	Ball* GetBall(int index) const { return m_balls[index].get(); }


// 内部処理
private:
};

