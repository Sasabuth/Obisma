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
#include <cmath>


// クラスの定義
class Score
{
// 定数
private:
	static constexpr float FRAME_SIZE = 0.3f;
	static constexpr float FACE_SIZE = 0.1f;

	static constexpr DirectX::SimpleMath::Vector2 POSITIONS[] =
	{
		DirectX::SimpleMath::Vector2{ 30.0f,30.0f},
		DirectX::SimpleMath::Vector2{ 1040.0f,30.0f},
	};

	static constexpr Sprite::Format FREAM =
	{
		DirectX::SimpleMath::Vector2(80.0f, 10.0f),   // 座標
		DirectX::SimpleMath::Vector2(34.5f, 50.0f),   // サイズ
		1.0f                                          // 拡大率
	};


// 変数
private:
	// ユーザーリソース
	UserResources* m_pUserResources;

	// スコア
	float m_score;

	// インデックス
	int m_index;

	// 座標
	DirectX::SimpleMath::Vector2 m_position;

	// テクスチャ
	Sprite m_scoreTexture;
	Sprite m_frameTexture;
	Sprite m_faceTexture;


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

	// スコアを上げる
	void ScoreUp() { m_score += 1; }

	// スコアを下げる
	void ScoreDown();


// 設定/取得
public:
	// スコア
	float GetScore() const { return m_score; }  // 取得

	// 順位
	int GetIndex() const { return m_index; }  // 取得


// 内部処理
private:


};

