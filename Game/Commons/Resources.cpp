/// <summary>
/// Resourcesに関するヘッダファイル
/// </summary>
/// <author>仲森智史</author>

// 多重インクルードの防止
#pragma once

// ヘッダファイルの読み込み
#include "pch.h"
#include "Resources.h"


// 名前の省略
using namespace DirectX;

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
/// リソースのロード
/// </summary>
void Resources::LoadResource()
{
	// モデルの設定
	auto device = m_userResource->GetDeviceResources()->GetD3DDevice();

	auto effectFactory = m_userResource->GetEffectFactory();
	effectFactory->SetDirectory(L"Resources/Models");

	// フィールドのモデルをロードする
	m_fieldModel = Model::CreateFromSDKMESH(device, L"Resources/Models/Planet.sdkmesh", *effectFactory);

	// プレーヤーモデルローダーフラグ
	DirectX::ModelLoaderFlags flags = DirectX::ModelLoader_Clockwise | DirectX::ModelLoader_IncludeBones;
	// プレーヤーモデルをロードする
	m_playerModel = Model::CreateFromSDKMESH(device, L"Resources/Models/Player.sdkmesh", *effectFactory, flags);
	m_playerModel->UpdateEffects(
		[&](IEffect* pEffect)
		{
			// BasicEffectにキャストする
			auto pBasicEffect = dynamic_cast<DirectX::SkinnedEffect*> (pEffect);

			pBasicEffect->SetAmbientLightColor(SimpleMath::Vector4(1, 1, 1, 0.5));
		}
	);

	// 敵モデルをロードする
	m_enemyModel = Model::CreateFromSDKMESH(device, L"Resources/Models/Enemy.sdkmesh", *effectFactory, flags);
	m_enemyModel->UpdateEffects(
		[&](IEffect* pEffect)
		{
			// BasicEffectにキャストする
			auto pBasicEffect = dynamic_cast<DirectX::SkinnedEffect*> (pEffect);

			pBasicEffect->SetAmbientLightColor(SimpleMath::Vector4(1, 1, 1, 0.5));
		}
	);

	// 星モデルをロードする
	m_sterModel = Model::CreateFromSDKMESH(device, L"Resources/Models/Ster.sdkmesh", *effectFactory);
	m_sterModel->UpdateEffects(
		[&](IEffect* pEffect)
		{
			// BasicEffectにキャストする
			DirectX::BasicEffect* pBasicEffect = dynamic_cast<DirectX::BasicEffect*>(pEffect);

			pBasicEffect->SetAmbientLightColor(SimpleMath::Vector4(1, 1, 1, 1));
		}
	);


	// スカイドームをロードする
	m_skydome = Model::CreateFromSDKMESH(device, L"Resources/Models/skydome.sdkmesh", *effectFactory);
	m_skydome->UpdateEffects(
		[&](IEffect* pEffect)
		{
			// BasicEffectにキャストする
			DirectX::BasicEffect* pBasicEffect = dynamic_cast<DirectX::BasicEffect*>(pEffect);


			pBasicEffect->SetAmbientLightColor(Colors::WhiteSmoke);
		}
	);
}



/// <summary>
/// 音の取得
/// </summary>
/// <param name="filename">ファイル名</param>
/// <returns>音インスタンス</returns>
std::unique_ptr<DirectX::SoundEffectInstance> Resources::GetSound(const std::wstring& filename)
{
	// 未登録の場合
	if (m_sounds.count(filename) == 0)
	{
		// 音ファイルの読み込み
		std::wstring fullPath = DEFAULT_SOUND_DIRECTORY + std::wstring(filename);

		std::unique_ptr<SoundEffect> sound = std::make_unique<SoundEffect>(m_audEngine.get(), fullPath.c_str());

		// 音データのハンドルを登録
		m_sounds.emplace(filename, std::move(sound));
	}

	// インスタンスの返却
	std::unique_ptr<SoundEffectInstance> sound = m_sounds[filename]->CreateInstance();

	// 音量の設定
	sound->SetVolume(m_volume);

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

	auto effectFactory = m_userResource->GetEffectFactory();
	effectFactory->SetDirectory(L"Resources/Models");

	// 未登録の場合
	if (m_models.count(filename) == 0)
	{
		// モデルファイルの読み込み
		std::wstring fullPath = DEFAULT_MODEL_DIRECTORY + std::wstring(filename);

		std::unique_ptr<Model> model = Model::CreateFromSDKMESH(device, fullPath.c_str(), *effectFactory);

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
		if (FAILED(CreateWICTextureFromFile(device, fullPath.c_str(), nullptr, texture.ReleaseAndGetAddressOf())))
		{
			MessageBox(NULL, fullPath.c_str(), L"エラー", MB_OK);
		}

		// テクスチャデータのハンドルを登録
		m_textures.emplace(filename, std::move(texture));
	}

	return m_textures[filename];
}



/// <summary>
/// リソースのリセット
/// </summary>
void Resources::Reset()
{
	m_fieldModel.reset();
	m_playerModel.reset();
	m_enemyModel.reset();
	m_sterModel.reset();
	m_skydome.reset();

	// 音データの削除
	m_sounds.clear();

	// モデルの削除
	m_models.clear();

	// テクスチャの削除
	m_textures.clear();
}
