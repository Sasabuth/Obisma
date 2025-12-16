/// <summary>
/// MenuUIに関するヘッダファイル
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
class Scene;
class FieldSelectUI;


// クラスの定義
class MenuUI
{
// 定数
private:
	static constexpr int MENU_COUNT = 3;

	// メニューの座標
	static constexpr DirectX::SimpleMath::Vector2 MENU_POSITIONS[MENU_COUNT] =
	{
		DirectX::SimpleMath::Vector2(250.0f, 360.0f),
		DirectX::SimpleMath::Vector2(600.0f, 360.0f),
		DirectX::SimpleMath::Vector2(100.0f, 70.0f),
	};

	// メニューのサイズ
	static constexpr DirectX::SimpleMath::Vector2 MENU_SIZE = DirectX::SimpleMath::Vector2(475.0f, 260.0f);

	// 拡大率
	static constexpr float MENU_SCALES[MENU_COUNT] =
	{
	    0.5f,
	    0.5f,
	    0.3f,
	};

	// メニューの色
	static constexpr DirectX::SimpleMath::Vector4 MENU_COLORS[MENU_COUNT] =
	{
		DirectX::SimpleMath::Vector4(1,0,0,1),
		DirectX::SimpleMath::Vector4(1,0,0,1),
		DirectX::SimpleMath::Vector4(0.63, 0.86, 1.0,1)
	};


// 変数
private:
	// ユーザーリソース
	UserResources* m_pUserResources;  

	// シーン
	Scene* m_pScene;

	// ボタン
	Button m_button[MENU_COUNT];

	// 開いたか
	bool m_isOpen;

	// SE
	std::unique_ptr<DirectX::SoundEffectInstance> m_se;


// 関数
public:
	// コンストラクタ
	MenuUI();

	// デストラクタ
	~MenuUI();

	// 初期化
	void Initialize(Scene* pScene, FieldSelectUI* fieldSelectUI);

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

