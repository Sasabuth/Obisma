/// <summary>
/// Movingに関するソースファイル
/// </summary>
/// <author>仲森智史</author>
/// <date>2025/05/21</date>

// ヘッダファイルの読み込み
#include "pch.h"
#include "Moving.h"

#include "Game/Scenes/GameplayScene.h"
#include "Game/GameObjects//Camera/Camera.h"
#include "Game/GameObjects/Field/Field.h"
#include "DebugDraw.h"
#include "Game/Commons/Resources.h"
#include "Game/GameObjects/Ball/Ball.h"


// 名前の省略
using namespace DirectX;


/// <summary>
/// コンストラクタ
/// </summary>
Moving::Moving(Ball* ball)
	: m_ball(ball)
	, m_userResources(nullptr)
	, m_model{}
{
}



/// <summary>
/// デストラクタ
/// </summary>
Moving::~Moving()
{
}


/// <summary>
/// 初期化処理
/// </summary>
void Moving::Initialize()
{
	m_userResources = UserResources::GetUserResource();

	auto device = m_userResources->GetDeviceResources()->GetD3DDevice();
	auto context = m_userResources->GetDeviceResources()->GetD3DDeviceContext();

	m_model = Resources::GetInstance()->GetBallModel();
}



/// <summary>
/// 更新処理
/// </summary>
/// <param name="elapsedTime">経過時間</param> 
void Moving::Update(float elapsedTime)
{
	auto kb = Keyboard::Get().GetState();
	auto mouse = Mouse::Get().GetState();

	// プロジェクション行列
	auto proj = m_userResources->GetProject();
	auto view = m_userResources->GetView();

	m_ball->SetVelocity(m_ball->GetVelocity() + m_ball->GetGravity() / 2);

	/*if (kb.W)
	{
		m_ball->SetVelocity(m_ball->GetVelocity() - SimpleMath::Vector3::Transform(-SimpleMath::Vector3::UnitX, m_ball->GetRotation()));
		m_ball->SetPosition(m_ball->GetPosition() + m_ball->GetVelocity() * elapsedTime);
	}
	else if (kb.S)
	{
		m_ball->SetVelocity(m_ball->GetVelocity() + SimpleMath::Vector3::Transform(-SimpleMath::Vector3::UnitX, m_ball->GetRotation()));
		m_ball->SetPosition(m_ball->GetPosition() + m_ball->GetVelocity() * elapsedTime);
	}
	else
	{
		m_ball->ChangeState(m_ball->GetStopping());
	}*/

	m_ball->SetPosition(m_ball->GetPosition() + m_ball->GetVelocity() * elapsedTime);
	m_ball->GetCollider().SetPosition(m_ball->GetPosition());
}



/// <summary>
/// 描画処理
/// </summary>
void Moving::Render()
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

	// デバック
	debugFont->Render(L"Moving");

}



/// <summary>
/// 終了処理
/// </summary>
void Moving::Finalize()
{
}