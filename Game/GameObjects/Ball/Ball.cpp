/// <summary>
/// Ballに関するソースファイル
/// </summary>
/// <author>仲森智史</author>
/// <date>2025/05/21</date>

// ヘッダファイルの読み込み
#include "pch.h"
#include "Ball.h"

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
Ball::Ball(GameplayScene* pScene)
	: m_pScene(pScene)
	, m_currentState{}
	, m_userResources(nullptr)
	, m_hitPos{}
{
}



/// <summary>
/// デストラクタ
/// </summary>
Ball::~Ball()
{
}


/// <summary>
/// 初期化処理
/// </summary>
void Ball::Initialize(DirectX::SimpleMath::Vector3 position)
{
	m_userResources = UserResources::GetUserResource();
	auto device = m_userResources->GetDeviceResources()->GetD3DDevice();
	auto context = m_userResources->GetDeviceResources()->GetD3DDeviceContext();

	m_position = position;

	m_collider.Initialize(context, m_position, BALL_SIZE);

	// 「立つ」状態の生成
	m_stopping = std::make_unique<Stopping>(this);
	// 「立つ」状態の初期化
	m_stopping->Initialize();
	// 「走る」状態の生成
	m_moving = std::make_unique<Moving>(this);
	// 「走る」状態の初期化
	m_moving->Initialize();
	// 「とられている」状態の生成
	m_catching = std::make_unique<Catching>(this);
	// 「とられている」状態の初期化
	m_catching->Initialize();

	// 立つ状態にする
	m_currentState = m_stopping.get();

	// 影の初期化
	InitializeShadow(device, context);
}



/// <summary>
/// 更新処理
/// </summary>
/// <param name="elapsedTime">経過時間</param> 
void Ball::Update(float elapsedTime)
{
	m_currentState->Update(elapsedTime);
}



/// <summary>
/// 描画処理
/// </summary>
void Ball::Render()
{
	m_currentState->Render();
}



/// <summary>
/// 終了処理
/// </summary>
void Ball::Finalize()
{
	m_currentState->Finalize();
}



/// <summary>
/// 重なりの補填
/// </summary>
/// <param name="field">フィールド</param>
void Ball::CorrectOverlap(Field& field)
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
/// ステートの変更
/// </summary>
/// <param name="newState">新しいステート</param>
void Ball::ChangeState(IState* newState)
{
	m_currentState = newState;
	m_currentState->Initialize();
}



/// <summary>
/// 影の初期化
/// </summary>
/// <param name="device">デバイス</param>
/// <param name="context">コンテキスト</param>
void Ball::InitializeShadow(ID3D11Device* device, ID3D11DeviceContext* context)
{
	// ベーシックエフェクトの作成
	m_basicEffect = std::make_unique<BasicEffect>(device);
	// ライティングOFF
	m_basicEffect->SetLightingEnabled(false);
	// 頂点カラーOFF
	m_basicEffect->SetVertexColorEnabled(false);
	// テクスチャON
	m_basicEffect->SetTextureEnabled(true);

	// 入力レイアウトの作成
	DX::ThrowIfFailed(
		CreateInputLayoutFromEffect<VertexPositionTexture>(
			device,
			m_basicEffect.get(),
			m_inputLayout.ReleaseAndGetAddressOf())
	);

	// プリミティブバッチの作成
	m_primitiveBatch = std::make_unique<PrimitiveBatch<VertexPositionTexture>>(context);

	// テクスチャの読み込み
	m_shadowTexture = Resources::GetInstance()->GetShadowTexture();
}



