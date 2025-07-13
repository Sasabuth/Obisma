/// <summary>
/// Runningに関するソースファイル
/// </summary>
/// <author>仲森智史</author>
/// <date>2025/05/21</date>

// ヘッダファイルの読み込み
#include "pch.h"
#include "Running.h"

#include "Game/Scenes/GameplayScene.h"
#include "Game/GameObjects//Camera/Camera.h"
#include "Game/GameObjects/Field/Field.h"
#include "DebugDraw.h"
#include "Game/Commons/Resources.h"


// 名前の省略
using namespace DirectX;


/// <summary>
/// コンストラクタ
/// </summary>
Running::Running(Player* player)
	: m_player(player)
	, m_userResources(nullptr)
	, m_model{}
{
}



/// <summary>
/// デストラクタ
/// </summary>
Running::~Running()
{
}


/// <summary>
/// 初期化処理
/// </summary>
void Running::Initialize()
{
	m_userResources = UserResources::GetUserResource();

	auto device = m_userResources->GetDeviceResources()->GetD3DDevice();
	auto context = m_userResources->GetDeviceResources()->GetD3DDeviceContext();

	m_model = Resources::GetInstance()->GetPlayerModel();
}



/// <summary>
/// 更新処理
/// </summary>
/// <param name="elapsedTime">経過時間</param> 
void Running::Update(float elapsedTime)
{
	auto kb = Keyboard::Get().GetState();
	auto mouse = Mouse::Get().GetState();
	auto mouseTK = m_userResources->GetMouseStateTracker();

	// プロジェクション行列
	auto proj = m_userResources->GetProject();
	auto view = m_userResources->GetView();

	auto const r = m_userResources->GetDeviceResources()->GetOutputSize();
	m_player->SetMouseRay(m_player->CreatePickingRay(mouse.x, mouse.y, r.right, r.bottom, *view, *proj));

	if (m_player->CalcRaySphere(m_player->GetMouseRay().position, m_player->GetMouseRay().direction, m_player->GetScene()->GetField().GetCollider().GetPosition(), m_player->GetScene()->GetField().GetCollider().GetRadius(), m_player->GetHitPos()))
	{
		m_player->RotateToMouse();
	}

	m_player->SetVelocity(m_player->GetGravity());

	if (kb.W)
	{
		m_player->SetVelocity(m_player->GetVelocity() - SimpleMath::Vector3::Transform(-SimpleMath::Vector3::UnitX, m_player->GetRotation()));
		m_player->SetPosition(m_player->GetPosition() + m_player->GetVelocity() * elapsedTime);
	}
	else if (kb.S)
	{
		m_player->SetVelocity(m_player->GetVelocity() + SimpleMath::Vector3::Transform(-SimpleMath::Vector3::UnitX, m_player->GetRotation()));
		m_player->SetPosition(m_player->GetPosition() + m_player->GetVelocity() * elapsedTime);
	}
	else
	{
		m_player->ChangeState(m_player->GetStanding());
	}

	if (IsHit(m_player->GetCollider(), m_player->GetScene()->GetBall().GetCollider()) && m_player->GetScene()->GetBall().GetCurrentState() != m_player->GetScene()->GetBall().GetMoving())
	{
		Ball& ball = m_player->GetScene()->GetBall();

		ball.ChangeState(ball.GetCatching());


		ball.SetPosition(m_player->GetPosition() + SimpleMath::Vector3(0.3f,0.3f, 0.3f));

		if (mouseTK->leftButton)
		{
			Ball& ball = m_player->GetScene()->GetBall();

			ball.ChangeState(ball.GetMoving());

			ball.SetVelocity(SimpleMath::Vector3::Transform(SimpleMath::Vector3::UnitX, m_player->GetRotation()));
		}
	}

	m_player->GetCollider().SetPosition(m_player->GetPosition());
}



/// <summary>
/// 描画処理
/// </summary>
void Running::Render()
{
	// デバックフォントの描画
	auto* debugFont = m_userResources->GetDebugFont();

	auto context = m_userResources->GetDeviceResources()->GetD3DDeviceContext();
	auto states = m_userResources->GetCommonStates();
	auto view = m_userResources->GetView();
	auto proj = m_userResources->GetProject();

	// ワールド座標
	SimpleMath::Matrix world;

	SimpleMath::Matrix pos = SimpleMath::Matrix::CreateTranslation(m_player->GetPosition());
	SimpleMath::Matrix scale = SimpleMath::Matrix::CreateScale(SimpleMath::Vector3(0.3f, 0.3f, 0.3f));

	SimpleMath::Matrix rotate = SimpleMath::Matrix::CreateFromQuaternion(m_player->GetRotation()); // ※回転順に合わせて調整

	world = scale * rotate * pos;

	// モデルの描画
	m_model->Draw(context, *states, world, *view, *proj);

	// デバック
	debugFont->Render(L"Running");

}



/// <summary>
/// 終了処理
/// </summary>
void Running::Finalize()
{
}