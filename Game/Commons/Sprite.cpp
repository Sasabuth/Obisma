/// <summary>
/// Spriteに関するヘッダファイル
/// </summary>
/// <author>仲森智史</author>

// 多重インクルードの防止
#pragma once

// ヘッダファイルの読み込み
#include "pch.h"
#include "Sprite.h"


// 名前の省略
using namespace DirectX;


/// <summary>
/// コンストラクタ
/// </summary>
Sprite::Sprite()
	: m_spriteBatch{ nullptr }
{
	// 
	m_userResources = UserResources::GetUserResource();

	auto context = m_userResources->GetDeviceResources()->GetD3DDeviceContext();

	// スプライトバッチの作成
	m_spriteBatch = std::make_unique<DirectX::DX11::SpriteBatch>(context);
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
void Sprite::CreateTexture(const wchar_t* path)
{
	auto device = m_userResources->GetDeviceResources()->GetD3DDevice();

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
/// <param name="size">サイズ</param>
/// <param name="scale">拡大率</param>
void Sprite::Draw(DirectX::SimpleMath::Vector2 position, DirectX::SimpleMath::Vector2 size, float scale)
{
	auto states = m_userResources->GetCommonStates();

	// 半透明の設定
	m_spriteBatch->Begin(DirectX::SpriteSortMode_Deferred, states->NonPremultiplied());

	// 解像度に応じた拡大率を計算
	auto const scrennSize = UserResources::GetUserResource()->GetDeviceResources()->GetOutputSize();
	float scaleX = scrennSize.right / BASE_WIDTH;
	float scaleY = scrennSize.bottom / BASE_HEIGHT;

	SimpleMath::Vector2 pos = SimpleMath::Vector2(position.x * scaleX, position.y * scaleY);

	// スプライトを描画する
	m_spriteBatch->Draw(
		m_texture.Get(),              // テクスチャのポインタ
		pos,                          // 座標
		nullptr,
		DirectX::Colors::White,       // 色
		0.0f,                         // 回転
		size / 2,                     // 中心点
		scale * scaleX,               // 拡大率
		DirectX::SpriteEffects_None   // 反転するか
	);

	m_spriteBatch->End();
}



/// <summary>
/// 描画
/// </summary>
/// <param name="position">座標</param>
/// <param name="rect">切り取り</param>
/// <param name="size">サイズ</param>
/// <param name="scale">拡大率</param>
void Sprite::Draw(DirectX::SimpleMath::Vector2 position, DirectX::SimpleMath::Vector2 rect, float width, float scale)
{
	auto states = m_userResources->GetCommonStates();

	// 半透明の設定
	m_spriteBatch->Begin(DirectX::SpriteSortMode_Deferred, states->NonPremultiplied());

	// 数字を切り取る
	RECT r = { (LONG)(rect.x - width), 0, (LONG)rect.x, (LONG)rect.y };

	// 解像度に応じた拡大率を計算
	auto const scrennSize = UserResources::GetUserResource()->GetDeviceResources()->GetOutputSize();
	float scaleX = scrennSize.right  / BASE_WIDTH;
	float scaleY = scrennSize.bottom / BASE_HEIGHT;

	SimpleMath::Vector2 pos = SimpleMath::Vector2(position.x * scaleX, position.y * scaleY);

	// スプライトを描画する
	m_spriteBatch->Draw(
		m_texture.Get(),                      // テクスチャのポインタ
		pos,                             // 座標
		&r,
		DirectX::Colors::White,               // 色
		0.0f,                                 // 回転
		SimpleMath::Vector2{ 0.0f,0.0f },     // 中心点
		scale* scaleX,                                // 拡大率
		DirectX::SpriteEffects_None           // 反転するか
	);

	m_spriteBatch->End();
}
