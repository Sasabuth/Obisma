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


// 名前の省略
using namespace DirectX;


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

	auto mouse = Mouse::Get().GetState();

	m_rotate += 10.0f * elapsedTime;

	// プレイヤーの設定
	m_airTarget->SetVelocity(SimpleMath::Vector3::Zero);
	m_airTarget->GetCollider().SetPosition(m_airTarget->GetPosition());
}



/// <summary>
/// 描画処理
/// </summary>
void Floating::Render()
{
	// デバックフォントの描画
	auto* debugFont = m_userResources->GetDebugFont();

	auto context = m_userResources->GetDeviceResources()->GetD3DDeviceContext();
	auto states = m_userResources->GetCommonStates();
	auto view = m_userResources->GetView();
	auto proj = m_userResources->GetProject();

	// ワールド座標
	SimpleMath::Matrix world;

	SimpleMath::Matrix pos = SimpleMath::Matrix::CreateTranslation(m_airTarget->GetPosition());
	SimpleMath::Matrix scale = SimpleMath::Matrix::CreateScale(SimpleMath::Vector3(AirTarget::BALL_SIZE));

	SimpleMath::Matrix rotate = SimpleMath::Matrix::CreateRotationY(XMConvertToRadians(m_rotate)) * SimpleMath::Matrix::CreateFromQuaternion(m_airTarget->GetRotation()); // ※回転順に合わせて調整

	world = scale * rotate * pos;

	// モデルの描画
	m_airTarget->GetModel()->Draw(context, *states, world, *view, *proj);

	// 影の描画
	m_airTarget->DrawShadow(context, states, AirTarget::SHADOW_SIZE);

	// デバック
	//debugFont->Render(L"Floating");
	debugFont->Render(L"Position", m_airTarget->GetPosition().x * m_airTarget->GetPosition().y * m_airTarget->GetPosition().z);

}



/// <summary>
/// 終了処理
/// </summary>
void Floating::Finalize()
{
}