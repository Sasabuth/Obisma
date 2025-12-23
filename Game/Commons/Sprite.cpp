/// <summary>
/// Spriteに関するヘッダファイル
/// </summary>
/// <author>仲森智史</author>

// 多重インクルードの防止
#pragma once

// ヘッダファイルの読み込み
#include "pch.h"
#include "Sprite.h"

#include "Game/Commons/Collision.h"
#include "Game/Commons/Resources.h"



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
void Sprite::Draw(DirectX::SimpleMath::Vector2 position, DirectX::SimpleMath::Vector2 size, float scale, DirectX::XMVECTOR color)
{
	auto states = m_userResources->GetCommonStates();

	// 半透明の設定
	m_spriteBatch->Begin(DirectX::SpriteSortMode_Deferred, states->NonPremultiplied(), states->LinearClamp());

	// 解像度に応じた拡大率を計算
	auto const scrennSize = UserResources::GetUserResource()->GetDeviceResources()->GetOutputSize();
	float scaleX = scrennSize.right / BASE_WIDTH;
	float scaleY = scrennSize.bottom / BASE_HEIGHT;

	DirectX::SimpleMath::Vector2 pos = DirectX::SimpleMath::Vector2(position.x * scaleX, position.y * scaleY);

	// スプライトを描画する
	m_spriteBatch->Draw(
		m_texture.Get(),              // テクスチャのポインタ
		pos,                          // 座標
		nullptr,
		color,                        // 色
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
/// <param name="size">サイズ</param>
/// <param name="scale">拡大率</param>
/// <param name="color">色</param>
void Sprite::Draw(DirectX::SimpleMath::Vector2 position, DirectX::SimpleMath::Vector2 size, DirectX::SimpleMath::Vector2 scale, DirectX::XMVECTOR color)
{
	auto states = m_userResources->GetCommonStates();

	// 半透明の設定
	m_spriteBatch->Begin(DirectX::SpriteSortMode_Deferred, states->NonPremultiplied());

	// 解像度に応じた拡大率を計算
	auto const scrennSize = UserResources::GetUserResource()->GetDeviceResources()->GetOutputSize();
	float scaleX = scrennSize.right / BASE_WIDTH;
	float scaleY = scrennSize.bottom / BASE_HEIGHT;

	DirectX::SimpleMath::Vector2 pos = DirectX::SimpleMath::Vector2(position.x * scaleX, position.y * scaleY);
	scale.x *= scaleX;
	scale.y *= scaleY;

	// スプライトを描画する
	m_spriteBatch->Draw(
		m_texture.Get(),              // テクスチャのポインタ
		pos,                          // 座標
		nullptr,
		color,                        // 色
		0.0f,                         // 回転
		size / 2,                     // 中心点
		scale,                        // 拡大率
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

	DirectX::SimpleMath::Vector2 pos = DirectX::SimpleMath::Vector2(position.x * scaleX, position.y * scaleY);

	// スプライトを描画する
	m_spriteBatch->Draw(
		m_texture.Get(),                      // テクスチャのポインタ
		pos,                             // 座標
		&r,
		DirectX::Colors::White,               // 色
		0.0f,                                 // 回転
		DirectX::SimpleMath::Vector2{ 0.0f,0.0f },     // 中心点
		scale * scaleX,                                // 拡大率
		DirectX::SpriteEffects_None           // 反転するか
	);

	m_spriteBatch->End();
}



/// <summary>
/// 桁数描画
/// </summary>
/// <param name="x">X座標</param>
/// <param name="y">Y座標</param>
/// <param name="width">縦の幅</param>
/// <param name="height">横の幅</param>
/// <param name="value">数字</param>
/// <param name="size">サイズ</param>
void Sprite::DigitsDraw(float x, float y, float width, float height, int value, float size, int digits)
{
	// スコアフォントの描画
	float posX = x;
	float posY = y;

	posX += static_cast<int>(width * (digits));

	for (int i = 0; i < digits; i++)
	{
		int number = value % 10 + 1;

		int sourceX = static_cast<int>(number * width);
		Draw(DirectX::SimpleMath::Vector2(posX, posY), DirectX::SimpleMath::Vector2((float)sourceX, height), width, size);

		value /= 10;
		posX -= static_cast<int>(width);
	}
}
