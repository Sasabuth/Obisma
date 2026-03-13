/// <summary>
/// Resourcesに関するソースファイル
/// </summary>
/// <author>仲森智史</author>

// 多重インクルードの防止
#pragma once

// ヘッダファイルの読み込み
#include "pch.h"
#include "Resources.h"

#include <fstream>
#include <iostream>



// シングルトンの初期化
std::unique_ptr<Resources> Resources::m_resources = nullptr;



/// <summary>
/// インスタンスの取得
/// </summary>
/// <returns>リソースのポインタ</returns>
Resources* const Resources::GetInstance()
{
	if (m_resources == nullptr)
	{
		// Resourcesクラスのインスタンスを生成する
		m_resources.reset(new Resources());
	}

	// Resourcesクラスのインスタンスを返す
	return m_resources.get();
}



/// <summary>
/// 音の取得
/// </summary>
/// <param name="filename">ファイル名</param>
/// <returns>音インスタンス</returns>
std::unique_ptr<DirectX::SoundEffectInstance> Resources::GetBGMSound(const std::wstring& filename, DirectX::SimpleMath::Vector3 emitterPos, bool loop)
{
	// 未登録の場合
	if (m_sounds.count(filename) == 0)
	{
		// 音ファイルの読み込み
		std::wstring fullPath = DEFAULT_SOUND_DIRECTORY + std::wstring(filename);

		std::unique_ptr<DirectX::SoundEffect> sound = std::make_unique<DirectX::SoundEffect>(m_audEngine.get(), fullPath.c_str());

		// 音データのハンドルを登録
		m_sounds.emplace(filename, std::move(sound));
	}

	// インスタンスの返却
	std::unique_ptr<DirectX::SoundEffectInstance> sound = m_sounds[filename]->CreateInstance(DirectX::SoundEffectInstance_Use3D);

	// 音量の設定
	sound->SetVolume(m_bgmVolume);

	DirectX::AudioEmitter emitter;
	emitter.SetPosition(emitterPos);
	emitter.CurveDistanceScaler = 10.0f;
	emitter.DopplerScaler = 1.0f;
	sound->Play(loop);
	return sound;
}



/// <summary>
/// 音の取得
/// </summary>
/// <param name="filename">ファイル名</param>
/// <returns>音インスタンス</returns>
std::unique_ptr<DirectX::SoundEffectInstance> Resources::GetSESound(const std::wstring& filename, DirectX::SimpleMath::Vector3 emitterPos, bool loop)
{
	// 未登録の場合
	if (m_sounds.count(filename) == 0)
	{
		// 音ファイルの読み込み
		std::wstring fullPath = DEFAULT_SOUND_DIRECTORY + std::wstring(filename);

		std::unique_ptr<DirectX::SoundEffect> sound = std::make_unique<DirectX::SoundEffect>(m_audEngine.get(), fullPath.c_str());

		// 音データのハンドルを登録
		m_sounds.emplace(filename, std::move(sound));
	}

	// インスタンスの返却
	std::unique_ptr<DirectX::SoundEffectInstance> sound = m_sounds[filename]->CreateInstance(DirectX::SoundEffectInstance_Use3D);

	// 音量の設定
	sound->SetVolume(m_seVolume);

	DirectX::AudioEmitter emitter;
	emitter.SetPosition(emitterPos);
	emitter.CurveDistanceScaler = 10.0f;
	emitter.DopplerScaler = 1.0f;
	sound->Apply3D(m_listener, emitter);
	sound->Play(loop);
	return sound;
}



/// <summary>
/// モデルの取得
/// </summary>
/// <param name="filename">ファイル名</param>
/// <returns>モデルのポインタ</returns>
DirectX::Model* Resources::GetModel(const std::wstring& filename)
{
	// モデルの設定
	auto device = m_userResource->GetDeviceResources()->GetD3DDevice();

	// プレーヤーモデルローダーフラグ
	DirectX::ModelLoaderFlags flags = DirectX::ModelLoader_Clockwise | DirectX::ModelLoader_IncludeBones;

	auto effectFactory = m_userResource->GetEffectFactory();
	effectFactory->SetDirectory(L"Resources/Models");

	// 未登録の場合
	if (m_models.count(filename) == 0)
	{
		// モデルファイルの読み込み
		std::wstring fullPath = DEFAULT_MODEL_DIRECTORY + std::wstring(filename);

		std::unique_ptr<DirectX::Model> model = DirectX::Model::CreateFromSDKMESH(device, fullPath.c_str(), *effectFactory, flags);

		// モデルデータのハンドルを登録
		m_models.emplace(filename, std::move(model));
	}

	return m_models[filename].get();
}



