/// <summary>
/// Enemyに関するソースファイル
/// </summary>
/// <author>仲森智史</author>

// ヘッダファイルの読み込み
#include "pch.h"
#include "Enemy.h"

#include "DebugDraw.h"
#include "Game/Commons/Resources.h"
#include "Game/Commons/Factory.h"



/// <summary>
/// コンストラクタ
/// </summary>
Enemy::Enemy(Player* pPlayer, Field* pField, AirTarget* pAirTarget, BallManager* pBallManager)
	: m_pPlayer(pPlayer)
	, m_pField(pField)
	, m_pAirTarget(pAirTarget)
	, m_pUserResources(nullptr)
	, m_ballManager(pBallManager)
	, m_currentState{}
	, m_ballIndex(0)
	, m_invincibleTime(0.0f)
	, m_target(nullptr)
{
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
	m_pUserResources = UserResources::GetUserResource();
	auto device = m_pUserResources->GetDeviceResources()->GetD3DDevice();
	auto context = m_pUserResources->GetDeviceResources()->GetD3DDeviceContext();

	m_position = position;

	m_worldMatrix = DirectX::SimpleMath::Matrix::Identity;

	m_collider.Initialize(context, m_position, Resources::GetInstance()->GetJson(L"Enemy.json")["ColliderSize"]);
	m_catchCollider.Initialize(context, m_position, (float)Resources::GetInstance()->GetJson(L"Enemy.json")["ColliderSize"] - 0.1f);

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

	m_invincibleTime = 0.0f;

	m_target = nullptr;

	m_score = Factory::CreateScore(Ball::ENEMY);

	InitializeShadow(device, context);
}



/// <summary>
/// 更新処理
/// </summary>
/// <param name="elapsedTime">経過時間</param> 
void Enemy::Update(float elapsedTime)
{
	m_currentState->Update(elapsedTime);

	Resources::GetInstance()->Set3DSound(m_se.get(), m_position);
	m_invincibleTime -= elapsedTime;
}



/// <summary>
/// 描画処理
/// </summary>
void Enemy::Render()
{
	m_currentState->Render();

	//auto states = m_pUserResources->GetCommonStates();
	//auto view = m_pUserResources->GetView();
	//auto proj = m_pUserResources->GetProject();

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
/// <param name="field">フィールド</param>
void Enemy::CorrectOverlap(Field& field)
{
	// 差分を求める
	DirectX::SimpleMath::Vector3 delta = m_position - field.GetCollider().GetPosition();

	// 長さを求める
	float distance = delta.Length();
	float minDistance = m_collider.GetRadius() + field.GetCollider().GetRadius();

	// 差分を求める
	float pushLength = minDistance - distance;

	// 正規化
	delta.Normalize();

	// 押し出しする
	m_position += delta * pushLength;
}

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
/// ステートの変更
/// </summary>
/// <param name="newState">新しいステート</param>
void Enemy::ChangeState(IState* newState)
{
	m_currentState = newState;
	m_currentState->Initialize();
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
bool Enemy::CalcRaySphere(DirectX::SimpleMath::Vector3 rayPos, DirectX::SimpleMath::Vector3 rayDir, DirectX::SimpleMath::Vector3 spherePos, float radius, DirectX::SimpleMath::Vector3& hitPos)
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

	vertexes[0].position = DirectX::SimpleMath::Vector3(-radius, 0.01f, -radius);
	vertexes[1].position = DirectX::SimpleMath::Vector3(radius, 0.01f, -radius);
	vertexes[2].position = DirectX::SimpleMath::Vector3(-radius, 0.01f, radius);
	vertexes[3].position = DirectX::SimpleMath::Vector3(radius, 0.01f, radius);

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
	for (int i = 0; i < m_ballManager->GetObjectCount(); i++)
	{
		Ball* ball = m_ballManager->GetBall(i);

		if (ball->GetCurrentState() == ball->GetMoving() && ball->GetBallColorNum() != Ball::BallColor::ENEMY)
		{
			if (IsHit(m_collider, ball->GetCollider()) && m_invincibleTime <= 0.0f)
			{
				m_currentState = m_dizzying.get();
				m_score->ScoreDown();

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
	m_isBall[key] = ball;
}



/// <summary>
/// ボールの取得
/// </summary>
/// <param name="key">キー</param>
/// <returns>ボールのポインタ</returns>
Ball* Enemy::GetCatchBall(int key) const
{
	Ball* ball = m_isBall.at(key);

	if (ball)
	{
		return ball;
	}

	return nullptr;
}
