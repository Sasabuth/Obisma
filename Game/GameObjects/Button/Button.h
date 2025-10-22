/// <summary>
/// Buttonに関するヘッダファイル
/// </summary>
/// <author>仲森智史</author>
/// <date></date>

// 多重インクルードの防止
#pragma once

// ヘッダファイルの読み込み
#include "Game/Commons/Collision.h"
#include "Game/Commons/Sprite.h"


// クラスの定義
class Button
{
// 変数
private:
	Sprite m_sprite;  // スプライト
	BoxCollider2D m_collider; // コライダー

	std::function<void()> m_clickFunc;  // クリックしたときの関数

	std::unique_ptr<DirectX::SoundEffectInstance> m_se;  // SE

// 関数
public:
	// コンストラクタ
	Button();

	// デストラクタ
	~Button();

	// 描画
	void Draw(DirectX::XMVECTOR color = DirectX::Colors::White);

	// クリック
	void Click();


// 設定/取得
public:
	const Sprite& GetSprite() { return m_sprite; }
	const BoxCollider2D& GetCollider() { return m_collider; }

	// テクスチャの設定
	void SetTexture(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture) { m_sprite.SetTexture(texture); }  // 設定

	// 座標の設定
	void SetPosition(DirectX::SimpleMath::Vector2 pos) { m_collider.SetPosition(pos); }

	// サイズの設定
	void SetSize(DirectX::SimpleMath::Vector2 size) { m_collider.SetSize(size); }

	// 拡大率の設定
	void SetScale(float scale) { m_collider.SetScale(scale); }

	// 関数の設定
	void SetFunc(std::function<void()> func) { m_clickFunc = func; }
};

