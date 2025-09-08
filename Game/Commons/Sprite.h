/// <summary>
/// Spriteに関するヘッダファイル
/// </summary>
/// <author>仲森智史</author>

// 多重インクルードの防止
#pragma once

// ヘッダファイルの読み込み
#include "Game/Commons/UserResources.h"

// クラスの定義
class Sprite
{
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

	// テクスチャの設定
	void SetTexture(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture) { m_texture = texture; }

	// 描画
	void Draw(DirectX::SimpleMath::Vector2 position, DirectX::SimpleMath::Vector2 size, float scale);
	void Draw(DirectX::SimpleMath::Vector2 position, DirectX::SimpleMath::Vector2 rect, float width, float scale);
};
