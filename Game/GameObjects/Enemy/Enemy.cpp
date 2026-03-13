/// <summary>
/// Enemyに関するソースファイル
/// </summary>
/// <author>仲森智史</author>

// ヘッダファイルの読み込み
#include "pch.h"
#include "Enemy.h"

#include "Common/DebugDraw.h"
#include "Game/Commons/Resources.h"
#include "Game/Commons/Factory.h"
#include "Game/Commons/Messenger.h"
#include "Game/GameObjects/Field/Field.h"
#include "Game/GameObjects/Camera/Camera.h"



/// <summary>
/// コンストラクタ
/// </summary>
Enemy::Enemy()
	: m_pUserResources(nullptr)
	, m_currentState{}
	, m_ballIndex(0)
	, m_invincibleTime(0.0f)
	, m_target(nullptr)
{
	// モデル
	m_model = Resources::GetInstance()->GetModel(L"Enemy.sdkmesh");
	m_model->UpdateEffects(
		[&](DirectX::IEffect* pEffect)
		{
			// BasicEffectにキャストする
			auto pBasicEffect = dynamic_cast<DirectX::SkinnedEffect*> (pEffect);

			pBasicEffect->SetAmbientLightColor(DirectX::SimpleMath::Vector4(1, 1, 1, 0.5));
		}
	);

	// オブジェクト番号とオブジェクトを登録する
	Messenger::GetInstance()->Register(Factory::ENEMY, this);
}



/// <summary>
/// デストラクタ
/// </summary>
Enemy::~Enemy()
{
}


/// <summary>
/// 初期化処理
/// </summary>
void Enemy::Initialize(DirectX::SimpleMath::Vector3 position)
{
	// ユーザーリソースの取得
	m_pUserResources = UserResources::GetUserResource();

	auto device = m_pUserResources->GetDeviceResources()->GetD3DDevice();
	auto context = m_pUserResources->GetDeviceResources()->GetD3DDeviceContext();

	// 座標の初期化
	m_position = position;
	// ワールド座標の初期化
	m_worldMatrix = DirectX::SimpleMath::Matrix::Identity;

	// コライダーの初期化
	m_collider.Initialize(context, m_position, Resources::GetInstance()->GetJson(L"Enemy.json")["ColliderSize"]);
	m_catchCollider.Initialize(context, m_position, (float)Resources::GetInstance()->GetJson(L"Enemy.json")["ColliderSize"] - 0.1f);

	// ボール番号の初期化
	m_ballIndex = 0;

	// 「立つ」状態の生成
	m_standing = std::make_unique<EnemyStanding>(this);
	// 「立つ」状態の初期化
	m_standing->Initialize();
	// 「走る」状態の生成
	m_running = std::make_unique<EnemyRunning>(this);
	// 「走る」状態の初期化
	m_running->Initialize();
	// 「右手で投げる」状態の生成
	m_throwingR = std::make_unique<EnemyThrowingR>(this);
	// 「右手で投げる」状態の初期化
	m_throwingR->Initialize();
	// 「左手で投げる」状態の生成
	m_throwingL = std::make_unique<EnemyThrowingL>(this);
	// 「左手で投げる」状態の初期化
	m_throwingL->Initialize();
	// 「くらくら」状態の生成
	m_dizzying = std::make_unique<EnemyDizzying>(this);
	// 「くらくら」状態の初期化
	m_dizzying->Initialize();
	// 「とる」状態の生成
	m_catching = std::make_unique<EnemyCatching>(this);
	// 「とる」状態の初期化
	m_catching->Initialize();

	// 立つ状態にする
	m_currentState = m_standing.get();

	// ボールを両手に持つための箱を用意する
	m_isBall.insert(std::make_pair(RIGHT, nullptr));
	m_isBall.insert(std::make_pair(LEFT, nullptr));

	// 無敵時間の初期化
	m_invincibleTime = 0.0f;

	// パーティクル用オブジェクトの作成
	for (int i = 0; i < MAX_COUNT; i++)
	{
		m_particle[i] = std::make_unique<Particle>();
	}

	// 初期化
	m_particle[CIRCLE]->Create(device, context, L"Circle.png");
	m_particle[STER]->Create(device, context, L"Ster.png");

	// ターゲットの初期化
	m_target = nullptr;

	// スコアの初期化
	m_score = Factory::CreateScore(Ball::ENEMY);

	// 影の初期化
	InitializeShadow(device, context);
}



/// <summary>
/// 更新処理
/// </summary>
/// <param name="elapsedTime">経過時間</param> 
void Enemy::Update(float elapsedTime)
{
	m_currentState->Update(elapsedTime);

	// パーティクルの更新
	for (int i = 0; i < MAX_COUNT; i++)
	{
		m_particle[i]->Update(elapsedTime);
	}
	// カメラの取得
	Camera* camera = dynamic_cast<Camera*>(Messenger::GetInstance()->GetObject(Factory::CAMERA));
	m_particle[STER]->CreateBillboard(m_position, camera->GetEyePosition(), DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitY, m_rotate));

	// 音の更新
	Resources::GetInstance()->Set3DSound(m_se.get(), m_position);

	// 無敵時間の減少
	m_invincibleTime -= elapsedTime;
}



/// <summary>
/// 描画処理
/// </summary>
void Enemy::Render()
{
	auto context = m_pUserResources->GetDeviceResources()->GetD3DDeviceContext();
	//auto states = m_pUserResources->GetCommonStates();
	auto view = m_pUserResources->GetView();
	auto proj = m_pUserResources->GetProject();

	m_currentState->Render();

	// パーティクルの描画
	for (int i = 0; i < MAX_COUNT; i++)
	{
		m_particle[i]->Render(context, *view, *proj);
	}

	//m_collider.Draw(states, *view, *proj);
}



