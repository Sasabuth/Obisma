/// <summary>
/// Resourcesに関するヘッダファイル
/// </summary>
/// <author>仲森智史</author>

// 多重インクルードの防止
#pragma once

// ヘッダファイルの読み込み
#include "Model.h"
#include "Game/Commons/UserResources.h"
#include <unordered_map>


// Resourcesクラスを定義する
class Resources
{
// 定数
private:
	static constexpr const wchar_t* DEFAULT_MODEL_DIRECTORY = L"Resources/Models/";
	static constexpr const wchar_t* DEFAULT_TEXTURE_DIRECTORY = L"Resources/Textures/";
	static constexpr const wchar_t* DEFAULT_SOUND_DIRECTORY = L"Resources/Sounds/";


// エイリアス宣言
private:
	using ResourceSound = std::unordered_map<std::wstring, std::unique_ptr<DirectX::SoundEffect>>;
	using ResourceModel = std::unordered_map<std::wstring, std::unique_ptr<DirectX::Model>>;
	using ResourceTexture = std::unordered_map<std::wstring, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>;


public:
	// フィールドモデルを取得
	DirectX::Model* GetFieldModel() { return m_fieldModel.get(); }
	// プレイヤーモデルの取得
	DirectX::Model* GetPlayerModel() { return m_playerModel.get(); }
	// 敵モデルの取得
	DirectX::Model* GetEnemyModel() { return m_enemyModel.get(); }
	// 星モデルの取得
	DirectX::Model* GetSterModel() { return m_sterModel.get(); }
	// スカイドームの取得
	DirectX::Model* GetSkydome() { return m_skydome.get(); }

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

	// 音データの取得
	std::unique_ptr<DirectX::SoundEffectInstance> GetSound(const std::wstring& filename, DirectX::SimpleMath::Vector3 emitterPos, bool loop);

	// モデルデータの取得
	DirectX::Model* GetModel(const std::wstring& filename);

	// 画像データの取得
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetTexture(const std::wstring& filename);

	// 音量の設定
	void SetVolume(float volume) { m_volume = volume; }

	// リスナーの設定
	void SetListener(const DirectX::SimpleMath::Vector3& pos) { m_listener.SetPosition(pos); }

	// リセット
	void Reset();


private:
	// コンストラクタ
	Resources() noexcept
		:
		m_fieldModel{},
		m_playerModel{},
		m_volume{}
	{
		m_userResource = UserResources::GetUserResource();

		DirectX::AUDIO_ENGINE_FLAGS eflags = DirectX::AudioEngine_Default;
		m_audEngine = std::make_unique<DirectX::AudioEngine>(eflags);
	}

private:
	// リソース
	static std::unique_ptr<Resources> m_resources;

	// モデル
	// グラフィックス
	UserResources* m_userResource = UserResources::GetUserResource();

	// プレーヤーモデル
	std::unique_ptr<DirectX::Model> m_playerModel;
	// 敵モデル
	std::unique_ptr<DirectX::Model> m_enemyModel;
	// フィールドモデル
	std::unique_ptr<DirectX::Model> m_fieldModel;
	// 星モデル
	std::unique_ptr<DirectX::Model> m_sterModel;
	// スカイドーム
	std::unique_ptr<DirectX::Model> m_skydome;
	
	// 音エンジン
	std::unique_ptr<DirectX::AudioEngine>  m_audEngine;
	// 音データ群
	ResourceSound m_sounds;  

	// モデルデータ群
	ResourceModel m_models;      

	// 画像データ群
	ResourceTexture m_textures;  

	// 音量
	float m_volume;

	// リスナー
	DirectX::AudioListener m_listener;

};
