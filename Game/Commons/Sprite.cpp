#include "pch.h"
#include "Sprite.h"

using namespace DirectX;

Sprite::Sprite()
	: m_spriteBatch{ nullptr }
{
}

Sprite::~Sprite()
{
}

void Sprite::CreateTexture(ID3D11Device1* device, ID3D11DeviceContext1* context, const wchar_t* string)
{
	// スプライトバッチの作成
	m_spriteBatch = std::make_unique<DirectX::DX11::SpriteBatch>(context);

	// テクスチャがあるか
	if (FAILED(DirectX::CreateWICTextureFromFile(device, string, nullptr, m_texture.GetAddressOf())))
	{
		MessageBox(NULL, string, L"エラー", MB_OK);
	}
}

void Sprite::Draw(DirectX::SimpleMath::Vector2 position)
{
	m_spriteBatch->Begin();

	// スプライトを描画する
	m_spriteBatch->Draw(
		m_texture.Get(),                      // テクスチャのポインタ
		position,                             // 座標
		nullptr,                              
		DirectX::Colors::White,               // 色
		0.0f,                                 // 回転
		DirectX::SimpleMath::Vector2::Zero,   // 中心点
		1.0f,                                 // 拡大率
		DirectX::SpriteEffects_None           // 反転するか
	);

	m_spriteBatch->End();
}