/// <summary>
/// 影の描画
/// </summary>
/// <param name="context">コンテキスト</param>
/// <param name="states">コモンステート</param>
/// <param name="radius">半径</param>
void Ball::DrawShadow(ID3D11DeviceContext* context, DirectX::CommonStates* states, float radius)
{
	auto view = m_userResources->GetView();
	auto proj = m_userResources->GetProject();

	// エフェクトの設定＆適用
	m_basicEffect->SetWorld(SimpleMath::Matrix::Identity);
	m_basicEffect->SetView(*view);
	m_basicEffect->SetProjection(*proj);
	m_basicEffect->SetTexture(m_shadowTexture.Get());
	m_basicEffect->Apply(context);

	// 入力レイアウト
	context->IASetInputLayout(m_inputLayout.Get());

	// テクスチャサンプラー
	ID3D11SamplerState* sampler[] = { states->LinearClamp() };
	context->PSSetSamplers(0, 1, sampler);

	// アルファブレンド
	context->OMSetBlendState(states->AlphaBlend(), nullptr, 0xffffffff);

	VertexPositionTexture vertexes[] =
	{
		VertexPositionTexture(SimpleMath::Vector3::Zero, SimpleMath::Vector2(0.0f, 0.0f)),  // 0
		VertexPositionTexture(SimpleMath::Vector3::Zero, SimpleMath::Vector2(1.0f, 0.0f)),  // 1
		VertexPositionTexture(SimpleMath::Vector3::Zero, SimpleMath::Vector2(0.0f, 1.0f)),  // 2
		VertexPositionTexture(SimpleMath::Vector3::Zero, SimpleMath::Vector2(1.0f, 1.0f))   // 3
	};

	uint16_t indexes[] = { 2,3,1,2,1,0 };

	vertexes[0].position = SimpleMath::Vector3(-radius, 0.01f, -radius);
	vertexes[1].position = SimpleMath::Vector3(radius, 0.01f, -radius);
	vertexes[2].position = SimpleMath::Vector3(-radius, 0.01f, radius);
	vertexes[3].position = SimpleMath::Vector3(radius, 0.01f, radius);

	// レイ
	SimpleMath::Ray ray(m_position, m_gravity);

	// レイの当たったところの座標を設定
	CalcRaySphere(ray.position, ray.direction, m_pScene->GetField().GetCollider().GetPosition(), m_pScene->GetField().GetCollider().GetRadius(), m_hitPos);
	for (int i = 0; i < 4; ++i)
	{
		SimpleMath::Vector3 rotatedOffset = SimpleMath::Vector3::Transform(vertexes[i].position, m_rotate);
		vertexes[i].position = rotatedOffset + m_hitPos;
	}

	// 影の描画
	m_primitiveBatch->Begin();

	m_primitiveBatch->DrawIndexed(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, indexes, _countof(indexes), vertexes, _countof(vertexes));

	m_primitiveBatch->End();
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
void Ball::CalcRaySphere(DirectX::SimpleMath::Vector3 rayPos, DirectX::SimpleMath::Vector3 rayDir, DirectX::SimpleMath::Vector3 spherePos, float radius, DirectX::SimpleMath::Vector3& hitPos)
{
	spherePos.x = spherePos.x - rayPos.x;
	spherePos.y = spherePos.y - rayPos.y;
	spherePos.z = spherePos.z - rayPos.z;

	float A = rayDir.x * rayDir.x + rayDir.y * rayDir.y + rayDir.z * rayDir.z;
	float B = rayDir.x * spherePos.x + rayDir.y * spherePos.y + rayDir.z * spherePos.z;
	float C = spherePos.x * spherePos.x + spherePos.y * spherePos.y + spherePos.z * spherePos.z - radius * radius;

	// レイが存在するか
	if (A == 0.0f)
		return;

	// 衝突しているか
	float s = B * B - A * C;
	if (s < 0.0f)
		return;

	s = sqrtf(s);
	float a1 = (B - s) / A;
	float a2 = (B + s) / A;

	// マイナス方向に当たっていないか
	if (a1 < 0.0f || a2 < 0.0f)
		return;

	// 当たった座標を入れる
	hitPos.x = rayPos.x + a1 * rayDir.x;
	hitPos.y = rayPos.y + a1 * rayDir.y;
	hitPos.z = rayPos.z + a1 * rayDir.z;
}
