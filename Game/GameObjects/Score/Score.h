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
	// 座標
	static constexpr DirectX::SimpleMath::Vector2 POSITIONS[] =
	{
		DirectX::SimpleMath::Vector2{ 30.0f,30.0f},
		DirectX::SimpleMath::Vector2{ 1040.0f,30.0f},
	};

	// フレーム
	static constexpr Sprite::Format FREAM =
	{
		DirectX::SimpleMath::Vector2(80.0f, 10.0f),   // 座標
		DirectX::SimpleMath::Vector2(34.5f, 50.0f),   // サイズ
		1.0f                                          // 拡大率
	};

	// ボール
	static constexpr Sprite::Format BALL =
	{
		DirectX::SimpleMath::Vector2(70.0f, 80.0f),   // 座標
		DirectX::SimpleMath::Vector2(0.0f, 0.0f), // サイズ
		0.2f                                          // 拡大率
	};

	// フレームのサイズ
	static constexpr float FRAME_SIZE = 0.3f;
	// 顔のサイズ
	static constexpr float FACE_SIZE = 0.1f;
	// ボールテクスチャカウント
	static constexpr int BALLTEXTURE_COUNT = 2;


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
	Sprite m_ballTexture[BALLTEXTURE_COUNT];


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

	// ボールの画像の設定
	void SetBallTexture(int index) { m_ballTexture[index].SetTexture(Resources::GetInstance()->GetTexture(L"Ball" + std::to_wstring(m_index) + L".png")); }

	// ボール画像の初期化
	void ClearBallTexture(int index) { m_ballTexture[index].SetTexture(Resources::GetInstance()->GetTexture(L"Ball2.png")); }

// 内部処理
private:


};

