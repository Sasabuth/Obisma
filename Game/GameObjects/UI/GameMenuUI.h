/// <summary>
/// GameMenuUIに関するヘッダファイル
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
class AudioUI;


// クラスの定義
class GameMenuUI
{
// 定数
private:
	// ゲームメニューの数
	static constexpr int GAMEMENU_COUNT = 3;

	// ゲームメニュー
	static constexpr Sprite::Format GAMEMENU[GAMEMENU_COUNT] =
	{
		{
			DirectX::SimpleMath::Vector2(640.0f, 200.0f),   // 座標
			DirectX::SimpleMath::Vector2(1224.0f, 261.0f),  // サイズ
			0.28f                                         // 拡大率
		},
		
		{
			DirectX::SimpleMath::Vector2(640.0f, 360.0f),   // 座標
			DirectX::SimpleMath::Vector2(940.0f, 261.0f),  // サイズ
			0.28f                                         // 拡大率
		},

		{
			DirectX::SimpleMath::Vector2(640.0f, 520.0f),   // 座標
			DirectX::SimpleMath::Vector2(1391.0f, 261.0f),  // サイズ
			0.28f                                         // 拡大率
		},
	};

	// 隠す
	static constexpr Sprite::Format HIDE =
	{
		DirectX::SimpleMath::Vector2(640.0f, 360.0f),   // 座標
		DirectX::SimpleMath::Vector2(216.0f, 157.0f),  // サイズ
		6.0f                                         // 拡大率
	};


	// メニューの色
	static constexpr DirectX::SimpleMath::Vector4 MENU_COLORS = DirectX::SimpleMath::Vector4(1, 0, 0, 1);


// 変数
private:
	// ユーザーリソース
	UserResources* m_pUserResources;  

	// ボタン
	Button m_button[GAMEMENU_COUNT];

	// 開いたか
	bool m_isOpen;

	// 隠す
	Sprite m_hideTexture;

	// SE
	std::unique_ptr<DirectX::SoundEffectInstance> m_se;


// 関数
public:
	// コンストラクタ
	GameMenuUI();

	// デストラクタ
	~GameMenuUI();

	// 初期化
	void Initialize(AudioUI* audioUI);

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

