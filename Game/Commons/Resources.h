#pragma once
#ifndef RESOURCES_DEFINED
#define RESOURCES_DEFINED
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
	// ボールモデルの取得
	DirectX::Model* GetBallModel() { return m_ballModel.get(); }
	// ボールモデルの取得
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetShadowTexture() { return m_shadowTexture.Get(); }

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
	// ボールモデル
	std::unique_ptr<DirectX::Model> m_ballModel;
	// 影のテクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shadowTexture;

};

#endif		// RESOURCES_DEFINED
