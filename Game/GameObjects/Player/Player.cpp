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
Player::Player(GameplayScene* pScene)
	: m_pScene(pScene)
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
void Player::Initialize(DirectX::SimpleMath::Vector3 position)
{
	auto context = UserResources::GetUserResource()->GetDeviceResources()->GetD3DDeviceContext();

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
	// 「投げる」状態の生成
	m_throwing = std::make_unique<Throwing>(this);
	// 「投げる」状態の初期化
	m_throwing->Initialize();

	// 立つ状態にする
	m_currentState = m_standing.get();
}



/// <summary>
/// 更新処理
/// </summary>
/// <param name="elapsedTime">経過時間</param> 
void Player::Update(float elapsedTime)
{
	m_currentState->Update(elapsedTime);
}



/// <summary>
/// 描画処理
/// </summary>
void Player::Render()
{
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
