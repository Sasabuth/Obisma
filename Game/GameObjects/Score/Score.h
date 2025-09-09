/// <summary>
/// Scoreに関するヘッダファイル
/// </summary>
/// <author>仲森智史</author>


// 多重インクルードの防止
#pragma once

// ヘッダファイルの読み込み
#include "Game/Commons/UserResources.h"
#include "Game/Commons/Sprite.h"
#include "Game/Commons/Resources.h"


// クラスの定義
class Score
{
// 定数
private:
	static constexpr float NUMBER_WIDTH = 34.5f;
	static constexpr int NUMBER_HEIGHT = 50;

	static constexpr DirectX::SimpleMath::Vector2 POSITIONS[] =
	{
		DirectX::SimpleMath::Vector2{ 30.0f,30.0f},
		DirectX::SimpleMath::Vector2{ 1040.0f,30.0f},
	};


// 変数
private:
	UserResources* m_userResources;

	int m_score;

	DirectX::SimpleMath::Vector2 m_position;

	Sprite m_scoreSprite;
	Sprite m_frameSprite;
	Sprite m_faceSprite;


// 関数
public:
	// コンストラクタ
	Score();

	// デストラクタ
	~Score();

	// 初期化
	void Initialize(int index);

	// 描画
	void Render();


// 設定/取得
public:
	// スコアの設定
	void SetScore();


// 内部処理
private:
	// スコア描画
	void ScoreDraw(int x, int y, int score, int size);

};

