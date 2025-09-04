#pragma once

class Sprite
{
private:
	// スプライトバッチ
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;

	//  エフェクト 
	std::unique_ptr<DirectX::AlphaTestEffect> m_batchEffect;

	// テクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;


public:
	// コンストラクタ
	Sprite();

	// デストラクタ
	~Sprite();

	// テクスチャの作成
	void CreateTexture(ID3D11Device1* device, ID3D11DeviceContext1* context, const wchar_t* path);

	// 描画
	void Draw(DirectX::SimpleMath::Vector2 position, DirectX::SimpleMath::Vector2 size, float scale);

};
