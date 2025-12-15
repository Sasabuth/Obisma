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

