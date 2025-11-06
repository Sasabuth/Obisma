/// <summary>
/// Playerに関するソースファイル
/// </summary>
/// <author>仲森智史</author>

// ヘッダファイルの読み込み
#include "pch.h"
#include "Player.h"

#include "Game/Scenes/GameplayScene.h"
#include "Game/GameObjects//Camera/Camera.h"
#include "Game/GameObjects/Field/Field.h"
#include "Game/GameObjects/Ball/Ball.h"
#include "DebugDraw.h"
#include "Game/Commons/Resources.h"
#include "Game/Commons/Factory.h"



/// <summary>
/// コンストラクタ
/// </summary>
Player::Player(Field* pField, AirTarget* pAirTarget, BallManager* pBallManager)
	: m_pField(pField)
	, m_pAirTarget(pAirTarget)
	, m_pUserResources(nullptr)
	, m_pBallManager(pBallManager)
	, m_currentState{}
	, m_invincibleTime(0.0f)
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
void Player::Initialize(DirectX::SimpleMath::Vector3 position)
{
	m_pUserResources = UserResources::GetUserResource();
	auto device = m_pUserResources->GetDeviceResources()->GetD3DDevice();
	auto context = m_pUserResources->GetDeviceResources()->GetD3DDeviceContext();

	m_position = position;

	m_worldMatrix = DirectX::SimpleMath::Matrix::Identity;

	m_collider.Initialize(context, m_position, Resources::GetInstance()->GetJson(L"Player.json")["ColliderSize"]);

	// 「立つ」状態の生成
	m_standing = std::make_unique<Standing>(this);
	// 「立つ」状態の初期化
	m_standing->Initialize();
	// 「走る」状態の生成
	m_running = std::make_unique<Running>(this);
	// 「走る」状態の初期化
	m_running->Initialize();
	// 「右で投げる」状態の生成
	m_throwingR = std::make_unique<ThrowingR>(this);
	// 「右で投げる」状態の初期化
	m_throwingR->Initialize();
	// 「左で投げる」状態の生成
	m_throwingL = std::make_unique<ThrowingL>(this);
	// 「左で投げる」状態の初期化
	m_throwingL->Initialize();
	// 「キャッチ」状態の生成
	m_catching = std::make_unique<PlayerCatching>(this);
	// 「キャッチ」状態の初期化
	m_catching->Initialize();
	// 「くらくら」状態の生成
	m_dizzying = std::make_unique<Dizzying>(this);
	// 「くらくら」状態の初期化
	m_dizzying->Initialize();

	// 立つ状態にする
	m_currentState = m_standing.get();

	// ボールを両手に持つための箱を用意する
	m_isBall.insert(std::make_pair(RIGHT, nullptr));
	m_isBall.insert(std::make_pair(LEFT, nullptr));

	// 無敵時間の初期化
	m_invincibleTime = 0.0f;

	// スコアの初期化
	m_score = Factory::CreateScore(Ball::PLAYER);

	// 影の初期化
	InitializeShadow(device, context);

	// ロックオンテクスチャの初期化
	m_lockOnTexture.SetTexture(nullptr);
}



/// <summary>
/// 更新処理
/// </summary>
/// <param name="elapsedTime">経過時間</param> 
void Player::Update(float elapsedTime)
{	
	m_currentState->Update(elapsedTime);

	m_invincibleTime -= elapsedTime;
}



/// <summary>
/// 描画処理
/// </summary>
void Player::Render()
{
	m_currentState->Render();

	DirectX::SimpleMath::Vector3 hitPos1;
	DirectX::SimpleMath::Vector3 hitPos2;

	// マウスの方向に回転
	if (CalcRaySphere(m_pAirTarget->GetPosition(), m_pAirTarget->GetCollider().GetRadius(), hitPos1) &&
		CalcRaySphere(m_pField->GetCollider().GetPosition(), m_pField->GetCollider().GetRadius(), hitPos2))
	{
		hitPos1 = m_mouseRay.position - hitPos1;
		hitPos2 = m_mouseRay.position - hitPos2;

		if (hitPos1.Length() < hitPos2.Length())
		{
			DrawLockOn(m_pAirTarget->GetPosition());
		}
	}
	else
	{
		// ロックオンの描画
		if (CalcRaySphere(m_pAirTarget->GetPosition(), m_pAirTarget->GetCollider().GetRadius(), m_hitPos))
		{
			DrawLockOn(m_pAirTarget->GetPosition());
		}
	}

	// デバック用
	/*auto* debugFont = m_pUserResources->GetDebugFont();
	debugFont->Render(L"pos", 1);
	debugFont->Render(L"pos", std::any(m_velocity));*/

	/*auto states = m_userResources->GetCommonStates();
	auto view = m_userResources->GetView();
	auto proj = m_userResources->GetProject();
	m_collider.Draw(states, *view, *proj);*/
}



