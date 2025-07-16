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


// 名前の省略
using namespace DirectX;


/// <summary>
/// コンストラクタ
/// </summary>
Catching::Catching(Ball* ball)
	: m_ball(ball)
	, m_userResources(nullptr)
	, m_model{}
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

	// モデルの取得
	m_model = Resources::GetInstance()->GetBallModel();
}



/// <summary>
/// 更新処理
/// </summary>
/// <param name="elapsedTime">経過時間</param> 
void Catching::Update(float elapsedTime)
{
	UNREFERENCED_PARAMETER(elapsedTime);

	auto mouse = Mouse::Get().GetState();

	// プレイヤーの設定
	m_ball->SetVelocity(SimpleMath::Vector3::Zero);
	m_ball->GetCollider().SetPosition(m_ball->GetPosition());
}



/// <summary>
/// 描画処理
/// </summary>
void Catching::Render()
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
	/*m_ball->GetCollider().Draw(states, *view, *proj);*/
	debugFont->Render(L"Catching");
	debugFont->Render(L"Position", m_ball->GetPosition());

}



/// <summary>
/// 終了処理
/// </summary>
void Catching::Finalize()
{
}