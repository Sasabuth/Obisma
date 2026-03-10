/// <summary>
/// Spriteに関するヘッダファイル
/// </summary>
/// <author>仲森智史</author>

// 多重インクルードの防止
#pragma once

// ヘッダファイルの読み込み
#include "Game/Commons/UserResources.h"
#include "Game/Commons/Collision.h"



// クラスの定義
class Sprite
{
// 構造体
public:
	// フォーマット
	struct Format
	{
		DirectX::SimpleMath::Vector2 pos;   // 座標
		DirectX::SimpleMath::Vector2 size;  // サイズ
		float scale;                        // スケール
	};


// 定数
private:
	// ベースの横幅
	static constexpr float BASE_WIDTH = 1280.0f;
	// ベースの縦幅
	static constexpr float BASE_HEIGHT = 720.0f;

private:
	// ユーザーリソース
	UserResources* m_userResources;

	// スプライトバッチ
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;

	// テクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;


public:
	// コンストラクタ
	Sprite();

	// デストラクタ
	~Sprite();

	// テクスチャの作成
	void CreateTexture(const wchar_t* path);

	// 描画
	void Draw(DirectX::SimpleMath::Vector2 position, DirectX::SimpleMath::Vector2 size, float scale, DirectX::XMVECTOR color = DirectX::Colors::White);
	void Draw(DirectX::SimpleMath::Vector2 position, DirectX::SimpleMath::Vector2 size, DirectX::SimpleMath::Vector2 scale, DirectX::XMVECTOR color = DirectX::Colors::White);
	void Draw(DirectX::SimpleMath::Vector2 position, DirectX::SimpleMath::Vector2 rect, float width, float scale);

	// 桁数描画
	void DigitsDraw(float x, float y, float width, float height, int value, float size, int digits = 1);


// 設定/取得
public:
	// テクスチャの設定
	void SetTexture(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture) { m_texture = texture; }  // 設定
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetTexture() const { return m_texture; }           // 取得
};
