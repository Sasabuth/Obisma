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

	// サッカーボールのモデルをロードする
	m_fieldModel = Model::CreateFromSDKMESH(device, L"Resources/Models/Planet.sdkmesh", *effectFactory);

	// プレーヤーモデルローダーフラグ
	DirectX::ModelLoaderFlags flags = DirectX::ModelLoader_Clockwise | DirectX::ModelLoader_IncludeBones;
	// SDKMESH形式のプレーヤーモデルをロードする
	m_playerModel = Model::CreateFromSDKMESH(device, L"Resources/Models/Player.sdkmesh", *effectFactory, flags);
	m_playerModel->UpdateEffects(
		[&](IEffect* pEffect)
		{
			// BasicEffectにキャストする
			DirectX::BasicEffect* pBasicEffect = dynamic_cast<DirectX::BasicEffect*>(pEffect);

			pBasicEffect->SetAmbientLightColor(SimpleMath::Vector4(1, 1, 1, 1));
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
}