/// <summary>
/// 終了処理
/// </summary>
void Player::Finalize()
{
	m_currentState->Finalize();
}



/// <summary>
/// 重なりの補填
/// </summary>
/// <param name="field">フィールド</param>
void Player::CorrectOverlap(Field& field)
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
/// ステートの変更
/// </summary>
/// <param name="newState">新しいステート</param>
void Player::ChangeState(IState* newState)
{
	m_currentState = newState;
	m_currentState->Initialize();
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
	// マウスの座標(NDC座標)
	float px = (2.0f * mouseX / screenWidth - 1.0f);
	float py = (1.0f - 2.0f * mouseY / screenHeight);

	// 2点を作って距離を計算
	DirectX::SimpleMath::Vector3 nearPoint = DirectX::SimpleMath::Vector3(px, py, 0.0f);
	DirectX::SimpleMath::Vector3 farPoint = DirectX::SimpleMath::Vector3(px, py, 1.0f);

	// ワールド座標に変換
	DirectX::SimpleMath::Matrix viewProj = view * proj;
	DirectX::SimpleMath::Matrix invViewProj;
	viewProj.Invert(invViewProj);

	// レイの座標とベクトルを求める
	DirectX::SimpleMath::Vector3 rayOrigin = DirectX::SimpleMath::Vector3::Transform(nearPoint, invViewProj);
	DirectX::SimpleMath::Vector3 rayTarget = DirectX::SimpleMath::Vector3::Transform(farPoint, invViewProj);
	DirectX::SimpleMath::Vector3 rayDir = rayTarget - rayOrigin;
	rayDir.Normalize();

	return DirectX::SimpleMath::Ray(rayOrigin, rayDir);
}



/// <summary>
/// レイと球体の交差
/// </summary>
/// <param name="spherePos">球の座標</param>
/// <param name="radius">半径</param>
/// <param name="hitPos">当たった座標</param>
/// <returns>[true] 当たった　[false] 当たってない</returns>
bool Player::CalcRaySphere(DirectX::SimpleMath::Vector3 spherePos, float radius, DirectX::SimpleMath::Vector3& hitPos)
{
	spherePos.x = spherePos.x - m_mouseRay.position.x;
	spherePos.y = spherePos.y - m_mouseRay.position.y;
	spherePos.z = spherePos.z - m_mouseRay.position.z;

	float A = m_mouseRay.direction.x * m_mouseRay.direction.x + m_mouseRay.direction.y * m_mouseRay.direction.y + m_mouseRay.direction.z * m_mouseRay.direction.z;
	float B = m_mouseRay.direction.x * spherePos.x + m_mouseRay.direction.y * spherePos.y + m_mouseRay.direction.z * spherePos.z;
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
	hitPos.x = m_mouseRay.position.x + a1 * m_mouseRay.direction.x;
	hitPos.y = m_mouseRay.position.y + a1 * m_mouseRay.direction.y;
	hitPos.z = m_mouseRay.position.z + a1 * m_mouseRay.direction.z;

	return true;
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



/// <summary>
/// マウスの方向に回転
/// </summary>
void Player::RotateToMouse()
{
	// 方向
	DirectX::SimpleMath::Vector3 dir = m_position - m_hitPos;
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
}



/// <summary>
/// ボールの座標の設定
/// </summary>
/// <param name="ball">ボールのポインタ</param>
/// <param name="handMatrix">手のマトリックス</param>
void Player::SetBallPosition(Ball* ball, DirectX::SimpleMath::Matrix handMatrix)
{
	// ボーンに設定した境界球のワールド計算を行う
	DirectX::SimpleMath::Matrix sphereMatrix = handMatrix * m_worldMatrix;
	// バウンディングスフィアの中心点を設定する
	DirectX::SimpleMath::Vector3 dir = DirectX::SimpleMath::Vector3(sphereMatrix._41, sphereMatrix._42, sphereMatrix._43);
	dir.Normalize();
	ball->SetPosition(DirectX::SimpleMath::Vector3(dir.x * 3.2f, dir.y * 3.2f, dir.z * 3.2f));
	ball->SetInvincibleTime(m_invincibleTime);
}



/// <summary>
/// 影の初期化
/// </summary>
/// <param name="device">デバイス</param>
/// <param name="context">コンテキスト</param>
void Player::InitializeShadow(ID3D11Device* device, ID3D11DeviceContext* context)
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
void Player::DrawShadow(ID3D11DeviceContext* context, DirectX::CommonStates* states, float radius, DirectX::SimpleMath::Vector3& hitPos)
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

	// レイ
	DirectX::SimpleMath::Ray ray{ m_position, m_gravity };

	// レイが当たった座標に影を出す
	if (CalcRaySphere(ray.position, ray.direction, m_pField->GetCollider().GetPosition(), m_pField->GetCollider().GetRadius(), hitPos))
	{
		for (int i = 0; i < 4; ++i)
		{
			DirectX::SimpleMath::Vector3 rotatedOffset = DirectX::SimpleMath::Vector3::Transform(vertexes[i].position, m_rotate);
			vertexes[i].position = rotatedOffset + hitPos;
		}
	}

	// 影の描画
	m_primitiveBatch->Begin();
	m_primitiveBatch->DrawIndexed(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, indexes, _countof(indexes), vertexes, _countof(vertexes));
	m_primitiveBatch->End();
}



