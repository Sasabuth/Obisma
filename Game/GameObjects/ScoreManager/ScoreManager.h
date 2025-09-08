/// <summary>
/// ScoreManagerに関するヘッダファイル
/// </summary>
/// <author>仲森智史</author>


// 多重インクルードの防止
#pragma once

// ヘッダファイルの読み込み
#include "Game/Commons/UserResources.h"
#include "Game/Commons/Sprite.h"


// クラスの定義
class ScoreManager
{
// 定数
private:
	static constexpr float NUMBER_WIDTH = 34.5f;
	static constexpr int NUMBER_HEIGHT = 50;


// 変数
private:
	UserResources* m_userResources;

	int m_score;

	Sprite m_sprite;
	Sprite m_frameSprite;


// 関数
public:
	// コンストラクタ
	ScoreManager();

	// デストラクタ
	~ScoreManager();

	// 初期化
	void Initialize();

	// 更新
	void Update(float elapsedTime);

	// 描画
	void Render();

	// 終了処理
	void Finalize();

	// スコア
	void SetScore(int ballColorNum);

	// スコア描画
	void ScoreDraw(int x, int y, int score, int size);
};

