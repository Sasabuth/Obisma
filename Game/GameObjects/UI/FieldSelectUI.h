/// <summary>
/// FieldSelectUIに関するヘッダファイル
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


// クラスの定義
class FieldSelectUI
{
// 定数
public:
	static constexpr int MAXSTAGE_COUNT = 3;


// 定数
private:
	// フィールド選択の数
	static constexpr int FIELDSELECT_COUNT = 4;

	// フィールド選択
	static constexpr Sprite::Format FIELDSELECT[FIELDSELECT_COUNT] =
	{
		{
			DirectX::SimpleMath::Vector2(100.0f, 360.0f),   // 座標
			DirectX::SimpleMath::Vector2(350.0f, 189.0f),  // サイズ
			0.3f                                         // 拡大率
		},

		{
			DirectX::SimpleMath::Vector2(1170.0f, 360.0f),   // 座標
			DirectX::SimpleMath::Vector2(350.0f, 189.0f),  // サイズ
			0.3f                                        // 拡大率
		},

		{
			DirectX::SimpleMath::Vector2(100.0f, 70.0f),   // 座標
			DirectX::SimpleMath::Vector2(475.0f, 260.0f),  // サイズ
			0.3f                                       // 拡大率
		},

		{
			DirectX::SimpleMath::Vector2(640.0f, 650.0f),   // 座標
			DirectX::SimpleMath::Vector2(1354.0f, 296.0f),  // サイズ
			0.3f                                       // 拡大率
		},
	};

	// メニューの色
	static constexpr DirectX::SimpleMath::Vector4 FIELDSELECT_COLORS[FIELDSELECT_COUNT] =
	{
		DirectX::SimpleMath::Vector4(0.63, 0.63f, 0.63f, 1.0f),
		DirectX::SimpleMath::Vector4(0.63, 0.63f, 0.63f, 1.0f),
		DirectX::SimpleMath::Vector4(0.63, 0.86, 1.0,1),
		DirectX::SimpleMath::Vector4(1, 0, 0,1)
	};

	static constexpr Sprite::Format STAGE =
	{
		DirectX::SimpleMath::Vector2(640.0f, 52.0f),   // 座標
		DirectX::SimpleMath::Vector2(796.0f, 271.0f),  // サイズ
		0.28f                                          // 拡大率
	};


// 変数
private:
	// ユーザーリソース
	UserResources* m_pUserResources;  

	// シーン
	Scene* m_pScene;

	// ボタン
	Button m_button[FIELDSELECT_COUNT];

	// 開いたか
	bool m_isOpen;

	// フィールドの番号
	int m_fieldIndex;

	// SE
	std::unique_ptr<DirectX::SoundEffectInstance> m_se;

	// 背景
	Sprite m_backGround;

	// ステージ
	Sprite m_stageTexture[MAXSTAGE_COUNT];


// 関数
public:
	// コンストラクタ
	FieldSelectUI();

	// デストラクタ
	~FieldSelectUI();

	// 初期化
	void Initialize(Scene* pScene);

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

	// フィールド番号の取得
	int GetFieldIndex() const { return m_fieldIndex; }
};