/// <summary>
/// ロックオンの描画
/// </summary>
/// <param name="pos">座標</param>
void Player::DrawLockOn(const DirectX::SimpleMath::Vector3& pos)
{	
	auto view = m_pUserResources->GetView();
	auto proj = m_pUserResources->GetProject();

	// ビュー射影行列
	DirectX::SimpleMath::Matrix viewProj = *view * *proj;

	// ワールド座標
	DirectX::SimpleMath::Vector4 pos4(pos.x, pos.y, pos.z, 1.0f);

	// クリップ座標に変換
	DirectX::SimpleMath::Vector4 clipPos = DirectX::SimpleMath::Vector4::Transform(pos4, viewProj);
	clipPos /= clipPos.w;

	// スクリーン座標に変換
	float screenX = (clipPos.x * 0.5f + 0.5f) * BASE_WIDTH;
	float screenY = (1.0f - (clipPos.y * 0.5f + 0.5f)) * BASE_HEIGHT;
	DirectX::SimpleMath::Vector2 screenPos(screenX, screenY);

	// 距離の計算
	DirectX::SimpleMath::Vector3 dir = m_position - pos;

	// ボールが当たる距離に応じてロックオンの色を変える
	if (IsInHitRange())
	{
		// 赤を描画(当たる)
		m_lockOnTexture.SetTexture(Resources::GetInstance()->GetTexture(L"LockOnR.png"));
		m_lockOnTexture.Draw(screenPos, LOCKON.size, LOCKON.scale);
	}
	else if (IsInHitRange(1.0f))
	{
		// 黄を描画(当たらない)
		m_lockOnTexture.SetTexture(Resources::GetInstance()->GetTexture(L"LockOnY.png"));
		m_lockOnTexture.Draw(screenPos, LOCKON.size, LOCKON.scale);
	}
	else
	{
		// 緑を描画(絶対当たらない)
		m_lockOnTexture.SetTexture(Resources::GetInstance()->GetTexture(L"LockOnG.png"));
		m_lockOnTexture.Draw(screenPos, LOCKON.size, LOCKON.scale);
	}
	
}



/// <summary>
/// スコアを下げる
/// </summary>
void Player::ScoreDown()
{
	for (int i = 0; i < m_pBallManager->GetObjectCount(); i++)
	{
		Ball* ball = m_pBallManager->GetBall(i);

		if (ball->GetCurrentState() == ball->GetMoving() && ball->GetBallColorNum() != Ball::BallColor::PLAYER)
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
/// 当たる距離か
/// </summary>
/// <returns>当たる距離か</returns>
bool Player::IsInHitRange(float offset)
{
	// 距離の計算
	DirectX::SimpleMath::Vector3 dir = m_position - m_pAirTarget->GetPosition();

	// 成分の合計距離の計算
	float airTargtPos = m_pAirTarget->GetPosition().x * m_pAirTarget->GetPosition().y * m_pAirTarget->GetPosition().z;

	// 距離が当たる距離に入っているか
	if (dir.Length() <= offset + (float)Resources::GetInstance()->GetJson(L"Player.json")["LockOn"] - (std::fabs(airTargtPos) - MIN_AIRPOS) * LOCKON_HEIGHT_RATE)
	{
		return true;
	}

	return false;
}



/// <summary>
/// ボールの設定
/// </summary>
/// <param name="key">キー</param>
/// <param name="ball">ボールのポインタ</param>
void Player::SetCatchBall(int key, Ball* ball)
{
	m_isBall[key] = ball;
}



/// <summary>
/// ボールの取得
/// </summary>
/// <param name="key">キー</param>
/// <returns>ボールのポインタ</returns>
Ball* Player::GetCatchBall(int key) const
{
	Ball* ball = m_isBall.at(key);

	if (ball)
	{
		return ball;
	}

	return nullptr;
}



