/// <summary>
/// AirTargetに関するソースファイル
/// </summary>
/// <author>仲森智史</author>

// ヘッダファイルの読み込み
#include "pch.h"
#include "AirTarget.h"

#include "Common/DebugDraw.h"
#include "Game/Commons/Resources.h"
#include "Game/Commons/Factory.h"
#include "Game/Commons/Messenger.h"
#include "Game/GameObjects/Camera/Camera.h"
#include "Game/GameObjects/Field/Field.h"




/// <summary>
/// コンストラクタ
/// </summary>
AirTarget::AirTarget()
	: m_currentState{}
	, m_pUserResources(nullptr)
	, m_model(nullptr)
	, m_shadowHitPos{}
{
	// オブジェクト番号とオブジェクトを登録する
	Messenger::GetInstance()->Register(Factory::AIRTARGET, this);
}



/// <summary>
/// デストラクタ
/// </summary>
AirTarget::~AirTarget()
{
}


/// <summary>
/// 初期化処理
/// </summary>
void AirTarget::Initialize(DirectX::SimpleMath::Vector3 position)
{
	// ユーザーリソースの取得
	m_pUserResources = UserResources::GetUserResource();

	auto device = m_pUserResources->GetDeviceResources()->GetD3DDevice();
	auto context = m_pUserResources->GetDeviceResources()->GetD3DDeviceContext();

	// 座標の設定
	m_position = position;

	// コライダーの初期化
	m_collider.Initialize(context, m_position, Resources::GetInstance()->GetJson(L"AirTarget.json")["ColliderSize"]);

	// モデル
	m_model = Resources::GetInstance()->GetModel(L"ster.sdkmesh");
	m_model->UpdateEffects(
		[&](DirectX::IEffect* pEffect)
		{
			// BasicEffectにキャストする
			DirectX::BasicEffect* pBasicEffect = dynamic_cast<DirectX::BasicEffect*>(pEffect);
			pBasicEffect->SetAmbientLightColor(DirectX::SimpleMath::Vector4(1, 1, 1, 1));
		}
	);

	// 「浮いている」状態の生成
	m_floating = std::make_unique<Floating>(this);
	// 「浮いている」状態の初期化
	m_floating->Initialize();
	// 「当たった」状態の生成
	m_hitting = std::make_unique<Hitting>(this);
	// 「当たった」状態の初期化
	m_hitting->Initialize();

	// 浮いている状態にする
	m_currentState = m_floating.get();

	// 影の初期化
	InitializeShadow(device, context);

	//  パーティクル用オブジェクトの作成
	m_particle = std::make_unique<Particle>();
	//  初期化
	m_particle->Create(device, context, L"Ster.png");
}



/// <summary>
/// 更新処理
/// </summary>
/// <param name="elapsedTime">経過時間</param> 
void AirTarget::Update(float elapsedTime)
{
	m_currentState->Update(elapsedTime);

	// パーティクルの更新
	m_particle->Update(elapsedTime);
	// カメラの取得
	Camera* camera = dynamic_cast<Camera*>(Messenger::GetInstance()->GetObject(Factory::CAMERA));
	m_particle->CreateBillboard(m_position, camera->GetEyePosition(), DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitY, m_rotate));

	//m_particle->HandleFieldCollision(*m_pField);
	
	// 音の設定
	Resources::GetInstance()->Set3DSound(m_se.get(), m_position);
}



/// <summary>
/// 描画処理
/// </summary>
void AirTarget::Render()
{
	auto context = m_pUserResources->GetDeviceResources()->GetD3DDeviceContext();
	//auto states = m_pUserResources->GetCommonStates();
	auto view = m_pUserResources->GetView();
	auto proj = m_pUserResources->GetProject();

	// 座標の上限で描画する
	if (m_position.y <= 20.0f)
	{
		m_currentState->Render();
	}

	// パーティクルの描画
	m_particle->Render(context, *view, *proj);

	// デバック
	//m_collider.Draw(states, *view, *proj);

	/*m_particle->ColliderDraw(states, *view, *proj);*/

	/*auto debugFont = m_pUserResources->GetDebugFont();
	debugFont->Render(L"index", std::any(m_debugIndex));*/
}



/// <summary>
/// 終了処理
/// </summary>
void AirTarget::Finalize()
{
	m_currentState->Finalize();
}



/// <summary>
/// 重なりの補填
/// </summary>
/// <param name="field">座標</param>
void AirTarget::CorrectOverlap(DirectX::SimpleMath::Vector3& pos)
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



