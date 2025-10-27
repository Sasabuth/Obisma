/// <summary>
/// Catchingに関するソースファイル
/// </summary>
/// <author>仲森智史</author>
/// <date>2025/05/21</date>

// ヘッダファイルの読み込み
#include "pch.h"
#include "Catching.h"

#include "Game/Scenes/GameplayScene.h"
#include "Game/GameObjects/Field/Field.h"
#include "DebugDraw.h"
#include "Game/Commons/Resources.h"
#include "Game/GameObjects/Ball/Ball.h"



/// <summary>
/// コンストラクタ
/// </summary>
Catching::Catching(Ball* ball)
	: m_ball(ball)
	, m_userResources(nullptr)
{
}



/// <summary>
/// デストラクタ
/// </summary>
Catching::~Catching()
{
}


/// <summary>
/// 初期化処理
/// </summary>
void Catching::Initialize()
{
	// ユーザーリソースの取得
	m_userResources = UserResources::GetUserResource();
}



/// <summary>
/// 更新処理
/// </summary>
/// <param name="elapsedTime">経過時間</param> 
void Catching::Update(float elapsedTime)
{
	UNREFERENCED_PARAMETER(elapsedTime);

	// プレイヤーの設定
	m_ball->SetVelocity(DirectX::SimpleMath::Vector3::Zero);
	m_ball->GetCollider().SetPosition(m_ball->GetPosition());
}



/// <summary>
/// 描画処理
/// </summary>
void Catching::Render()
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

	// アニメーションモデルを描画
	if (m_ball->GetInvincibleTime() >= 0.0f && sinf(m_ball->GetInvincibleTime() * 10) <= 0.0f)
	{
		return;
	}

	// モデルの描画
	m_ball->GetModel()->Draw(context, *states, world, *view, *proj);

	// 影の描画
	m_ball->DrawShadow(context, states, Resources::GetInstance()->GetJson(L"Ball.json")["ShadowSize"]);

	// デバック
	// デバックフォントの描画
	//auto* debugFont = m_userResources->GetDebugFont();
	/*debugFont->Render(L"Catching");
	debugFont->Render(L"Position", m_ball->GetPosition());*/

}



/// <summary>
/// 終了処理
/// </summary>
void Catching::Finalize()
{
}