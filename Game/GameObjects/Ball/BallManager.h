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
class BallManager
{
public:


// 変数
private:
	// フィールド
	Field* m_pField;

	// ボールの配列
	std::vector<std::unique_ptr<Ball>> m_balls;


// 関数
public:
	// コンストラクタ
	BallManager(Field* pField);

	// デストラクタ
	~BallManager();

	// 初期化
	void Initialize(int ballCount);

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
	// オブジェクトの数の取得
	int GetObjectCount() const { return (int)m_balls.size(); }

	// ボールの取得
	Ball* GetBall(int index) const { return m_balls[index].get(); }


// 内部処理
private:
};

