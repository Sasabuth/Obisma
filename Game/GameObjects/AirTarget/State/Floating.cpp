/// <summary>
/// Floatingに関するソースファイル
/// </summary>
/// <author>仲森智史</author>
/// <date>2025/05/21</date>

// ヘッダファイルの読み込み
#include "pch.h"
#include "Floating.h"

#include "Game/GameObjects/AirTarget/AirTarget.h"
#include "DebugDraw.h"
#include "Game/Commons/Resources.h"




/// <summary>
/// コンストラクタ
/// </summary>
Floating::Floating(AirTarget* pAirTarget)
	: m_pAirTarget(pAirTarget)
	, m_pUserResources(nullptr)
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
	m_pUserResources = UserResources::GetUserResource();

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
	m_pAirTarget->SetVelocity(DirectX::SimpleMath::Vector3::Zero);
	m_pAirTarget->GetCollider().SetPosition(m_pAirTarget->GetPosition());
}



/// <summary>
/// 描画処理
/// </summary>
void Floating::Render()
{
	auto context = m_pUserResources->GetDeviceResources()->GetD3DDeviceContext();
	auto states = m_pUserResources->GetCommonStates();
	auto view = m_pUserResources->GetView();
	auto proj = m_pUserResources->GetProject();

	// ワールド座標
	DirectX::SimpleMath::Matrix world;

	DirectX::SimpleMath::Matrix pos = DirectX::SimpleMath::Matrix::CreateTranslation(m_pAirTarget->GetPosition());
	DirectX::SimpleMath::Matrix scale = DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(Resources::GetInstance()->GetJson(L"AirTarget.json")["ColliderSize"]));

	DirectX::SimpleMath::Matrix rotate =
		DirectX::SimpleMath::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_rotate)) *
		DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_pAirTarget->GetRotation());

	world = scale * rotate * pos;

	// モデルの描画
	m_pAirTarget->GetModel()->Draw(context, *states, world, *view, *proj);

	// 影の描画
	m_pAirTarget->DrawShadow(context, states, Resources::GetInstance()->GetJson(L"AirTarget.json")["ShadowSize"]);

	// デバック
	/*auto* debugFont = m_pUserResources->GetDebugFont();

	debugFont->Render(L"Floating");
	debugFont->Render(L"Position", m_pAirTarget->GetPosition().x * m_pAirTarget->GetPosition().y * m_pAirTarget->GetPosition().z);*/

}



/// <summary>
/// 終了処理
/// </summary>
void Floating::Finalize()
{
}

void Floating::EventHandle(Event e)
{
	UNREFERENCED_PARAMETER(e);
}
