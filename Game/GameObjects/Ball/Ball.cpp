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
	auto context = UserResources::GetUserResource()->GetDeviceResources()->GetD3DDeviceContext();

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