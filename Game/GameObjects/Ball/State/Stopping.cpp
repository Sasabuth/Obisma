/// <summary>
/// Stoppingに関するソースファイル
/// </summary>
/// <author>仲森智史</author>
/// <date>2025/05/21</date>

// ヘッダファイルの読み込み
#include "pch.h"
#include "Stopping.h"

#include "DebugDraw.h"
#include "Game/Commons/Resources.h"
#include "Game/GameObjects/Ball/Ball.h"



/// <summary>
/// コンストラクタ
/// </summary>
Stopping::Stopping(Ball* ball)
	: m_ball(ball)
	, m_userResources(nullptr)
{
}



/// <summary>
/// デストラクタ
/// </summary>
Stopping::~Stopping()
{
}


/// <summary>
/// 初期化処理
/// </summary>
void Stopping::Initialize()
{
	m_userResources = UserResources::GetUserResource();
}



/// <summary>
/// 更新処理
/// </summary>
/// <param name="elapsedTime">経過時間</param> 
void Stopping::Update(float elapsedTime)
{
	UNREFERENCED_PARAMETER(elapsedTime);

	// 白色にする
	m_ball->SetBallColorNum(Ball::BallColor::NOMAL);

	// プレイヤーの設定
	m_ball->SetVelocity(m_ball->GetGravity());
	m_ball->SetPosition(m_ball->GetPosition() + m_ball->GetVelocity() * elapsedTime);
	m_ball->GetCollider().SetPosition(m_ball->GetPosition());
}



/// <summary>
/// 描画処理
/// </summary>
void Stopping::Render()
{
	auto context = m_userResources->GetDeviceResources()->GetD3DDeviceContext();
	auto states = m_userResources->GetCommonStates();
	auto view = m_userResources->GetView();
	auto proj = m_userResources->GetProject();

	// ワールド座標
	DirectX::SimpleMath::Matrix world;

	DirectX::SimpleMath::Matrix pos = DirectX::SimpleMath::Matrix::CreateTranslation(m_ball->GetPosition());
	DirectX::SimpleMath::Matrix scale = DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(Resources::GetInstance()->GetJson(L"Ball.json")["BallSize"]));

	DirectX::SimpleMath::Matrix rotate = DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_ball->GetRotation()); // ※回転順に合わせて調整

	world = scale * rotate * pos;

	// モデルの描画
	m_ball->GetModel()->Draw(context, *states, world, *view, *proj);

	// 影の描画
	m_ball->DrawShadow(context, states, Resources::GetInstance()->GetJson(L"Ball.json")["ShadowSize"]);

	// デバック用
	/*auto* debugFont = m_userResources->GetDebugFont();*/

	/*debugFont->Render(L"Stopping");*/

}



/// <summary>
/// 終了処理
/// </summary>
void Stopping::Finalize()
{
}

void Stopping::EventHandle(Event e)
{
	UNREFERENCED_PARAMETER(e);
}
