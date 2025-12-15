/// <summary>
/// CameraUpに関するソースファイル
/// </summary>
/// <author>仲森智史</author>

// ヘッダファイルの読み込み
#include "pch.h"
#include "CameraUp.h"

#include "Game/GameObjects/Player/Player.h"
#include "Game/GameObjects/Field/Field.h"
#include "DebugDraw.h"
#include "Game/Commons/Resources.h"




/// <summary>
/// コンストラクタ
/// </summary>
CameraUp::CameraUp(Player* pPlayer)
	: m_pUserResources(nullptr)
	, m_pPlayer(pPlayer)
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
	m_pUserResources = UserResources::GetUserResource();

	auto device = m_pUserResources->GetDeviceResources()->GetD3DDevice();
	auto context = m_pUserResources->GetDeviceResources()->GetD3DDeviceContext();

	m_position = position;

	m_collider.Initialize(context, m_position, 0.5f);

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
void CameraUp::Update(float elapsedTime)
{
	// 座標の更新
	m_velocity = m_gravity;

	// 重力の方向
	DirectX::SimpleMath::Vector3 dir = m_position - m_pPlayer->GetPosition();
	dir.Normalize();

	// 方向ベクトルの反転
	DirectX::SimpleMath::Vector3 targetUp;
	targetUp = -dir;

	// 現在の姿勢制御
	DirectX::SimpleMath::Vector3 currentUp = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitX, m_rotate);

	// 回転軸の計算
	DirectX::SimpleMath::Vector3 axis = currentUp.Cross(targetUp);
	axis.Normalize();

	// 回転角の計算
	float dot = currentUp.Dot(targetUp);
	float angle = acosf(dot);

	// クォータニオンの作成
	DirectX::SimpleMath::Quaternion q;

	// 角度が少しでもあれば軸を作る
	if (angle > 0.01f)
	{
		q = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(axis, angle);
	}
	// なければ何もしない
	else
	{
		q = DirectX::SimpleMath::Quaternion::Identity;
	}

	m_rotate *= q;

	DirectX::SimpleMath::Vector3 dis = m_position - m_pPlayer->GetPosition();

	if (dis.Length() >= 4.0f)
	{
		m_velocity += DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitX, m_rotate) * 3;
	}
	else
	{
		m_velocity += DirectX::SimpleMath::Vector3::Transform(-DirectX::SimpleMath::Vector3::UnitX, m_rotate) * 3;
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
	auto context = m_pUserResources->GetDeviceResources()->GetD3DDeviceContext();
	auto states = m_pUserResources->GetCommonStates();
	auto view = m_pUserResources->GetView();
	auto proj = m_pUserResources->GetProject();

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

	DirectX::SimpleMath::Vector3 forward = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f), m_rotate);
	DirectX::SimpleMath::Vector3 horizontal = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f), m_rotate);
	DirectX::SimpleMath::Vector3 vertical = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f), m_rotate);

	m_primitiveBatch->Begin();
	DX::DrawRay(m_primitiveBatch.get(), m_position, forward, false, DirectX::Colors::Yellow);
	DX::DrawRay(m_primitiveBatch.get(), m_position, horizontal, false, DirectX::Colors::Red);
	DX::DrawRay(m_primitiveBatch.get(), m_position, vertical, false, DirectX::Colors::Green);
	m_primitiveBatch->End();

	//// デバックフォントの描画
	//auto* debugFont = m_pUserResources->GetDebugFont();

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
	DirectX::SimpleMath::Vector3 delta = m_position - field.GetCollider().GetPosition();

	// 長さを求める
	float distance = delta.Length();
	float minDistance = m_collider.GetRadius() + field.GetCollider().GetRadius();

	// 差分を求める
	float pushLength = minDistance - distance;

	delta.Normalize();
	m_position += delta * pushLength;
}



/// <summary>
/// 重なりの補填
/// </summary>
/// <param name="field">座標</param>
void CameraUp::CorrectOverlap(DirectX::SimpleMath::Vector3& pos)
{
	// 差分を求める
	DirectX::SimpleMath::Vector3 delta = m_position - pos;

	// 長さを求める
	float distance = delta.Length();
	float r = m_collider.GetRadius();

	// 差分を求める
	float pushLength = r - distance;

	// 正規化
	delta.Normalize();

	// 押し出しする
	m_position += delta * pushLength;
}
