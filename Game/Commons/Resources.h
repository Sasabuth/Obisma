/// <summary>
/// Resourcesに関するヘッダファイル
/// </summary>
/// <author>仲森智史</author>

// 多重インクルードの防止
#pragma once

// ヘッダファイルの読み込み
#include "Model.h"
#include "Game/Commons/UserResources.h"

// Resourcesクラスを定義する
class Resources
{
public:
	// フィールドモデルを取得
	DirectX::Model* GetFieldModel() { return m_fieldModel.get(); }
	// プレイヤーモデルの取得
	DirectX::Model* GetPlayerModel() { return m_playerModel.get(); }
	// 敵モデルの取得
	DirectX::Model* GetEnemyModel() { return m_enemyModel.get(); }
	// 影のテクスチャの取得
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetShadowTexture() { return m_shadowTexture.Get(); }
	// タイトルのテクスチャの取得
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetTitleTexture() { return m_titleTexture.Get(); }
	// プレイヤーフェイスのテクスチャの取得
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetPlayerFace() { return m_playerFaceTexture.Get(); }
	// スコアフォントのテクスチャの取得
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetScoreFont() { return m_scoreFontTexture.Get(); }
	// プレイヤーフレームのテクスチャの取得
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetPlayerFrame() { return m_playerFrameTexture.Get(); }

public:
	Resources(Resources&&) = default;
	Resources& operator= (Resources&&) = default;
	Resources& operator= (Resources const&) = delete;
	// デストラクタ
	~Resources() = default;
	// Resoucesクラスのインスタンスを取得する
	static Resources* const GetInstance();
	// リソースをロードする
	void LoadResource();

	void Reset();

private:
	// コンストラクタ
	Resources() noexcept
		:
		m_fieldModel{},
		m_playerModel{}
	{
		m_userResource = UserResources::GetUserResource();
	}

private:
	// リソース
	static std::unique_ptr<Resources> m_resources;
	// グラフィックス
	UserResources* m_userResource = UserResources::GetUserResource();

	// プレーヤーモデル
	std::unique_ptr<DirectX::Model> m_playerModel;
	// 敵モデル
	std::unique_ptr<DirectX::Model> m_enemyModel;
	// フィールドモデル
	std::unique_ptr<DirectX::Model> m_fieldModel;
	// 影のテクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shadowTexture;
	// タイトルのテクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_titleTexture;
	// プレイヤーフェイスのテクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_playerFaceTexture;
	// スコアフォントのテクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_scoreFontTexture;
	// プレイヤーフレームのテクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_playerFrameTexture;

};
