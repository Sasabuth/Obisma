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
	//UNREFERENCED_PARAMETER(elapsedTime);

	////m_model->UpdateEffects(
	////	// 引数にラムダ式として処理内容を指定する
	////	[&](IEffect* pEffect)
	////	{
	////		// BasicEffectにキャストする
	////		DirectX::BasicEffect* pBasicEffect = dynamic_cast<DirectX::BasicEffect*>(pEffect);

	////		//// ライトをオフにする
	////		pBasicEffect->SetLightEnabled(0, false);
	////		pBasicEffect->SetLightEnabled(1, false);
	////		pBasicEffect->SetLightEnabled(2, false);

	////		// 自己発光(引数はカラー)
	////		pBasicEffect->SetEmissiveColor(DirectX::SimpleMath::Vector3(1, 1, 1));
	////	}
	////);

	//auto kb = Keyboard::Get().GetState();
	//auto kbTracker = m_userResources->GetKeyboardStateTracker();
	//auto mouse = Mouse::Get().GetState();
	//auto mouseTk = m_userResources->GetMouseStateTracker();

	//// プロジェクション行列
	//auto proj = m_userResources->GetProject();
	//// ビュー行列
	//auto view = m_userResources->GetView();

	//// 速度の更新
	//m_velocity = m_gravity;

	//// 現在の軸を取得
	//SimpleMath::Vector3 currentUp = SimpleMath::Vector3::Transform(SimpleMath::Vector3::UnitY, m_rotate);
	//SimpleMath::Vector3 currentRight = SimpleMath::Vector3::Transform(-SimpleMath::Vector3::UnitX, m_rotate);
	//currentUp.Normalize();

	//// マウスの位置を取得
	//auto const r = m_userResources->GetDeviceResources()->GetOutputSize();

	//m_mouseRay = CreatePickingRay(mouse.x, mouse.y, r.right, r.bottom, *view, *proj);

	//if (CalcRaySphere(m_mouseRay.position, m_mouseRay.direction, m_pScene->GetField().GetCollider().GetPosition(), m_pScene->GetField().GetCollider().GetRadius(), m_hitPos))
	//{
	//	using namespace DirectX::SimpleMath;

	//	// 重力の方向
	//	Vector3 gravityDir = m_position - m_hitPos;
	//	gravityDir.Normalize();

	//	// 方向ベクトルの反転
	//	Vector3 targetUp;
	//	targetUp = -gravityDir;

	//	// 現在の姿勢制御
	//	Vector3 currentUp = Vector3::Transform(Vector3::UnitX, m_rotate);

	//	// 回転軸の計算
	//	SimpleMath::Vector3 axis = currentUp.Cross(targetUp);
	//	axis.Normalize();

	//	// 回転角の計算
	//	float dot = currentUp.Dot(targetUp);
	//	float angle = acosf(dot);

	//	// クォータニオンの作成
	//	Quaternion q;

	//	// 角度が少しでもあれば軸を作る
	//	if (angle > 0.01f)
	//	{
	//		q = Quaternion::CreateFromAxisAngle(axis, angle);
	//	}
	//	// なければ何もしない
	//	else
	//	{
	//		q = Quaternion::Identity;
	//	}

	//	m_rotate *= q;

	//}

	//if (kb.W)
	//{
	//	m_velocity -= SimpleMath::Vector3::Transform(-SimpleMath::Vector3::UnitX, m_rotate);
	//}
	//if (kb.S) m_velocity += SimpleMath::Vector3::Transform(-SimpleMath::Vector3::UnitX, m_rotate);

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

	SimpleMath::Vector3 a = m_position - m_player->GetPosition();

	if (a.Length() >= 1.0f)
	{
		m_velocity -= SimpleMath::Vector3::Transform(-SimpleMath::Vector3::UnitX, m_rotate)*2;
	}
	else
	{
		m_velocity += SimpleMath::Vector3::Transform(-SimpleMath::Vector3::UnitX, m_rotate)*2;
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

	//// ワールド座標
	//SimpleMath::Matrix world;

	//SimpleMath::Matrix pos = SimpleMath::Matrix::CreateTranslation(m_position);
	//SimpleMath::Matrix scale = SimpleMath::Matrix::CreateScale(SimpleMath::Vector3(0.3f, 0.3f, 0.3f));

	//SimpleMath::Matrix rotate = SimpleMath::Matrix::CreateFromQuaternion(m_rotate); // ※回転順に合わせて調整

	//world = scale * rotate * pos;

	//// モデルの描画
	//m_model->Draw(context, *states, world, *view, *proj);

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