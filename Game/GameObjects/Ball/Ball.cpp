/// <summary>
/// Ballに関するソースファイル
/// </summary>
/// <author>仲森智史</author>

// ヘッダファイルの読み込み
#include "pch.h"
#include "Ball.h"

#include "Common/DebugDraw.h"
#include "Game/Commons/Resources.h"
#include "Game/Commons/Factory.h"
#include "Game/Commons/GameObjectMessenger.h"
#include "Game/GameObjects/Camera/Camera.h"




/// <summary>
/// コンストラクタ
/// </summary>
Ball::Ball(int objectID)
	: m_currentState{}
	, m_ballColorNum(0)
	, m_pUserResources(nullptr)
	, m_shadowHitPos{}
	, m_soundSpan(0.0f)
	, m_isSound(true)
	, m_invincibleTime(0.0f)
{
	// オブジェクト番号とオブジェクトを登録する
	GameObjectMessenger::GetInstance()->Register(objectID, this);
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
	m_pUserResources = UserResources::GetUserResource();
	auto device = m_pUserResources->GetDeviceResources()->GetD3DDevice();
	auto context = m_pUserResources->GetDeviceResources()->GetD3DDeviceContext();

	// 座標の初期化
	m_position = position;

	// コライダーの初期化
	m_collider.Initialize(context, m_position, Resources::GetInstance()->GetJson(L"Ball.json")["ColliderSize"]);

	// ボールのモデルをロードする
	auto effectFactory = m_pUserResources->GetEffectFactory();
	effectFactory->SetSharing(false);
	effectFactory->SetDirectory(L"Resources/Models");
	m_model = DirectX::Model::CreateFromSDKMESH(device, L"Resources/Models/Ball.sdkmesh", *effectFactory);
	m_model->UpdateEffects(
		[&](DirectX::IEffect* pEffect)
		{
			// BasicEffectにキャストする
			DirectX::BasicEffect* pBasicEffect = dynamic_cast<DirectX::BasicEffect*>(pEffect);

			pBasicEffect->SetAmbientLightColor(DirectX::SimpleMath::Vector4(1, 1, 1, 1));
		}
	);

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

	// ボールの色の番号の初期化
	m_ballColorNum = 0;

	// パーティクル用オブジェクトの作成
	m_particle = std::make_unique<Particle>();
	// パーティクルの初期化
	m_particle->Create(device, context, L"Line.png");

	// 音の間隔の初期化
	m_soundSpan = 0.0f;

	// 無敵時間の初期化
	m_invincibleTime = 0.0f;

	// 音をつける
	m_isSound = true;

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

	// パーティクルの更新
	m_particle->Update(elapsedTime);
	// カメラの取得
	Camera* camera = dynamic_cast<Camera*>(GameObjectMessenger::GetInstance()->GetObject(Factory::CAMERA));
	m_particle->CreateBillboard(m_position, camera->GetEyePosition(), DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitY, m_rotate));

	// 3Dサウンドの設定
	Resources::GetInstance()->Set3DSound(m_se.get(), m_position);

	// サウンド間隔の更新
	m_soundSpan += 1.0f * elapsedTime;
}



/// <summary>
/// 描画処理
/// </summary>
void Ball::Render()
{
	auto context = m_pUserResources->GetDeviceResources()->GetD3DDeviceContext();
	//auto states = m_pUserResources->GetCommonStates();
	auto view = m_pUserResources->GetView();
	auto proj = m_pUserResources->GetProject();

	m_currentState->Render();

	// パーティクルの描画
	m_particle->Render(context, *view, *proj);

	// デバック
	//m_collider.Draw(states, *view, *proj);

	//auto debagFont = m_pUserResources->GetDebugFont();
	//debagFont->Render(L"SoundSpan", m_soundSpan);
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
/// <param name="field">座標</param>
void Ball::CorrectOverlap(DirectX::SimpleMath::Vector3& pos)
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

	// 法線ベクトル
	DirectX::SimpleMath::Vector3 normalVec = m_gravity * -1.0f;
	normalVec.Normalize();

	// 反射ベクトル
	DirectX::SimpleMath::Vector3 reflVec = m_velocity - 2.0f * (m_velocity.Dot(normalVec)) * normalVec;

	// 摩擦
	reflVec *= (float)Resources::GetInstance()->GetJson(L"Ball.json")["DecelerationRate"];

	// 音をつけていないかつ音の間隔が一定時間たったら音を入れる
	if (!m_isSound && m_soundSpan >= (float)Resources::GetInstance()->GetJson(L"Ball.json")["SoundSpan"])
	{
		m_se = Resources::GetInstance()->GetSESound(L"BallBound.wav", m_position, false);
		m_isSound = true;
	}

	// 音の間隔の設定
	m_soundSpan = 0.0f;

	// 速度の設定
	m_velocity = reflVec;
}



/// <summary>
/// メッセージの取得
/// </summary>
/// <param name="messageID">メッセージID</param>
void Ball::OnMessegeAccepted(Message::MessageID messageID)
{
	UNREFERENCED_PARAMETER(messageID);
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
void Ball::DrawShadow(ID3D11DeviceContext* context, DirectX::CommonStates* states, float radius)
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

	vertexes[0].position = DirectX::SimpleMath::Vector3(-radius, 0.03f, -radius);
	vertexes[1].position = DirectX::SimpleMath::Vector3(radius, 0.03f, -radius);
	vertexes[2].position = DirectX::SimpleMath::Vector3(-radius, 0.03f, radius);
	vertexes[3].position = DirectX::SimpleMath::Vector3(radius, 0.03f, radius);

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
/// ボールの色の設定
/// </summary>
/// <param name="ballColor">ボールの色</param>
void Ball::SetBallColorNum(int ballColorNum)
{
	m_ballColorNum = ballColorNum;

	// 色を変更する
	m_model->UpdateEffects(
		[&](DirectX::IEffect* pEffect)
		{
			DirectX::BasicEffect* pBasicEffect = dynamic_cast<DirectX::BasicEffect*>(pEffect);
			pBasicEffect->SetColorAndAlpha(DirectX::SimpleMath::Vector4(
				Resources::GetInstance()->GetJson(L"Ball.json")["BallColor"][std::to_string(m_ballColorNum)]["x"],
				Resources::GetInstance()->GetJson(L"Ball.json")["BallColor"][std::to_string(m_ballColorNum)]["y"],
				Resources::GetInstance()->GetJson(L"Ball.json")["BallColor"][std::to_string(m_ballColorNum)]["z"],
				Resources::GetInstance()->GetJson(L"Ball.json")["BallColor"][std::to_string(m_ballColorNum)]["w"]
			));
		}
	);
}