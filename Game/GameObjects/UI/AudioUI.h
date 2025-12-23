/// <summary>
/// AudioUIに関するヘッダファイル
/// </summary>
/// <author>仲森智史</author>
/// <date></date>

// 多重インクルードの防止
#pragma once

// ヘッダファイルの読み込み
#include "Game/Commons/Sprite.h"
#include "Game/Commons/Collision.h"
#include "Game/Commons/UserResources.h"
#include "Game/GameObjects/Button/Button.h"


// クラスの定義
class AudioUI
{
// 定数
private:
	// バーの数
	static constexpr int BAR_COUNT = 2;

	// バーの座標
	static constexpr DirectX::SimpleMath::Vector2 BAR_POSITIONS[BAR_COUNT] =
	{
		DirectX::SimpleMath::Vector2(600.0f, 340.0f),
		DirectX::SimpleMath::Vector2(600.0f, 462.0f),
	};

	// バーのサイズ
	static constexpr DirectX::SimpleMath::Vector2 BAR_SIZE = DirectX::SimpleMath::Vector2(25.0f, 40.0f);

	// バーの座標の上限
	static constexpr float BAR_MINPOS = 542.0f;
	static constexpr float BAR_MAXPOS = 900.0f;

	// UI
	static constexpr Sprite::Format UI =
	{
		DirectX::SimpleMath::Vector2(640.0f, 360.0f),   // 座標
		DirectX::SimpleMath::Vector2(1743.0f, 850.0f),  // サイズ
		0.4f                                            // 拡大率
	};

	// ボタン
	static constexpr Sprite::Format BUTTON =
	{
		DirectX::SimpleMath::Vector2(100.0f, 70.0f),   // 座標
		DirectX::SimpleMath::Vector2(475.0f, 260.0f),  // サイズ
		0.3f                                           // 拡大率
	};

	// 隠す
	static constexpr Sprite::Format HIDE =
	{
		DirectX::SimpleMath::Vector2(640.0f, 360.0f),   // 座標
		DirectX::SimpleMath::Vector2(216.0f, 157.0f),  // サイズ
		6.0f                                         // 拡大率
	};


// 変数
private:
	// ユーザーリソース
	UserResources* m_pUserResources;

	// オーディオUI
	Sprite m_audioUI;

	// コライダー
	BoxCollider2D m_uiCollider;
	BoxCollider2D m_barCollider[BAR_COUNT];

	// ボタン
	Button m_button;

	// 隠す
	Sprite m_hideTexture;

	// 当たったか
	bool m_isHit[BAR_COUNT];

	// 開いたか
	bool m_isOpen;

	// SE
	std::unique_ptr<DirectX::SoundEffectInstance> m_se;


// 関数
public:
	// コンストラクタ
	AudioUI();

	// デストラクタ
	~AudioUI();

	// 初期化
	void Initialize();

	// 更新
	void Update(const BoxCollider2D& collider);

	// 描画
	void Draw(const BoxCollider2D& collider);

	// 終了処理
	void Finalize();

	// クリック
	void Click() { m_isOpen = !m_isOpen; }


// 設定/取得
public:
	// オープンしたか
	bool IsOpen() { return m_isOpen; }
};

