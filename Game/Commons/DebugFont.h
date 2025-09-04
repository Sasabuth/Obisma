/// <summary>
/// DebugFontに関するヘッダファイル
/// </summary>
/// <author>仲森智史</author>

// 多重インクルードの防止
#pragma once

// ヘッダファイルの読み込み
#include <vector>
#include <string>
#include "SimpleMath.h"
#include "CommonStates.h"
#include "Effects.h"
#include "SpriteBatch.h"
#include "SpriteFont.h"
#include <wchar.h>


// クラスの定義
class DebugFont
{
private:

	// 文字列情報
	struct String
	{
		// 位置
		DirectX::SimpleMath::Vector2 pos;

		// 文字列
		std::wstring string;

		// 色
		DirectX::SimpleMath::Color color;

		// スケール
		float scale = 1.0f;
	};

	// 表示文字列の配列S
	std::vector<String> m_strings;

	// 共通ステート
	DirectX::CommonStates* m_states;

	const wchar_t* m_fString;

	int m_count;

protected:

	// スプライトバッチ
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;

	// スプライトフォント
	std::unique_ptr<DirectX::SpriteFont> m_spriteFont;

	// フォントの縦サイズ
	float m_fontHeight;

public:

	// コンストラクタ
	DebugFont(ID3D11Device* device, ID3D11DeviceContext* context, wchar_t const* fileName);

	// デストラクタ
	virtual ~DebugFont();

	// 初期化
	void Initialize();

	// 描画する文字列を登録する関数
	//void AddString(
	//	const wchar_t* string,
	//	DirectX::SimpleMath::Vector2 pos,
	//	DirectX::FXMVECTOR color = DirectX::Colors::White,
	//	float scale = 1.0f);

	//// 描画関数
	//void Render(DirectX::CommonStates* states);

	// 描画処理
	void Render(const wchar_t* string, DirectX::FXMVECTOR color = DirectX::Colors::White, float scale = 1.0f);
	void Render(const wchar_t* string, DirectX::SimpleMath::Vector3 pos, DirectX::FXMVECTOR color = DirectX::Colors::White, float scale = 1.0f);
	void Render(const wchar_t* string, DirectX::SimpleMath::Quaternion rotate, DirectX::FXMVECTOR color = DirectX::Colors::White, float scale = 1.0f);

	template<typename T>
	void Render(const wchar_t* string, T num, DirectX::FXMVECTOR color = DirectX::Colors::White, float scale = 1.0f)
	{
		// カウントを増やす
		m_count += 1;

		if (wcscmp(m_fString, L"-1") == 0 || wcscmp(m_fString, string) == 0)
		{
			m_fString = string;
			m_count = 0;
		}

		String str;

		str.string = std::wstring(string) + L"=" + std::to_wstring(num);
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

	// フォントの高さを取得する関数
	float GetFontHeight() {	return m_fontHeight; }

	// 共通ステートの設定
	void SetCommonStates(DirectX::CommonStates* states);
};

class DebugFont3D : protected DebugFont
{
private:

	// 文字列情報
	struct String
	{
		// 位置
		DirectX::SimpleMath::Vector3 pos;

		// 文字列
		std::wstring string;

		// 色
		DirectX::SimpleMath::Color color;

		// スケール
		float scale = 1.0f;
	};

	// 表示文字列の配列
	std::vector<String> m_strings;

	// エフェクト
	std::unique_ptr<DirectX::BasicEffect> m_effect;

	// 入力レイアウト
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

public:

	// コンストラクタ
	DebugFont3D(
		ID3D11Device* device,
		ID3D11DeviceContext* context,
		wchar_t const* fileName);

	// デストラクタ
	~DebugFont3D();

	// 描画する文字列を登録する関数
	void AddString(
		const wchar_t* string,
		DirectX::SimpleMath::Vector3 pos,
		DirectX::FXMVECTOR color = DirectX::Colors::White,
		float scale = 1.0f);

	// 描画関数
	void Render(
		ID3D11DeviceContext* context,
		DirectX::CommonStates* states,
		const DirectX::SimpleMath::Matrix& view,
		const DirectX::SimpleMath::Matrix& proj);

	// フォントの高さを取得する関数
	float GetFontHeight() { return m_fontHeight; }
};

