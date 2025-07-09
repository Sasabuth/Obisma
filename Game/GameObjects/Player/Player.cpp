/// <summary>
/// Playerに関するソースファイル
/// </summary>
/// <author>仲森智史</author>
/// <date>2025/05/21</date>

// ヘッダファイルの読み込み
#include "pch.h"
#include "Player.h"

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
Player::Player(GameplayScene* pScene, Camera* pCamera)
	: m_pScene(pScene)
	, m_pCamera(pCamera)
	, m_userResources(nullptr)
	, m_currentState{}
{
}



/// <summary>
/// デストラクタ
/// </summary>
Player::~Player()
{
}


/// <summary>
/// 初期化処理
/// </summary>
void Player::Initialize()
{
	m_userResources = UserResources::GetUserResource();

	auto device = m_userResources->GetDeviceResources()->GetD3DDevice();
	auto context = m_userResources->GetDeviceResources()->GetD3DDeviceContext();

	auto effectFactory = m_userResources->GetEffectFactory();
	effectFactory->SetDirectory(L"Resources/Models/");

	m_model = Resources::GetInstance()->GetPlayerModel();

	// ベーシックエフェクトの作成
	m_basicEffect = std::make_unique<DirectX::BasicEffect>(device);
	m_basicEffect->SetVertexColorEnabled(true);

	// プリミティブバッチの作成
	m_primitiveBatch = std::make_unique<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>>(context);

	// 入力レイアウトの作成
	CreateInputLayoutFromEffect<DirectX::VertexPositionColor>(device, m_basicEffect.get(), m_inputLayout.ReleaseAndGetAddressOf());

	m_position = SimpleMath::Vector3{ 3.0f, 3.0f, 2.0f };
	m_velocity = SimpleMath::Vector3::Zero;
	m_gravity = SimpleMath::Vector3::Zero;
	m_rotate = SimpleMath::Quaternion::Identity;

	m_collider.Initialize(context, m_position, 0.6f);

	// 「立つ」状態の生成
	m_standing = std::make_unique<Standing>(this);
	// 「立つ」状態の初期化
	m_standing->Initialize();
	// 「走る」状態の生成
	m_running = std::make_unique<Running>(this);
	// 「走る」状態の初期化
	m_running->Initialize();

	// 立つ状態にする
	m_currentState = m_standing.get();
}



/// <summary>
/// 更新処理
/// </summary>
/// <param name="elapsedTime">経過時間</param> 
void Player::Update(float elapsedTime)
{
	UNREFERENCED_PARAMETER(elapsedTime);

	//m_model->UpdateEffects(
	//	// 引数にラムダ式として処理内容を指定する
	//	[&](IEffect* pEffect)
	//	{
	//		// BasicEffectにキャストする
	//		DirectX::BasicEffect* pBasicEffect = dynamic_cast<DirectX::BasicEffect*>(pEffect);

	//		//// ライトをオフにする
	//		pBasicEffect->SetLightEnabled(0, false);
	//		pBasicEffect->SetLightEnabled(1, false);
	//		pBasicEffect->SetLightEnabled(2, false);

	//		// 自己発光(引数はカラー)
	//		pBasicEffect->SetEmissiveColor(DirectX::SimpleMath::Vector3(1, 1, 1));
	//	}
	//);

	auto kb = Keyboard::Get().GetState();
	auto kbTracker = m_userResources->GetKeyboardStateTracker();
	auto mouse = Mouse::Get().GetState();
	auto mouseTk = m_userResources->GetMouseStateTracker();

	// プロジェクション行列
	auto proj = m_userResources->GetProject();
	// ビュー行列
	auto view = m_pCamera->GetCameraMatrix();

	// 速度の更新
	m_velocity = m_gravity;

	// 現在の軸を取得
	SimpleMath::Vector3 currentUp = SimpleMath::Vector3::Transform(SimpleMath::Vector3::UnitY, m_rotate);
	SimpleMath::Vector3 currentRight = SimpleMath::Vector3::Transform(-SimpleMath::Vector3::UnitX, m_rotate);
	currentUp.Normalize();

	// マウスの位置を取得
	auto const r = m_userResources->GetDeviceResources()->GetOutputSize();

	m_mouseRay = CreatePickingRay(mouse.x, mouse.y, r.right, r.bottom, view, *proj);

	if (CalcRaySphere(m_mouseRay.position, m_mouseRay.direction, m_pScene->GetField().GetCollider().GetPosition(), m_pScene->GetField().GetCollider().GetRadius(), m_hitPos))
	{
		using namespace DirectX::SimpleMath;

		// 重力の方向
		Vector3 gravityDir = m_position - m_hitPos;
		gravityDir.Normalize();

		// 方向ベクトルの反転
		Vector3 targetUp;
		targetUp = -gravityDir;

		// 現在の姿勢制御
		Vector3 currentUp = Vector3::Transform(Vector3::UnitX, m_rotate);

		// 回転軸の計算
		SimpleMath::Vector3 axis = currentUp.Cross(targetUp);
		axis.Normalize();

		// 回転角の計算
		float dot = currentUp.Dot(targetUp);
		float angle = acosf(dot);

		// クォータニオンの作成
		Quaternion q;

		// 角度が少しでもあれば軸を作る
		if (angle > 0.01f)
		{
			q = Quaternion::CreateFromAxisAngle(axis, angle);
		}
		// なければ何もしない
		else
		{
			q = Quaternion::Identity;
		}

		m_rotate *= q;

	}

	if (kb.W)
	{
		m_velocity -= SimpleMath::Vector3::Transform(-SimpleMath::Vector3::UnitX, m_rotate);
	}
	if (kb.S) m_velocity += SimpleMath::Vector3::Transform(-SimpleMath::Vector3::UnitX, m_rotate);

	// 座標の更新
	m_position += m_velocity * elapsedTime;

	// コライダーの更新
	m_collider.SetPosition(m_position);

	m_currentState->Update(elapsedTime);
}



