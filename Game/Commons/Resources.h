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
	// ボールモデルの取得
	DirectX::Model* GetBallModel() { return m_ballModel.get(); }

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
	// フィールドモデル
	std::unique_ptr<DirectX::Model> m_fieldModel;
	// ボールモデル
	std::unique_ptr<DirectX::Model> m_ballModel;

};

#endif		// RESOURCES_DEFINED
