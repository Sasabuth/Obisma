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
	, m_pUserResources(nullptr)
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
	m_pUserResources = UserResources::GetUserResource();

	auto device = m_pUserResources->GetDeviceResources()->GetD3DDevice();
	auto context = m_pUserResources->GetDeviceResources()->GetD3DDeviceContext();

	// ベーシックエフェクトの作成
	m_basicEffect = std::make_unique<DirectX::BasicEffect>(device);
	m_basicEffect->SetVertexColorEnabled(true);

	// プリミティブバッチの作成
	m_primitiveBatch = std::make_unique<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>>(context);

	// 入力レイアウトの作成
	DirectX::CreateInputLayoutFromEffect<DirectX::VertexPositionColor>(device, m_basicEffect.get(), m_inputLayout.ReleaseAndGetAddressOf());
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

	// 速度の計算
	DirectX::SimpleMath::Vector3 velocity = m_ball->GetVelocity() + m_ball->GetGravity() * elapsedTime;

	// ボールの設定
	m_ball->SetVelocity(velocity * DECELERATIONRATE);  // 徐々に遅くする
	m_ball->SetPosition(m_ball->GetPosition() + m_ball->GetVelocity() * elapsedTime);
	m_ball->GetCollider().SetPosition(m_ball->GetPosition());

	// 速度がなくなったらステート変更
	if (m_ball->GetVelocity().Length() <= STOP_SPEED)
	{
		m_ball->ChangeState(m_ball->GetStopping());
	}
}



/// <summary>
/// 描画処理
/// </summary>
void Moving::Render()
{
	auto context = m_pUserResources->GetDeviceResources()->GetD3DDeviceContext();
	auto states = m_pUserResources->GetCommonStates();
	auto view = m_pUserResources->GetView();
	auto proj = m_pUserResources->GetProject();

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
	// 軸の描画
	context->OMSetBlendState(states->Opaque(), nullptr, 0xFFFFFFFF);

	// 深度の設定
	context->OMSetDepthStencilState(states->DepthDefault(), 0);

	// カリングの設定
	context->RSSetState(states->CullNone());

	// 
	m_basicEffect->SetView(*view);
	m_basicEffect->SetProjection(*proj);
	m_basicEffect->Apply(context);

	// インプットレイアウトの設定
	context->IASetInputLayout(m_inputLayout.Get());

	DirectX::SimpleMath::Vector3 forward = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f), m_ball->GetRotation());
	DirectX::SimpleMath::Vector3 horizontal = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f), m_ball->GetRotation());
	DirectX::SimpleMath::Vector3 vertical = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f), m_ball->GetRotation());

	m_primitiveBatch->Begin();
	DX::DrawRay(m_primitiveBatch.get(), m_ball->GetPosition(), forward, false, DirectX::Colors::Yellow);
	DX::DrawRay(m_primitiveBatch.get(), m_ball->GetPosition(), horizontal, false, DirectX::Colors::Red);
	DX::DrawRay(m_primitiveBatch.get(), m_ball->GetPosition(), vertical, false, DirectX::Colors::Green);
	m_primitiveBatch->End();

	/*auto* debugFont = m_pUserResources->GetDebugFont();*/

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