/// <summary>
/// テクスチャの取得
/// </summary>
/// <param name="filename">ファイル名</param>
/// <returns>テクスチャ</returns>
Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Resources::GetTexture(const std::wstring& filename)
{
	// モデルの設定
	auto device = m_userResource->GetDeviceResources()->GetD3DDevice();

	// 未登録の場合
	if (m_textures.count(filename) == 0)
	{
		// テクスチャファイルの読み込み
		std::wstring fullPath = DEFAULT_TEXTURE_DIRECTORY + std::wstring(filename);
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;
		DX::ThrowIfFailed(DirectX::CreateWICTextureFromFile(device, fullPath.c_str(), nullptr, texture.ReleaseAndGetAddressOf()));

		// テクスチャデータのハンドルを登録
		m_textures.emplace(filename, std::move(texture));
	}

	return m_textures[filename];
}



/// <summary>
/// DDSテクスチャの取得
/// </summary>
/// <param name="filename">ファイル名</param>
/// <returns>DDSテクスチャ</returns>
Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Resources::GetDDSTexture(const std::wstring& filename)
{
	// モデルの設定
	auto device = m_userResource->GetDeviceResources()->GetD3DDevice();

	// 未登録の場合
	if (m_textures.count(filename) == 0)
	{
		// テクスチャファイルの読み込み
		std::wstring fullPath = DEFAULT_TEXTURE_DIRECTORY + std::wstring(filename);
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;
		DX::ThrowIfFailed(DirectX::CreateDDSTextureFromFile(device, fullPath.c_str(), nullptr, texture.ReleaseAndGetAddressOf()));

		// テクスチャデータのハンドルを登録
		m_textures.emplace(filename, std::move(texture));
	}

	return m_textures[filename];
}



/// <summary>
/// Jsonの設定
/// </summary>
/// <param name="filename">ファイル名</param>
/// <param name="json">Json</param>
void Resources::SetJson(const std::wstring& filename, nlohmann::json json)
{
	// Jsonファイルの読み込み
	std::wstring fullPath = DEFAULT_JSON_DIRECTORY + std::wstring(filename);
	std::ofstream file(fullPath);

	// Jsonを保存
	file << std::setw(4) << json << std::endl << std::endl;
	file.close();

	// 変更内容を保存
	m_jsons[filename] = json;
}



/// <summary>
/// Jsonの取得
/// </summary>
/// <param name="filename">ファイル名</param>
/// <returns>Json</returns>
nlohmann::json Resources::GetJson(const std::wstring& filename)
{
	// 未登録の場合
	if (m_jsons.count(filename) == 0)
	{
		// Jsonファイルの読み込み
		std::wstring fullPath = DEFAULT_JSON_DIRECTORY + std::wstring(filename);
		std::ifstream file(fullPath);
		nlohmann::json json;
		file >> json;

		// Jsonのハンドルを登録
		m_jsons.emplace(filename, std::move(json));
	}

	return m_jsons[filename];
}



/// <summary>
/// リスナーの設定
/// </summary>
/// <param name="pos">座標</param>
/// <param name="forward">前向きベクトル</param>
/// <param name="up">上向きベクトル</param>
void Resources::SetListener(const DirectX::SimpleMath::Vector3& pos, const DirectX::SimpleMath::Vector3& forward, const DirectX::SimpleMath::Vector3& up)
{
	// リスナーの座標
	m_listener.SetPosition(pos);
	// リスナーの向き
	m_listener.SetOrientation(forward, up);
}



/// <summary>
/// 3Dサウンドの設定
/// </summary>
/// <param name="sound">サウンド</param>
/// <param name="pos">座標</param>
void Resources::Set3DSound(DirectX::SoundEffectInstance* sound, const DirectX::SimpleMath::Vector3& pos)
{
	if (sound && sound->GetState() == DirectX::SoundState::PLAYING)
	{
		DirectX::AudioEmitter emitter;
		emitter.SetPosition(pos); // 常に最新の位置を設定

		// 減衰距離の設定（GetSoundの瞬間と同じ値を設定）
		emitter.CurveDistanceScaler = 6.0f; // 例
		emitter.DopplerScaler = 2.0f;

		// リスナーの位置を取得
		DirectX::AudioListener listener = Resources::GetInstance()->GetListener();

		// 3D効果を適用：この呼び出しが距離減衰を毎フレーム更新する
		sound->Apply3D(listener, emitter);
	}
}



/// <summary>
/// リソースのリセット
/// </summary>
void Resources::Reset()
{
	// 音データの削除
	m_sounds.clear();

	// モデルの削除
	m_models.clear();

	// テクスチャの削除
	m_textures.clear();

	// Jsonの削除
	m_jsons.clear();
}
