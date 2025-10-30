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
#include <json.hpp>


// Resourcesクラスを定義する
class Resources
{
// 定数
private:
	static constexpr const wchar_t* DEFAULT_MODEL_DIRECTORY = L"Resources/Models/";
	static constexpr const wchar_t* DEFAULT_TEXTURE_DIRECTORY = L"Resources/Textures/";
	static constexpr const wchar_t* DEFAULT_SOUND_DIRECTORY = L"Resources/Sounds/";
	static constexpr const wchar_t* DEFAULT_JSON_DIRECTORY = L"Resources/Jsons/";


// エイリアス宣言
private:
	using ResourceSound = std::unordered_map<std::wstring, std::unique_ptr<DirectX::SoundEffect>>;
	using ResourceModel = std::unordered_map<std::wstring, std::unique_ptr<DirectX::Model>>;
	using ResourceTexture = std::unordered_map<std::wstring, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>;
	using ResourceJson = std::unordered_map<std::wstring, nlohmann::json>;


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
	std::unique_ptr<DirectX::SoundEffectInstance> GetBGMSound(const std::wstring& filename, DirectX::SimpleMath::Vector3 emitterPos, bool loop);
	std::unique_ptr<DirectX::SoundEffectInstance> GetSESound(const std::wstring& filename, DirectX::SimpleMath::Vector3 emitterPos, bool loop);

	// モデルデータの取得
	DirectX::Model* GetModel(const std::wstring& filename);

	// 画像データの取得
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetTexture(const std::wstring& filename);

	// Json
	void SetJson(const std::wstring& filename, nlohmann::json json);  // 設定
	nlohmann::json GetJson(const std::wstring& filename);             // 取得 

	// 音量の設定
	void SetBGMVolume(float volume) { m_bgmVolume = volume; }
	float GetBGMVolume() const { return m_bgmVolume; }
	void SetSEVolume(float volume) { m_seVolume = volume; }
	float GetSEVolume() const { return m_seVolume; }

	// リスナーの設定
	void SetListener(const DirectX::SimpleMath::Vector3& pos, const DirectX::SimpleMath::Vector3& forward, const DirectX::SimpleMath::Vector3& up);
	DirectX::AudioListener GetListener() const { return m_listener; }

	// 3Dサウンドの設定
	void Set3DSound(DirectX::SoundEffectInstance* sound, const DirectX::SimpleMath::Vector3& pos);

	// リセット
	void Reset();
	void JsonReset() { m_jsons.clear(); }


private:
	// コンストラクタ
	Resources() noexcept
		:
		m_fieldModel{},
		m_playerModel{},
		m_bgmVolume{},
		m_seVolume{}
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

	// Jsonデータ群
	ResourceJson m_jsons;

	// 音量
	float m_bgmVolume;
	float m_seVolume;;

	// リスナー
	DirectX::AudioListener m_listener;

};
