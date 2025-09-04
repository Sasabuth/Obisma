/// <summary>
/// DebugFontに関するヘッダファイル
/// </summary>
/// <author>仲森智史</author>

// 多重インクルードの防止
#pragma once

// ヘッダファイルの読み込み
#include "pch.h"
#include "DebugFont.h"
#include "DirectXHelpers.h"
#include "VertexTypes.h"


// 名前の省略
using namespace DirectX;



/// <summary>
/// コンストラクタ
/// </summary>
/// <param name="device">デバイス</param>
/// <param name="context">コンテキスト</param>
/// <param name="fileName">ファイル名</param>
DebugFont::DebugFont(ID3D11Device* device, ID3D11DeviceContext* context, wchar_t const* fileName)
	: m_fontHeight{}
	, m_states{ nullptr }
	, m_fString{ L"-1" }
	, m_count{ 0 }
{
	m_spriteBatch = std::make_unique<SpriteBatch>(context);
	m_spriteFont = std::make_unique<DirectX::SpriteFont>(device, fileName);

	// フォントの縦サイズを取得する
	SimpleMath::Vector2 textSize = m_spriteFont->MeasureString("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
	m_fontHeight = textSize.y;
}



/// <summary>
/// デストラクタ
/// </summary>
DebugFont::~DebugFont()
{
	m_spriteFont.reset();
	m_spriteBatch.reset();
}



/// <summary>
/// 初期化
/// </summary>
void DebugFont::Initialize()
{
	m_fString = L"-1";
	m_count = 0;
}

//// 描画する文字列を登録する関数
//void DebugFont::AddString(const wchar_t * string, DirectX::SimpleMath::Vector2 pos, FXMVECTOR color, float scale)
//{
//	String str;
//
//	str.string = std::wstring(string);
//	str.pos = pos;
//	str.color = color;
//	str.scale = scale;
//
//	m_strings.push_back(str);
//}
//
//// 描画関数
//void DebugFont::Render(DirectX::CommonStates* states)
//{
//	m_spriteBatch->Begin(SpriteSortMode_Deferred, nullptr, nullptr, states->DepthNone(), states->CullCounterClockwise());
//	
//	for (size_t i = 0; i < m_strings.size(); i++)
//	{
//		m_spriteFont->DrawString(
//			m_spriteBatch.get(),
//			m_strings[i].string.c_str(),
//			m_strings[i].pos,
//			m_strings[i].color,
//			0.0f,
//			SimpleMath::Vector2(0.0f, 0.0f),
//			m_strings[i].scale);
//	}
//
//	m_spriteBatch->End();
//
//	// 登録されている文字列をクリア
//	m_strings.clear();
//}



/// <summary>
/// 描画処理
/// </summary>
/// <param name="string">文字</param> 
/// <param name="color">色</param>　
/// <param name="scale">サイズ</param>　
void DebugFont::Render(const wchar_t* string, DirectX::FXMVECTOR color, float scale)
{
	if (wcscmp(m_fString, L"-1") == 0 || wcscmp(m_fString, string) == 0)
	{
		m_fString = string;
		m_count = 0;
	}
	else
	{
		m_count += 1;
	}

	String str;

	str.string = std::wstring(string);
	str.pos = DirectX::SimpleMath::Vector2(0.0f, m_count * 25.0f);
	str.color = color;
	str.scale = scale;

	m_strings.push_back(str);

	m_spriteBatch->Begin(DirectX::SpriteSortMode_Deferred, nullptr, nullptr, m_states->DepthNone(), m_states->CullCounterClockwise());

	for (size_t i = 0; i < m_strings.size(); i++)
	{
		m_spriteFont->DrawString(
			m_spriteBatch.get(),
			m_strings[i].string.c_str(),
			m_strings[i].pos,
			m_strings[i].color,
			0.0f,
			DirectX::SimpleMath::Vector2(0.0f, 0.0f),
			m_strings[i].scale);
	}

	m_spriteBatch->End();

	// 登録されている文字列をクリア
	m_strings.clear();
}



/// <summary>
/// 
/// </summary>
/// <param name="string">文字</param>
/// <param name="pos">座標</param>
/// <param name="color">色</param>
/// <param name="scale">サイズ</param>
void DebugFont::Render(const wchar_t* string, DirectX::SimpleMath::Vector3 pos, DirectX::FXMVECTOR color, float scale)
{
	if (wcscmp(m_fString, L"-1") == 0 || wcscmp(m_fString, string) == 0)
	{
		m_fString = string;
		m_count = 0;
	}
	else
	{
		m_count += 1;
	}

	String str;

	str.string = std::wstring(string) + L"={" + std::to_wstring(pos.x) + L"," + std::to_wstring(pos.y) + L"," + std::to_wstring(pos.z) + L"}";
	str.pos = DirectX::SimpleMath::Vector2(0.0f, m_count * 25.0f);
	str.color = color;
	str.scale = scale;

	m_strings.push_back(str);

	m_spriteBatch->Begin(DirectX::SpriteSortMode_Deferred, nullptr, nullptr, m_states->DepthNone(), m_states->CullCounterClockwise());

	for (size_t i = 0; i < m_strings.size(); i++)
	{
		m_spriteFont->DrawString(
			m_spriteBatch.get(),
			m_strings[i].string.c_str(),
			m_strings[i].pos,
			m_strings[i].color,
			0.0f,
			DirectX::SimpleMath::Vector2(0.0f, 0.0f),
			m_strings[i].scale);
	}

	m_spriteBatch->End();

	// 登録されている文字列をクリア
	m_strings.clear();
}



/// <summary>
/// 描画
/// </summary>
/// <param name="string">文字列</param>
/// <param name="rotate">クオータニオン</param>
/// <param name="color">色</param>
/// <param name="scale">拡大率</param>
void DebugFont::Render(const wchar_t* string, DirectX::SimpleMath::Quaternion rotate, DirectX::FXMVECTOR color, float scale)
{
	if (wcscmp(m_fString, L"-1") == 0 || wcscmp(m_fString, string) == 0)
	{
		m_fString = string;
		m_count = 0;
	}
	else
	{
		m_count += 1;
	}

	String str;

	str.string = std::wstring(string) + L"={" + std::to_wstring(rotate.x) + L"," + std::to_wstring(rotate.y) + L"," + std::to_wstring(rotate.z) + L"," + std::to_wstring(rotate.w) + L"}";
	str.pos = DirectX::SimpleMath::Vector2(0.0f, m_count * 25.0f);
	str.color = color;
	str.scale = scale;

	m_strings.push_back(str);

	m_spriteBatch->Begin(DirectX::SpriteSortMode_Deferred, nullptr, nullptr, m_states->DepthNone(), m_states->CullCounterClockwise());

	for (size_t i = 0; i < m_strings.size(); i++)
	{
		m_spriteFont->DrawString(
			m_spriteBatch.get(),
			m_strings[i].string.c_str(),
			m_strings[i].pos,
			m_strings[i].color,
			0.0f,
			DirectX::SimpleMath::Vector2(0.0f, 0.0f),
			m_strings[i].scale);
	}

	m_spriteBatch->End();

	// 登録されている文字列をクリア
	m_strings.clear();
}



/// <summary>
/// 共通ステートの設定
/// </summary>
/// <param name="states"></param> 共通ステート
void DebugFont::SetCommonStates(DirectX::CommonStates* states)
{
	m_states = states;
}



// コンストラクタ
DebugFont3D::DebugFont3D(ID3D11Device* device, ID3D11DeviceContext* context, wchar_t const* fileName)
	: DebugFont(device, context, fileName)
{	
	// エフェクトを作成
	m_effect = std::make_unique<BasicEffect>(device);
	m_effect->SetTextureEnabled(true);
	m_effect->SetVertexColorEnabled(true);
	m_effect->SetLightingEnabled(false);

	// 入力レイアウトを作成
	DX::ThrowIfFailed(
		CreateInputLayoutFromEffect(
			device,
			m_effect.get(),
			VertexPositionColorTexture::InputElements,
			VertexPositionColorTexture::InputElementCount,
			m_inputLayout.ReleaseAndGetAddressOf())
	);
}

// デストラクタ
DebugFont3D::~DebugFont3D()
{
	m_inputLayout.Reset();
	m_effect.reset();
}

// 描画する文字列を登録する関数（3D版）
void DebugFont3D::AddString(
	const wchar_t* string,
	DirectX::SimpleMath::Vector3 pos,
	DirectX::FXMVECTOR color,
	float scale)
{
	String str;

	str.string = std::wstring(string);
	str.pos = pos;
	str.color = color;
	// 文字の高さが3D空間内で１になるよう調整している（余白があるのできっちりではない）
	str.scale = scale / m_fontHeight;

	m_strings.push_back(str);
}

// 描画関数（3D版）
void DebugFont3D::Render(
	ID3D11DeviceContext* context,
	DirectX::CommonStates* states,
	const DirectX::SimpleMath::Matrix& view,
	const DirectX::SimpleMath::Matrix& proj)
{
	// スクリーン座標はY軸が＋－逆なので
	SimpleMath::Matrix invertY = SimpleMath::Matrix::CreateScale(1.0f, -1.0f, 1.0f);

	// ビュー行列の回転を打ち消す行列を作成する
	SimpleMath::Matrix invView = view.Invert();
	invView._41 = 0.0f;
	invView._42 = 0.0f;
	invView._43 = 0.0f;

	// エフェクトにビュー行列と射影行列を設定する
	m_effect->SetView(view);
	m_effect->SetProjection(proj);

	for (size_t i = 0; i < m_strings.size(); i++)
	{
		m_spriteBatch->Begin(SpriteSortMode_Deferred, nullptr, nullptr, states->DepthNone(), states->CullCounterClockwise(), [=]
			{
				// ワールド行列作成
				SimpleMath::Matrix world = invertY * invView * SimpleMath::Matrix::CreateTranslation(m_strings[i].pos);
				// エフェクトを適応する
				m_effect->SetWorld(world);
				m_effect->Apply(context);
				// 入力レイアウトを設定する
				context->IASetInputLayout(m_inputLayout.Get());
			}
		);

		// 文字列の中心が表示位置になるように設定
		SimpleMath::Vector2 textOrigin = m_spriteFont->MeasureString(m_strings[i].string.c_str()) / 2.0f;

		m_spriteFont->DrawString(
			m_spriteBatch.get(),
			m_strings[i].string.c_str(),
			SimpleMath::Vector2::Zero,
			m_strings[i].color,
			0.0f,
			textOrigin,
			m_strings[i].scale);

		m_spriteBatch->End();
	}

	// 登録されている文字列をクリア
	m_strings.clear();
}
