/// <summary>
/// Arrowに関するソースファイル
/// </summary>
/// <author>仲森智史</author>

// ヘッダファイルの読み込み
#include "pch.h"
#include "Arrow.h"

#include "Game/Scenes/GameplayScene.h"
#include "Game/GameObjects//Camera/Camera.h"
#include "Game/GameObjects/Field/Field.h"
#include "DebugDraw.h"
#include "Game/Commons/Resources.h"
#include "Game/GameObjects/Player/Player.h"
#include "Game/GameObjects/Camera/Camera.h"



/// <summary>
/// コンストラクタ
/// </summary>
Arrow::Arrow(Player* pPlayer)
	: m_pUserResources(nullptr)
	, m_pPlayer(pPlayer)
	, m_waveSpeed(0.0f)
	, m_isDraw(false)
{
}



/// <summary>
/// デストラクタ
/// </summary>
Arrow::~Arrow()
{
}


/// <summary>
/// 初期化処理
/// </summary>
void Arrow::Initialize(DirectX::SimpleMath::Vector3 position)
{
	// ユーザーリソースの取得
	m_pUserResources = UserResources::GetUserResource();

	auto device = m_pUserResources->GetDeviceResources()->GetD3DDevice();
	auto context = m_pUserResources->GetDeviceResources()->GetD3DDeviceContext();

	// 座標の初期化
	m_position = position;

	// 波の速度の初期化
	m_waveSpeed = 0.0f;

	// コライダーの初期化
	m_collider.Initialize(context, m_position, 0.5f);

	// 描画しない
	m_isDraw = false;

	// 影の初期化
	InitializeArrow(device, context);
}



/// <summary>
/// 更新処理
/// </summary>
/// <param name="elapsedTime">経過時間</param> 
void Arrow::Update(float elapsedTime)
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
	DirectX::SimpleMath::Vector3 currentUp = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitZ, m_rotate);

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

	// 回転の計算
	m_rotate *= q;

	// 上下移動
	// 初期位置を保存しておく
	static DirectX::SimpleMath::Vector3 basePos = m_position;

	// サイン波用の上向きベクトルの取得
	DirectX::SimpleMath::Vector3 sinWave = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitY, m_rotate);

	// 波の速度の更新
	m_waveSpeed += 1.0f * elapsedTime;

	// 座標の更新
	m_position = basePos+sinWave * (sin(m_waveSpeed) + 1.5f) * 0.2f;

	// コライダーの更新
	m_collider.SetPosition(m_position);
}



/// <summary>
/// 描画処理
/// </summary>
void Arrow::Render()
{
	auto context = m_pUserResources->GetDeviceResources()->GetD3DDeviceContext();
	auto states = m_pUserResources->GetCommonStates();
	auto view = m_pUserResources->GetView();
	auto proj = m_pUserResources->GetProject();

	// エフェクトの設定a
	m_basicEffect->SetWorld(DirectX::SimpleMath::Matrix::Identity);
	m_basicEffect->SetView(*view);
	m_basicEffect->SetProjection(*proj);
	m_basicEffect->SetTexture(m_arrowTexture.Get());
	m_basicEffect->Apply(context);

	// カリングの設定
	context->RSSetState(states->CullNone());

	// 入力レイアウト
	context->IASetInputLayout(m_inputLayout.Get());

	// テクスチャサンプラー
	ID3D11SamplerState* sampler[] = { states->LinearClamp() };
	context->PSSetSamplers(0, 1, sampler);

	// アルファブレンド
	context->OMSetBlendState(states->AlphaBlend(), nullptr, 0xffffffff);

	DirectX::VertexPositionTexture vertexes[] =
	{
		DirectX::VertexPositionTexture(DirectX::SimpleMath::Vector3::Zero, DirectX::SimpleMath::Vector2(0.0f, 0.0f)),  // 0
		DirectX::VertexPositionTexture(DirectX::SimpleMath::Vector3::Zero, DirectX::SimpleMath::Vector2(1.0f, 0.0f)),  // 1
		DirectX::VertexPositionTexture(DirectX::SimpleMath::Vector3::Zero, DirectX::SimpleMath::Vector2(0.0f, 1.0f)),  // 2
		DirectX::VertexPositionTexture(DirectX::SimpleMath::Vector3::Zero, DirectX::SimpleMath::Vector2(1.0f, 1.0f))   // 3
	};

	uint16_t indexes[] = { 2,3,1,2,1,0 };

	vertexes[0].position = DirectX::SimpleMath::Vector3(-ARROW_SIZE,  ARROW_SIZE, 0.01f);
	vertexes[1].position = DirectX::SimpleMath::Vector3( ARROW_SIZE,  ARROW_SIZE, 0.01f);
	vertexes[2].position = DirectX::SimpleMath::Vector3(-ARROW_SIZE, -ARROW_SIZE, 0.01f);
	vertexes[3].position = DirectX::SimpleMath::Vector3( ARROW_SIZE, -ARROW_SIZE, 0.01f);

	for (int i = 0; i < 4; ++i)
	{
		DirectX::SimpleMath::Vector3 rotatedOffset = DirectX::SimpleMath::Vector3::Transform(vertexes[i].position, m_rotate);
		vertexes[i].position = rotatedOffset + m_position;
	}

	// 影の描画
	m_primitiveBatch->Begin();
	m_primitiveBatch->DrawIndexed(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, indexes, _countof(indexes), vertexes, _countof(vertexes));
	m_primitiveBatch->End();

	/*m_collider.Draw(states, *view, *proj);*/

	//debugFont->Render(L"Position", m_position);
	//debugFont->Render(L"Quotanion", m_rotate);
	//debugFont->Render(L"hitPos", m_hitPos);
}



/// <summary>
/// 終了処理
/// </summary>
void Arrow::Finalize()
{
}



/// <summary>
/// 重なりの補填
/// </summary>
/// <param name="field">フィールド</param>
void Arrow::CorrectOverlap(Field& field)
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
/// 矢印の初期化
/// </summary>
/// <param name="device">デバイス</param>
/// <param name="context">コンテキスト</param>
void Arrow::InitializeArrow(ID3D11Device* device, ID3D11DeviceContext* context)
{
	// ベーシックエフェクトの作成
	m_basicEffect = std::make_unique<DirectX::BasicEffect>(device);
	// ライティングOFF
	m_basicEffect->SetLightingEnabled(false);
	// 頂点カラーOFF
	m_basicEffect->SetVertexColorEnabled(false);
	// テクスチャON
	m_basicEffect->SetTextureEnabled(true);

	// 入力レイアウトの作成
	DX::ThrowIfFailed(
		DirectX::CreateInputLayoutFromEffect<DirectX::VertexPositionTexture>(
			device,
			m_basicEffect.get(),
			m_inputLayout.ReleaseAndGetAddressOf())
	);

	// プリミティブバッチの作成
	m_primitiveBatch = std::make_unique<DirectX::PrimitiveBatch<DirectX::VertexPositionTexture>>(context);

	// テクスチャの読み込み
	m_arrowTexture = Resources::GetInstance()->GetTexture(L"Arrow.png");
}