/// <summary>
/// 描画処理
/// </summary>
void Player::Render()
{
	auto context = m_userResources->GetDeviceResources()->GetD3DDeviceContext();
	auto states = m_userResources->GetCommonStates();
	auto view = m_pCamera->GetCameraMatrix();
	auto proj = m_userResources->GetProject();

	// ワールド座標
	SimpleMath::Matrix world;

	SimpleMath::Matrix pos = SimpleMath::Matrix::CreateTranslation(m_position);
	SimpleMath::Matrix scale = SimpleMath::Matrix::CreateScale(SimpleMath::Vector3(0.3f, 0.3f, 0.3f));

	SimpleMath::Matrix rotate = SimpleMath::Matrix::CreateFromQuaternion(m_rotate); // ※回転順に合わせて調整

	world = scale * rotate * pos;

	// モデルの描画
	m_model->Draw(context, *states, world, view, *proj);

	// 軸の描画
	context->OMSetBlendState(states->Opaque(), nullptr, 0xFFFFFFFF);

	// 深度の設定
	context->OMSetDepthStencilState(states->DepthDefault(), 0);

	// カリングの設定
	context->RSSetState(states->CullNone());

	// 
	m_basicEffect->SetView(view);
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

	// デバックフォントの描画
	auto* debugFont = m_userResources->GetDebugFont();
	// デバック用
	/*m_collider.Draw(states, view, *proj);*/
	debugFont->Render(L"Position", m_position);
	debugFont->Render(L"Quotanion", m_rotate);
	debugFont->Render(L"hitPos", m_hitPos);

	m_currentState->Render();
}



/// <summary>
/// 終了処理
/// </summary>
void Player::Finalize()
{
	m_currentState->Finalize();
}



/// <summary>
/// 座標の設定
/// </summary>
/// <param name="position">座標</param>
void Player::SetPosition(DirectX::SimpleMath::Vector3 position)
{
	m_position = position;
}



/// <summary>
/// 座標の取得
/// </summary>
/// <returns>座標</returns> 
DirectX::SimpleMath::Vector3 Player::GetPosition() const
{
	return m_position;
}



/// <summary>
/// 速度の設定
/// </summary>
/// <param name="velocity">速度</param>
void Player::SetVelocity(DirectX::SimpleMath::Vector3 velocity)
{
	m_velocity = velocity;
}



/// <summary>
/// 速度の取得
/// </summary>
/// <returns>速度</returns> 
DirectX::SimpleMath::Vector3 Player::GetVelocity() const
{
	return m_velocity;
}



/// <summary>
/// 回転の設定
/// </summary>
/// <param name="rotation">回転</param>
void Player::SetRotation(DirectX::SimpleMath::Quaternion rotation)
{
	m_rotate = rotation;
}



