/// <summary>
/// Stoppingに関するソースファイル
/// </summary>
/// <author>仲森智史</author>
/// <date>2025/05/21</date>

// ヘッダファイルの読み込み
#include "pch.h"
#include "Stopping.h"

#include "Game/Scenes/GameplayScene.h"
#include "Game/GameObjects/Field/Field.h"
#include "DebugDraw.h"
#include "Game/Commons/Resources.h"
#include "Game/GameObjects/Ball/Ball.h"


// 名前の省略
using namespace DirectX;


/// <summary>
/// コンストラクタ
/// </summary>
Stopping::Stopping(Ball* ball)
	: m_ball(ball)
	, m_userResources(nullptr)
	, m_model{}
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

	m_model = Resources::GetInstance()->GetBallModel();
}



/// <summary>
/// 更新処理
/// </summary>
/// <param name="elapsedTime">経過時間</param> 
void Stopping::Update(float elapsedTime)
{
	UNREFERENCED_PARAMETER(elapsedTime);

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
	// デバックフォントの描画
	auto* debugFont = m_userResources->GetDebugFont();

	auto context = m_userResources->GetDeviceResources()->GetD3DDeviceContext();
	auto states = m_userResources->GetCommonStates();
	auto view = m_userResources->GetView();
	auto proj = m_userResources->GetProject();

	// ワールド座標
	SimpleMath::Matrix world;

	SimpleMath::Matrix pos = SimpleMath::Matrix::CreateTranslation(m_ball->GetPosition());
	SimpleMath::Matrix scale = SimpleMath::Matrix::CreateScale(SimpleMath::Vector3(Ball::BALL_SIZE));

	SimpleMath::Matrix rotate = SimpleMath::Matrix::CreateFromQuaternion(m_ball->GetRotation()); // ※回転順に合わせて調整

	world = scale * rotate * pos;

	// モデルの描画
	m_model->Draw(context, *states, world, *view, *proj);

	// 影の描画
	m_ball->DrawShadow(context, states, Ball::SHADOW_SIZE);

	// デバック
	m_ball->GetCollider().Draw(states, *view, *proj);
	debugFont->Render(L"Stopping");

}



/// <summary>
/// 終了処理
/// </summary>
void Stopping::Finalize()
{
}