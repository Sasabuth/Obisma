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
#include "Game/Scenes/TitleScene.h"


// 名前の省略
using namespace DirectX;


/// <summary>
/// コンストラクタ
/// </summary>
Player::Player(GameplayScene* pScene, BallManager* ballManager)
	: m_pScene(pScene)
	, m_userResources(nullptr)
	, m_ballManager(ballManager)
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
void Player::Initialize(DirectX::SimpleMath::Vector3 position)
{
	m_userResources = UserResources::GetUserResource();
	auto device = m_userResources->GetDeviceResources()->GetD3DDevice();
	auto context = m_userResources->GetDeviceResources()->GetD3DDeviceContext();

	m_position = position;

	m_collider.Initialize(context, m_position, 0.5f);

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

	// 立つ状態にする
	m_currentState = m_standing.get();

	// ボールを両手に持つための箱を用意する
	m_isBall.insert(std::make_pair(RIGHT, nullptr));
	m_isBall.insert(std::make_pair(LEFT, nullptr));

	// 影の初期化
	InitializeShadow(device, context);
}



/// <summary>
/// 更新処理
/// </summary>
/// <param name="elapsedTime">経過時間</param> 
void Player::Update(float elapsedTime)
{
	/*for (int i = 0; i < m_ballManager->GetObjectCount(); i++)
	{
		Ball* ball = m_ballManager->GetBall(i);

		if (ball->GetCurrentState() == ball->GetMoving())
		{
			if (IsHit(m_collider, ball->GetCollider()))
			{
				m_pScene->ChangeScene<TitleScene>();
			}
		}
	}*/
	

	m_currentState->Update(elapsedTime);
}



/// <summary>
/// 描画処理
/// </summary>
void Player::Render()
{
	m_currentState->Render();

	// デバック用
	auto* debugFont = m_userResources->GetDebugFont();

	auto states = m_userResources->GetCommonStates();
	auto view = m_userResources->GetView();
	auto proj = m_userResources->GetProject();
	//m_collider.Draw(states, *view, *proj);
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
	SimpleMath::Vector3 delta = m_position - field.GetCollider().GetPosition();

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



/// <summary>
/// マウスの方向に回転
/// </summary>
void Player::RotateToMouse()
{
	// 方向
	SimpleMath::Vector3 dir = m_position - m_hitPos;
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
}



/// <summary>
/// 影の初期化
/// </summary>
/// <param name="device">デバイス</param>
/// <param name="context">コンテキスト</param>
void Player::InitializeShadow(ID3D11Device* device, ID3D11DeviceContext* context)
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
void Player::DrawShadow(ID3D11DeviceContext* context, DirectX::CommonStates* states, float radius, DirectX::SimpleMath::Vector3& hitPos)
{
	auto view = m_userResources->GetView();
	auto proj = m_userResources->GetProject();

	// エフェクトの設定＆適用
	m_basicEffect->SetWorld(SimpleMath::Matrix::Identity);
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
	SimpleMath::Ray ray{ m_position, m_gravity };

	// レイが当たった座標に影を出す
	if (CalcRaySphere(ray.position, ray.direction, m_pScene->GetField().GetCollider().GetPosition(), m_pScene->GetField().GetCollider().GetRadius(), hitPos))
	{
		for (int i = 0; i < 4; ++i)
		{
			SimpleMath::Vector3 rotatedOffset = SimpleMath::Vector3::Transform(vertexes[i].position, m_rotate);
			vertexes[i].position = rotatedOffset + hitPos;
		}
	}

	// 影の描画
	m_primitiveBatch->Begin();
	m_primitiveBatch->DrawIndexed(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, indexes, _countof(indexes), vertexes, _countof(vertexes));
	m_primitiveBatch->End();
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