/// <summary>
/// 終了処理
/// </summary>
void Enemy::Finalize()
{
	m_currentState->Finalize();
}



/// <summary>
/// 重なりの補填
/// </summary>
/// <param name="iEntity">実体</param>
void Enemy::CorrectOverlap(IEntity& iEntity)
{
	// 差分を求める
	DirectX::SimpleMath::Vector3 delta = m_position - iEntity.GetCollider().GetPosition();

	// 長さを求める
	float distance = delta.Length();
	float minDistance = m_collider.GetRadius() + iEntity.GetCollider().GetRadius();

	// 差分を求める
	float pushLength = minDistance - distance;

	// 正規化
	delta.Normalize();

	// 押し出しする
	m_position += delta * pushLength;
}



/// <summary>
/// 重なりの補填
/// </summary>
/// <param name="pos">座標</param>
void Enemy::CorrectOverlap(DirectX::SimpleMath::Vector3& pos)
{
	// 衝突点とプレイヤーの差分
	DirectX::SimpleMath::Vector3 delta = m_position - pos;

	// 距離
	float distance = delta.Length();
	float r = m_collider.GetRadius();

	// めり込み量
	float pushLength = r - distance;

	// 押し出し方向
	delta.Normalize();
	m_position += delta * pushLength;
}



/// <summary>
/// メッセージの取得
/// </summary>
/// <param name="messageID">メッセージID</param>
void Enemy::OnMessegeAccepted(Message::MessageID messageID)
{
	UNREFERENCED_PARAMETER(messageID);
}



/// <summary>
/// ステートの変更
/// </summary>
/// <param name="newState">新しいステート</param>
void Enemy::ChangeState(IState* newState)
{
	m_currentState = newState;
	m_currentState->Initialize();
}



/// <summary>
/// ボール座標の設定
/// </summary>
/// <param name="ball">ボールのポインタ</param>
/// <param name="handMatrix">手のマトリックス</param>
void Enemy::SetBallPosition(Ball* ball, DirectX::SimpleMath::Matrix handMatrix)
{
	// ボーンに設定した境界球のワールド計算を行う
	DirectX::SimpleMath::Matrix sphereMatrix = handMatrix * m_worldMatrix;
	// バウンディングスフィアの中心点を設定する
	float handOffsetScale = (float)Resources::GetInstance()->GetJson(L"Enemy.json")["HandOffsetScale"];
	ball->SetPosition(DirectX::SimpleMath::Vector3(sphereMatrix._41 * handOffsetScale, sphereMatrix._42 * handOffsetScale, sphereMatrix._43 * handOffsetScale));

	// ボールが当たった時に一緒に透過させるために無敵時間を設定する
	ball->SetInvincibleTime(m_invincibleTime);
}



/// <summary>
/// 影の初期化
/// </summary>
/// <param name="device">デバイス</param>
/// <param name="context">コンテキスト</param>
void Enemy::InitializeShadow(ID3D11Device* device, ID3D11DeviceContext* context)
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
void Enemy::DrawShadow(ID3D11DeviceContext* context, DirectX::CommonStates* states, float radius)
{
	auto view = m_pUserResources->GetView();
	auto proj = m_pUserResources->GetProject();

	// エフェクトの設定＆適用
	m_basicEffect->SetWorld(DirectX::SimpleMath::Matrix::Identity);
	m_basicEffect->SetView(*view);
	m_basicEffect->SetProjection(*proj);
	m_basicEffect->SetTexture(m_shadowTexture.Get());
	m_basicEffect->Apply(context);

	// カリングの設定
	context->RSSetState(states->CullClockwise());

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
/// スコアを下げる
/// </summary>
void Enemy::ScoreDown()
{
	for (int i = 0; i < Resources::GetInstance()->GetJson(L"Ball.json")["BallCount"]; i++)
	{
		// ボールの取得
		Ball* ball = dynamic_cast<Ball*>(Messenger::GetInstance()->GetObject(Factory::BALL + i));

		// ボールが動いているかつ自分のボールではなかったら
		if (ball->GetCurrentState() == ball->GetMoving() && ball->GetBallColorNum() != Ball::BallColor::ENEMY)
		{
			// コライダーとボールが当たっているかつ無敵時間ではなかったら
			if (IsHit(m_collider, ball->GetCollider()) && m_invincibleTime <= 0.0f)
			{
				// くらくら状態に変更する
				ChangeState(m_dizzying.get());
				// スコアを下げる
				m_score->ScoreDown();
				// 音を出す
				m_se = Resources::GetInstance()->GetSESound(L"BallHit.wav", m_position, false);
			}
		}
	}
}



/// <summary>
/// ボールの設定
/// </summary>
/// <param name="key">キー</param>
/// <param name="ball">ボールのポインタ</param>
void Enemy::SetCatchBall(int key, Ball* ball)
{
	// ボールを持っているならテクスチャをつける
	if (ball)
	{
		m_score->SetBallTexture(key);
	}
	// ボールを持っていないならテクスチャを外す
	else
	{
		m_score->ClearBallTexture(key);
	}

	m_isBall[key] = ball;
}



/// <summary>
/// ボールの取得
/// </summary>
/// <param name="key">キー</param>
/// <returns>ボールのポインタ</returns>
Ball* Enemy::GetCatchBall(int key) const
{
	// ボールの取得
	Ball* ball = m_isBall.at(key);

	// ボールがあったらポインターを渡す
	if (ball)
	{
		return ball;
	}

	return nullptr;
}
