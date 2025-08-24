#include "pch.h"
#include "Resources.h"

using namespace DirectX;

std::unique_ptr<Resources> Resources::m_resources = nullptr;

// Resourcesクラスのインスタンスを取得する
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

// リソースをロードする
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

	// ボールのモデルをロードする
	m_ballModel = Model::CreateFromSDKMESH(device, L"Resources/Models/Ball.sdkmesh", *effectFactory);
	m_ballModel->UpdateEffects(
		[&](IEffect* pEffect)
		{
			// BasicEffectにキャストする
			DirectX::BasicEffect* pBasicEffect = dynamic_cast<DirectX::BasicEffect*>(pEffect);

			pBasicEffect->SetAmbientLightColor(SimpleMath::Vector4(1, 1, 1, 1));
		}
	);

	// テクスチャの読み込み
	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(device, L"Resources/Textures/Shadow.dds", nullptr, m_shadowTexture.ReleaseAndGetAddressOf())
	);
}

void Resources::Reset()
{
	m_fieldModel.reset();
	m_ballModel.reset();
	m_playerModel.reset();
	m_shadowTexture.Reset();
}
