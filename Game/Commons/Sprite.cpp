#include "pch.h"
#include "Sprite.h"

using namespace DirectX;



/// <summary>
/// コンストラクタ
/// </summary>
Sprite::Sprite()
	: m_spriteBatch{ nullptr }
{
}



/// <summary>
/// デストラクタ
/// </summary>
Sprite::~Sprite()
{
}



/// <summary>
/// テクスチャの作成
/// </summary>
/// <param name="device">デバイス</param>
/// <param name="context">コンテキスト</param>
/// <param name="path">パス</param>
void Sprite::CreateTexture(ID3D11Device1* device, ID3D11DeviceContext1* context, const wchar_t* path)
{
	// スプライトバッチの作成
	m_spriteBatch = std::make_unique<DirectX::DX11::SpriteBatch>(context);

	// テクスチャがあるか
	if (FAILED(DirectX::CreateWICTextureFromFile(device, path, nullptr, m_texture.GetAddressOf())))
	{
		MessageBox(NULL, path, L"エラー", MB_OK);
	}
}



/// <summary>
/// 描画
/// </summary>
/// <param name="position">座標</param>
void Sprite::Draw(DirectX::SimpleMath::Vector2 position, DirectX::SimpleMath::Vector2 size, float scale)
{
	m_spriteBatch->Begin();

	// スプライトを描画する
	m_spriteBatch->Draw(
		m_texture.Get(),                      // テクスチャのポインタ
		position,                             // 座標
		nullptr,                              
		DirectX::Colors::White,               // 色
		0.0f,                                 // 回転
		size / 2,                             // 中心点
		scale,                                // 拡大率
		DirectX::SpriteEffects_None           // 反転するか
	);

	m_spriteBatch->End();
}
