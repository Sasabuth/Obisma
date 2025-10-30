/// <summary>
/// Floatingに関するソースファイル
/// </summary>
/// <author>仲森智史</author>
/// <date>2025/05/21</date>

// ヘッダファイルの読み込み
#include "pch.h"
#include "Floating.h"

#include "Game/Scenes/GameplayScene.h"
#include "Game/GameObjects/Field/Field.h"
#include "DebugDraw.h"
#include "Game/Commons/Resources.h"
#include "Game/GameObjects/AirTarget/AirTarget.h"



/// <summary>
/// コンストラクタ
/// </summary>
Floating::Floating(AirTarget* airTarget)
	: m_airTarget(airTarget)
	, m_userResources(nullptr)
	, m_rotate(0)
{
}



/// <summary>
/// デストラクタ
/// </summary>
Floating::~Floating()
{
}


/// <summary>
/// 初期化処理
/// </summary>
void Floating::Initialize()
{
	// ユーザーリソースの取得
	m_userResources = UserResources::GetUserResource();

	m_rotate = 0.0f;
}



/// <summary>
/// 更新処理
/// </summary>
/// <param name="elapsedTime">経過時間</param> 
void Floating::Update(float elapsedTime)
{
	UNREFERENCED_PARAMETER(elapsedTime);

	auto mouse = DirectX::Mouse::Get().GetState();

	m_rotate += Resources::GetInstance()->GetJson(L"AirTarget.json")["RotateSpeed"] * elapsedTime;

	// プレイヤーの設定
	m_airTarget->SetVelocity(DirectX::SimpleMath::Vector3::Zero);
	m_airTarget->GetCollider().SetPosition(m_airTarget->GetPosition());
}



/// <summary>
/// 描画処理
/// </summary>
void Floating::Render()
{
	auto context = m_userResources->GetDeviceResources()->GetD3DDeviceContext();
	auto states = m_userResources->GetCommonStates();
	auto view = m_userResources->GetView();
	auto proj = m_userResources->GetProject();

	// ワールド座標
	DirectX::SimpleMath::Matrix world;

	DirectX::SimpleMath::Matrix pos = DirectX::SimpleMath::Matrix::CreateTranslation(m_airTarget->GetPosition());
	DirectX::SimpleMath::Matrix scale = DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(Resources::GetInstance()->GetJson(L"AirTarget.json")["ColliderSize"]));

	DirectX::SimpleMath::Matrix rotate =
		DirectX::SimpleMath::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_rotate)) *
		DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_airTarget->GetRotation());

	world = scale * rotate * pos;

	// モデルの描画
	m_airTarget->GetModel()->Draw(context, *states, world, *view, *proj);

	// 影の描画
	m_airTarget->DrawShadow(context, states, Resources::GetInstance()->GetJson(L"AirTarget.json")["ShadowSize"]);

	// デバック
	auto* debugFont = m_userResources->GetDebugFont();

	debugFont->Render(L"Floating");
	debugFont->Render(L"Position", m_airTarget->GetPosition().x * m_airTarget->GetPosition().y * m_airTarget->GetPosition().z);

}



/// <summary>
/// 終了処理
/// </summary>
void Floating::Finalize()
{
}