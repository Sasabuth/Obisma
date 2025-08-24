/// <summary>
/// CameraUpに関するソースファイル
/// </summary>
/// <author>仲森智史</author>
/// <date>2025/07/15</date>

// ヘッダファイルの読み込み
#include "pch.h"
#include "CameraUp.h"

#include "Game/Scenes/GameplayScene.h"
#include "Game/GameObjects//Camera/Camera.h"
#include "Game/GameObjects/Field/Field.h"
#include "DebugDraw.h"
#include "Game/Commons/Resources.h"
#include "Game/GameObjects/Player/Player.h"


// 名前の省略
using namespace DirectX;


/// <summary>
/// コンストラクタ
/// </summary>
CameraUp::CameraUp(Player* player)
	: m_userResources(nullptr)
	, m_player(player)
{
}



/// <summary>
/// デストラクタ
/// </summary>
CameraUp::~CameraUp()
{
}


/// <summary>
/// 初期化処理
/// </summary>
void CameraUp::Initialize(DirectX::SimpleMath::Vector3 position)
{
	m_userResources = UserResources::GetUserResource();

	auto device = m_userResources->GetDeviceResources()->GetD3DDevice();
	auto context = m_userResources->GetDeviceResources()->GetD3DDeviceContext();

	m_position = position;

	m_collider.Initialize(context, m_position, 0.5f);

	// ベーシックエフェクトの作成
    m_basicEffect = std::make_unique<DirectX::BasicEffect>(device);
    m_basicEffect->SetVertexColorEnabled(true);
    
    // プリミティブバッチの作成
    m_primitiveBatch = std::make_unique<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>>(context);
    
    // 入力レイアウトの作成
    CreateInputLayoutFromEffect<DirectX::VertexPositionColor>(device, m_basicEffect.get(), m_inputLayout.ReleaseAndGetAddressOf());
}



/// <summary>
/// 更新処理
/// </summary>
/// <param name="elapsedTime">経過時間</param> 
void CameraUp::Update(float elapsedTime)
{
	// 座標の更新
	m_velocity = m_gravity;

	// 重力の方向
	SimpleMath::Vector3 dir = m_position - m_player->GetPosition();
	dir.Normalize();

	// 方向ベクトルの反転
	SimpleMath::Vector3 targetUp;
	targetUp = -dir;

	// 現在の姿勢制御
	SimpleMath::Vector3 currentUp = SimpleMath::Vector3::Transform(SimpleMath::Vector3::UnitX, m_rotate);

	// 回転軸の計算
	SimpleMath::Vector3 axis = currentUp.Cross(targetUp);
	axis.Normalize();

	// 回転角の計算
	float dot = currentUp.Dot(targetUp);
	float angle = acosf(dot);

	// クォータニオンの作成
	SimpleMath::Quaternion q;

	// 角度が少しでもあれば軸を作る
	if (angle > 0.01f)
	{
		q = SimpleMath::Quaternion::CreateFromAxisAngle(axis, angle);
	}
	// なければ何もしない
	else
	{
		q = SimpleMath::Quaternion::Identity;
	}

	m_rotate *= q;

	SimpleMath::Vector3 dis = m_position - m_player->GetPosition();

	if (dis.Length() >= 4.0f)
	{
		m_velocity -= SimpleMath::Vector3::Transform(-SimpleMath::Vector3::UnitX, m_rotate) * 2;
	}
	else
	{
		m_velocity += SimpleMath::Vector3::Transform(-SimpleMath::Vector3::UnitX, m_rotate) * 2;
	}

	m_position += m_velocity * elapsedTime;

	// コライダーの更新
	m_collider.SetPosition(m_position);
}



/// <summary>
/// 描画処理
/// </summary>
void CameraUp::Render()
{
	auto context = m_userResources->GetDeviceResources()->GetD3DDeviceContext();
	auto states = m_userResources->GetCommonStates();
	auto view = m_userResources->GetView();
	auto proj = m_userResources->GetProject();

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

	SimpleMath::Vector3 forward = SimpleMath::Vector3::Transform(SimpleMath::Vector3(0.0f, 0.0f, 1.0f), m_rotate);
	SimpleMath::Vector3 horizontal = SimpleMath::Vector3::Transform(SimpleMath::Vector3(1.0f, 0.0f, 0.0f), m_rotate);
	SimpleMath::Vector3 vertical = SimpleMath::Vector3::Transform(SimpleMath::Vector3(0.0f, 1.0f, 0.0f), m_rotate);

	m_primitiveBatch->Begin();
	DX::DrawRay(m_primitiveBatch.get(), m_position, forward, false, DirectX::Colors::Yellow);
	DX::DrawRay(m_primitiveBatch.get(), m_position, horizontal, false, DirectX::Colors::Red);
	DX::DrawRay(m_primitiveBatch.get(), m_position, vertical, false, DirectX::Colors::Green);
	m_primitiveBatch->End();

	//// デバックフォントの描画
	//auto* debugFont = m_userResources->GetDebugFont();
	
	//// デバック用
	m_collider.Draw(states, *view, *proj);

	//debugFont->Render(L"Position", m_position);
	//debugFont->Render(L"Quotanion", m_rotate);
	//debugFont->Render(L"hitPos", m_hitPos);
}



/// <summary>
/// 終了処理
/// </summary>
void CameraUp::Finalize()
{
}



/// <summary>
/// 重なりの補填
/// </summary>
/// <param name="field">フィールド</param>
void CameraUp::CorrectOverlap(Field& field)
{
	// 差分を求める
	SimpleMath::Vector3 delta = m_position - field.GetCollider().GetPosition();

	// 長さを求める
	float distance = delta.Length();
	float minDistance = m_collider.GetRadius() + field.GetCollider().GetRadius();

	// 差分を求める
	float pushLength = minDistance - distance;

	// 正規化
	delta.Normalize();

	// 押し出しする
	m_gravity = SimpleMath::Vector3::Zero;
	m_position += delta * pushLength;
}