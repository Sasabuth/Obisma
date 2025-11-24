/// <summary>
/// Movingに関するソースファイル
/// </summary>
/// <author>仲森智史</author>
/// <date>2025/05/21</date>

// ヘッダファイルの読み込み
#include "pch.h"
#include "Moving.h"

#include "DebugDraw.h"
#include "Game/Commons/Resources.h"
#include "Game/GameObjects/Ball/Ball.h"



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
	// 音が鳴っていたら連続して鳴らさないようにする
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
	auto context = m_userResources->GetDeviceResources()->GetD3DDeviceContext();
	auto states = m_userResources->GetCommonStates();
	auto view = m_userResources->GetView();
	auto proj = m_userResources->GetProject();

	// ワールド座標
	DirectX::SimpleMath::Matrix world;

	DirectX::SimpleMath::Matrix pos = DirectX::SimpleMath::Matrix::CreateTranslation(m_ball->GetPosition());
	DirectX::SimpleMath::Matrix scale = DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(Resources::GetInstance()->GetJson(L"Ball.json")["BallSize"]));

	DirectX::SimpleMath::Matrix rotate = DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_ball->GetRotation());

	world = scale * rotate * pos;

	// モデルの描画
	m_ball->GetModel()->Draw(context, *states, world, *view, *proj);

	// 影の描画
	m_ball->DrawShadow(context, states, Resources::GetInstance()->GetJson(L"Ball.json")["ShadowSize"]);


	// デバック
	/*auto* debugFont = m_userResources->GetDebugFont();*/

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

void Moving::EventHandle(Event e)
{
	UNREFERENCED_PARAMETER(e);
}