/// <summary>
/// 回転の取得
/// </summary>
/// <returns>回転</returns>
DirectX::SimpleMath::Quaternion Player::GetRotation() const
{
	return m_rotate;
}



/// <summary>
/// 重力の設定
/// </summary>
/// <param name="gravity">重力</param> 
void Player::SetGravity(SimpleMath::Vector3 gravity)
{
	m_gravity = gravity;
}



/// <summary>
/// 重力の取得
/// </summary>
/// <returns>重力</returns>
SimpleMath::Vector3 Player::GetGravity() const
{
	return m_gravity;
}



/// <summary>
/// 重なりの補填
/// </summary>
/// <param name="field">フィールド</param>
void Player::CorrectOverlap(Field& field)
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



/// <summary>
/// コライダーの取得
/// </summary>
/// <returns></returns>
SphereCollider& Player::GetCollider()
{
	return m_collider;
}



/// <summary>
/// レイの作成
/// </summary>
/// <param name="mouseX">X軸のマウス</param>
/// <param name="mouseY">Y軸のマウス</param>
/// <param name="screenWidth">横のスクリーンサイズ</param>
/// <param name="screenHeight">縦のスクリーンサイズ</param>
/// <param name="view">ビュー行列</param>
/// <param name="proj">プロジェクション行列</param>
/// <returns></returns>
DirectX::SimpleMath::Ray Player::CreatePickingRay(int mouseX, int mouseY, int screenWidth, int screenHeight, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj)
{
	// マウスの座標
	float px = (2.0f * mouseX / screenWidth - 1.0f);
	float py = (1.0f - 2.0f * mouseY / screenHeight); // Y軸は反転

	// 2点を作ってレイを飛ばす
	SimpleMath::Vector3 nearPoint = SimpleMath::Vector3(px, py, 0.0f);
	SimpleMath::Vector3 farPoint = SimpleMath::Vector3(px, py, 1.0f);

	// ワールド座標に変換
	SimpleMath::Matrix viewProj = view * proj;
	SimpleMath::Matrix invViewProj;
	viewProj.Invert(invViewProj);

	// レイの座標とベクトルを求める
	SimpleMath::Vector3 rayOrigin = SimpleMath::Vector3::Transform(nearPoint, invViewProj);
	SimpleMath::Vector3 rayTarget = SimpleMath::Vector3::Transform(farPoint, invViewProj);
	SimpleMath::Vector3 rayDir = rayTarget - rayOrigin;
	rayDir.Normalize();

	return SimpleMath::Ray(rayOrigin, rayDir);
}



/// <summary>
/// レイと球体の交差
/// </summary>
/// <param name="rayPos">レイの座標</param>
/// <param name="rayDir">レイのベクトル</param>
/// <param name="spherePos">球の座標</param>
/// <param name="radius">半径</param>
/// <param name="hitPos">当たった座標</param>
/// <returns>[true] 当たった　[false] 当たってない</returns>
bool Player::CalcRaySphere(DirectX::SimpleMath::Vector3 rayPos, DirectX::SimpleMath::Vector3 rayDir, DirectX::SimpleMath::Vector3 spherePos, float radius, DirectX::SimpleMath::Vector3& hitPos)
{
	spherePos.x = spherePos.x - rayPos.x;
	spherePos.y = spherePos.y - rayPos.y;
	spherePos.z = spherePos.z - rayPos.z;

	float A = rayDir.x * rayDir.x + rayDir.y * rayDir.y + rayDir.z * rayDir.z;
	float B = rayDir.x * spherePos.x + rayDir.y * spherePos.y + rayDir.z * spherePos.z;
	float C = spherePos.x * spherePos.x + spherePos.y * spherePos.y + spherePos.z * spherePos.z - radius * radius;

	// レイが存在するか
	if (A == 0.0f)
		return false; 

	// 衝突しているか
	float s = B * B - A * C;
	if (s < 0.0f)
		return false; 

	s = sqrtf(s);
	float a1 = (B - s) / A;
	float a2 = (B + s) / A;

	// マイナス方向に当たっていないか
	if (a1 < 0.0f || a2 < 0.0f)
		return false; 
	
	// 当たった座標を入れる
	hitPos.x = rayPos.x + a1 * rayDir.x;
	hitPos.y = rayPos.y + a1 * rayDir.y;
	hitPos.z = rayPos.z + a1 * rayDir.z;

	return true;
}