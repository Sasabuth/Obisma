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
	UserResources* m_userResources;

	std::vector<std::unique_ptr<Score>> m_scores;


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
	void Add(std::unique_ptr<Score>& score) { m_scores.push_back(std::move(score)); }


// 設定/取得
public:
	int GetObjectCount() const { return (int)m_scores.size(); }
	Score* GetScore(int index) const { return m_scores[index].get(); }
};