/// <summary>
/// メッセージの取得
/// </summary>
/// <param name="messageID">メッセージID</param>
void AirTarget::OnMessegeAccepted(Message::MessageID messageID)
{
	UNREFERENCED_PARAMETER(messageID);
}



/// <summary>
/// ステートの変更
/// </summary>
/// <param name="newState">新しいステート</param>
void AirTarget::ChangeState(IState* newState)
{
	// シーンで音を途切れないようにするため
	if (m_currentState == m_floating.get())
	{
		m_se = Resources::GetInstance()->GetSESound(L"GetSter.wav", m_position, false);
	}

	m_currentState = newState;
	m_currentState->Initialize();
}



/// <summary>
/// 影の初期化
/// </summary>
/// <param name="device">デバイス</param>
/// <param name="context">コンテキスト</param>
void AirTarget::InitializeShadow(ID3D11Device* device, ID3D11DeviceContext* context)
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
	m_shadowTexture = Resources::GetInstance()->GetTexture(L"Shadow.png");
}



/// <summary>
/// 影の描画
/// </summary>
/// <param name="context">コンテキスト</param>
/// <param name="states">コモンステート</param>
/// <param name="radius">半径</param>
void AirTarget::DrawShadow(ID3D11DeviceContext* context, DirectX::CommonStates* states, float radius)
{
	auto view = m_pUserResources->GetView();
	auto proj = m_pUserResources->GetProject();

	// エフェクトの設定＆適用
	m_basicEffect->SetWorld(DirectX::SimpleMath::Matrix::Identity);
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

	DirectX::VertexPositionTexture vertexes[] =
	{
		DirectX::VertexPositionTexture(DirectX::SimpleMath::Vector3::Zero, DirectX::SimpleMath::Vector2(0.0f, 0.0f)),  // 0
		DirectX::VertexPositionTexture(DirectX::SimpleMath::Vector3::Zero, DirectX::SimpleMath::Vector2(1.0f, 0.0f)),  // 1
		DirectX::VertexPositionTexture(DirectX::SimpleMath::Vector3::Zero, DirectX::SimpleMath::Vector2(0.0f, 1.0f)),  // 2
		DirectX::VertexPositionTexture(DirectX::SimpleMath::Vector3::Zero, DirectX::SimpleMath::Vector2(1.0f, 1.0f))   // 3
	};

	uint16_t indexes[] = { 2,3,1,2,1,0 };

	vertexes[0].position = DirectX::SimpleMath::Vector3(-radius, SHADOW_OFFSET, -radius);
	vertexes[1].position = DirectX::SimpleMath::Vector3(radius, SHADOW_OFFSET, -radius);
	vertexes[2].position = DirectX::SimpleMath::Vector3(-radius, SHADOW_OFFSET, radius);
	vertexes[3].position = DirectX::SimpleMath::Vector3(radius, SHADOW_OFFSET, radius);

	for (int i = 0; i < 4; ++i)
	{
		DirectX::SimpleMath::Vector3 rotatedOffset = DirectX::SimpleMath::Vector3::Transform(vertexes[i].position, m_rotate);
		vertexes[i].position = rotatedOffset + m_shadowHitPos;
	}

	// 影の描画
	m_primitiveBatch->Begin();

	m_primitiveBatch->DrawIndexed(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, indexes, _countof(indexes), vertexes, _countof(vertexes));

	m_primitiveBatch->End();
}



/// <summary>
/// ランダムに座標を設定
/// </summary>
void AirTarget::RandomPosition()
{
	// フィールドの取得
	Field* field = dynamic_cast<Field*>(Messenger::GetInstance()->GetObject(Factory::FIELD));

	// 番号の宣言
	int index = -1;
	// 三角形のために3で割れる数にする
	while ((index + 3) % 3 != 0)
	{
		// ステージの三角形の数でランダムに番号を決める
		std::uniform_int_distribution<int> dist(0, (int)field->GetFieldCollider().GetIndicesCount() - 1);
		std::mt19937 mt(m_rd());

		index = dist(mt);
	}

	// 三角形の中心を取得
	DirectX::SimpleMath::Vector3 center = field->GetFieldCollider().GetCenterPosition(index);

	// 空中の的の設定
	m_velocity = DirectX::SimpleMath::Vector3::Zero;
	m_position = center;
	m_shadowHitPos = center;
	m_gravity = field->CorrectUp(this, field->GetFieldCollider().GetNormalVector(index));


	// 元の座標からY軸方向に高くして置く
	m_position = m_position + DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitY, m_rotate) * OFFSET;
}