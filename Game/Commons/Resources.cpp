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

	// テクスチャの読み込み
	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(device, L"Resources/Textures/Shadow.dds", nullptr, m_shadowTexture.ReleaseAndGetAddressOf())
	);

	// テクスチャがあるか
	if (FAILED(DirectX::CreateWICTextureFromFile(device, L"Resources/Textures/Space.jpg", nullptr, m_spaceTexture.ReleaseAndGetAddressOf())))
	{
		MessageBox(NULL, L"Resources/Textures/Space.jpg", L"エラー", MB_OK);
	}

	// テクスチャがあるか
	if (FAILED(DirectX::CreateWICTextureFromFile(device, L"Resources/Textures/LockOn.png", nullptr, m_lockOnTexture.ReleaseAndGetAddressOf())))
	{
		MessageBox(NULL, L"Resources/Textures/LockOn.png", L"エラー", MB_OK);
	}

	// テクスチャがあるか
	if (FAILED(DirectX::CreateWICTextureFromFile(device, L"Resources/Textures/Title.png", nullptr, m_titleTexture.ReleaseAndGetAddressOf())))
	{
		MessageBox(NULL, L"Resources/Textures/Title.png", L"エラー", MB_OK);
	}

	// フォントテクスチャの読み込み
	m_fontTextures.resize(2);
	for (size_t i = 0; i < m_fontTextures.size(); i++)
	{
		std::wstring filename = L"Resources/Textures/ScoreFont" + std::to_wstring(i) + L".png";
		if (FAILED(CreateWICTextureFromFile(device, filename.c_str(), nullptr, m_fontTextures[i].ReleaseAndGetAddressOf())))
		{
			MessageBox(NULL, filename.c_str(), L"エラー", MB_OK);
		}
	}

	// フレームテクスチャの読み込み
	m_frameTextures.resize(2);
	for (size_t i = 0; i < m_frameTextures.size(); i++)
	{
		std::wstring filename = L"Resources/Textures/ScoreFrame" + std::to_wstring(i) + L".png";
		if (FAILED(CreateWICTextureFromFile(device, filename.c_str(), nullptr, m_frameTextures[i].ReleaseAndGetAddressOf())))
		{
			MessageBox(NULL, filename.c_str(), L"エラー", MB_OK);
		}
	}
	// 顔テクスチャの読み込み
	m_faceTextures.resize(2);
	for (size_t i = 0; i < m_faceTextures.size(); i++)
	{
		std::wstring filename = L"Resources/Textures/Face" + std::to_wstring(i) + L".png";
		if (FAILED(CreateWICTextureFromFile(device, filename.c_str(), nullptr, m_faceTextures[i].ReleaseAndGetAddressOf())))
		{
			MessageBox(NULL, filename.c_str(), L"エラー", MB_OK);
		}
	}
	// 勝利テクスチャの読み込み
	m_winTextures.resize(2);
	for (size_t i = 0; i < m_winTextures.size(); i++)
	{
		std::wstring filename = L"Resources/Textures/Win" + std::to_wstring(i) + L".png";
		if (FAILED(CreateWICTextureFromFile(device, filename.c_str(), nullptr, m_winTextures[i].ReleaseAndGetAddressOf())))
		{
			MessageBox(NULL, filename.c_str(), L"エラー", MB_OK);
		}
	}
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
	m_shadowTexture.Reset();
	m_lockOnTexture.Reset();
	m_titleTexture.Reset();
	m_spaceTexture.Reset();

	for (size_t i = 0; i < m_fontTextures.size(); i++)
	{
		m_fontTextures[i].Reset();
	}
	for (size_t i = 0; i < m_frameTextures.size(); i++)
	{
		m_frameTextures[i].Reset();
	}
	for (size_t i = 0; i < m_faceTextures.size(); i++)
	{
		m_faceTextures[i].Reset();
	}
}
