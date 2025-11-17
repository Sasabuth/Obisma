/// <summary>
/// ScoreManagerに関するヘッダファイル
/// </summary>
/// <author>仲森智史</author>


// 多重インクルードの防止
#pragma once

// ヘッダファイルの読み込み
#include "Game/Commons/UserResources.h"
#include "Game/Commons/Sprite.h"

#include "Game/GameObjects/Score/Score.h"


// クラスの定義
class ScoreManager
{
// 定数
private:


// 変数
private:
	// スコアの箱
	std::vector<Score*> m_scores;

	// 引き分けたか
	bool m_isDraw;


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

	// 追加
	void Add(Score* score) { m_scores.push_back(score); }

	// 高い順に並べる
	void SortRank();


// 設定/取得
public:
	// スコアの数を取得
	int GetObjectCount() const { return (int)m_scores.size(); }
	// スコアのポインタを取得
	Score* GetScore(int index) const { return m_scores[index]; }

	// ランキングの取得
	int GetRank(int index) const { return m_scores[index]->GetIndex(); };

	// 引き分けか
	bool GetIsDraw() const { return m_isDraw; }
};

