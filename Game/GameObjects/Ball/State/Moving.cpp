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
	// ユーザーリソースの取得
	m_userResources = UserResources::GetUserResource();
}



/// <summary>
/// 更新処理
/// </summary>
/// <param name="elapsedTime">経過時間</param> 
void Moving::Update(float elapsedTime)
{
	auto kb = Keyboard::Get().GetState();
	auto mouse = Mouse::Get().GetState();

	if (m_ball->GetIsSound())
	{
		m_ball->NoSound();
	}
	

	// ボールの設定
	m_ball->SetVelocity(m_ball->GetVelocity() + m_ball->GetGravity() * elapsedTime);
	m_ball->SetPosition(m_ball->GetPosition() + m_ball->GetVelocity() * elapsedTime);
	m_ball->GetCollider().SetPosition(m_ball->GetPosition());

	////// 速度がなくなったらステート変更
	if (m_ball->GetVelocity().Length() <= 0.05f)
	{
		m_ball->ChangeState(m_ball->GetStopping());
	}
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

	SimpleMath::Matrix rotate = SimpleMath::Matrix::CreateFromQuaternion(m_ball->GetRotation());

	world = scale * rotate * pos;

	// モデルの描画
	m_ball->GetModel()->Draw(context, *states, world, *view, *proj);

	// 影の描画
	m_ball->DrawShadow(context, states, Ball::SHADOW_SIZE);


	// デバック
	/*debugFont->Render(L"Moving");
	debugFont->Render(L"Speed",m_ball->GetVelocity().Length());*/
	//debugFont->Render(L"Length", m_ball->GetVelocity());
}



/// <summary>
/// 終了処理
/// </summary>
void Moving::Finalize()
{
}