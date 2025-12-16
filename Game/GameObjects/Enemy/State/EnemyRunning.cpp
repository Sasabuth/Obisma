/// <summary>
/// EnemyRunningに関するソースファイル
/// </summary>
/// <author>仲森智史</author>

// ヘッダファイルの読み込み
#include "pch.h"
#include "EnemyRunning.h"

#include "Game/GameObjects/Enemy/Enemy.h"
#include "Game/GameObjects/Player/Player.h"
#include "Game/GameObjects/AirTarget/AirTarget.h"
#include "Game/GameObjects/Field/Field.h"
#include "Game/GameObjects/Ball/BallManager.h"
#include "DebugDraw.h"
#include "Game/Commons/Resources.h"



/// <summary>
/// コンストラクタ
/// </summary>
EnemyRunning::EnemyRunning(Enemy* pEnemy)
	: m_pEnemy(pEnemy)
	, m_pUserResources(nullptr)
	, m_model{}
{
	// モデルの作成
	m_model = Resources::GetInstance()->GetEnemyModel();

	// AnimationSDKMESH クラスのインスタンスを生成する
	m_animation = std::make_unique<DX::AnimationSDKMESH>();
	// サッカープレイヤー アイドリングアニメーションをロードする
	m_animation->Load(L"resources/Animations/Player_Run.sdkmesh_anim");
	// アニメーションとモデルをバインドする
	m_animation->Bind(*m_model);

	// ボーン用のトランスフォーム配列を生成する
	m_drawBones = DirectX::ModelBone::MakeArray(m_model->bones.size());
	ZeroMemory(m_drawBones.get(), sizeof(DirectX::ModelBone) * m_model->bones.size());

	// アニメーションの初期化
	AnimationUpdate(0.0f);
}



/// <summary>
/// デストラクタ
/// </summary>
EnemyRunning::~EnemyRunning()
{
}


/// <summary>
/// 初期化処理
/// </summary>
void EnemyRunning::Initialize()
{
	m_pUserResources = UserResources::GetUserResource();

	auto device = m_pUserResources->GetDeviceResources()->GetD3DDevice();
	auto context = m_pUserResources->GetDeviceResources()->GetD3DDeviceContext();

	// アイドリングアニメーションの開始時間を設定する
	m_animation->SetStartTime(0.0f);
	// アイドリングアニメーションの終了時間を設定する
	m_animation->SetEndTime(0.5f);

	// ベーシックエフェクトの作成
	m_basicEffect = std::make_unique<DirectX::BasicEffect>(device);
	m_basicEffect->SetVertexColorEnabled(true);

	// プリミティブバッチの作成
	m_primitiveBatch = std::make_unique<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>>(context);

	// 入力レイアウトの作成
	DirectX::CreateInputLayoutFromEffect<DirectX::VertexPositionColor>(device, m_basicEffect.get(), m_inputLayout.ReleaseAndGetAddressOf());
}



/// <summary>
/// 更新処理
/// </summary>
/// <param name="elapsedTime">経過時間</param> 
void EnemyRunning::Update(float elapsedTime)
{
	// 速度の設定
	m_pEnemy->SetVelocity(m_pEnemy->GetGravity());

	// ボールの方向に走る
	if (!m_pEnemy->GetCatchBall(Enemy::RIGHT) && !m_pEnemy->GetCatchBall(Enemy::LEFT))
	{
		RunToBall();
	}
	else
	{
		if (!m_pEnemy->GetTarget())  m_pEnemy->SetTarget(NearEntity());

		RunToEntity();
	}

	// ボールを投げる
	ThrowBall();

	// ボールを持つ
	CatchHandBall();

	// アニメーションの更新
	AnimationUpdate(elapsedTime);

	// スコアを下げる
	m_pEnemy->ScoreDown();

	// 敵の設定
	m_pEnemy->SetPosition(m_pEnemy->GetPosition() + m_pEnemy->GetVelocity() * elapsedTime);
	m_pEnemy->GetCollider().SetPosition(m_pEnemy->GetPosition());

	DirectX::SimpleMath::Vector3 catchPos = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitX, m_pEnemy->GetRotation()) / 2.5;

	m_pEnemy->GetCatchCollider().SetPosition(m_pEnemy->GetPosition() + catchPos);

	for (int i = 0; i < m_pEnemy->GetBallManager()->GetObjectCount(); i++)
	{
		Ball* ball = m_pEnemy->GetBallManager()->GetBall(i);
		if (IsHit(m_pEnemy->GetCatchCollider(), ball->GetCollider()))
		{
			if (ball->GetBallColorNum() != Ball::ENEMY && ball->GetCurrentState() == ball->GetMoving())
			{
				m_pEnemy->ChangeState(m_pEnemy->GetCatching());
			}
		}
	}
}



/// <summary>
/// 描画処理
/// </summary>
void EnemyRunning::Render()
{
	auto context = m_pUserResources->GetDeviceResources()->GetD3DDeviceContext();
	auto states = m_pUserResources->GetCommonStates();
	auto view = m_pUserResources->GetView();
	auto proj = m_pUserResources->GetProject();

	// ワールド座標
	DirectX::SimpleMath::Matrix pos = DirectX::SimpleMath::Matrix::CreateTranslation(m_pEnemy->GetPosition());
	DirectX::SimpleMath::Matrix scale = DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(Resources::GetInstance()->GetJson(L"Enemy.json")["EnemySize"]));
	DirectX::SimpleMath::Matrix rotate = DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_pEnemy->GetRotation());

	m_pEnemy->SetWorld(scale * rotate * pos);

	// アニメーションモデルを描画
	if (m_pEnemy->GetInvincibleTime() >= 0.0f && sinf(m_pEnemy->GetInvincibleTime() * 10) <= 0.0f)
	{
		return;
	}

	// ボーン数を取得
	size_t nbones = m_model->bones.size();
	// アニメションにモデルを適用する
	m_animation->Apply(*m_model, nbones, m_drawBones.get());
	// スキン変形用行列を適用する(これを実行しないとアニメーションが崩れる)
	m_animation->ApplySkinMatrix(*m_model, nbones, m_drawBones.get());

	m_model->DrawSkinned(
		context,
		*states, nbones,
		m_drawBones.get(),
		m_pEnemy->GetWorld(),
		*view,
		*proj
	);

	// 影の描画
	m_pEnemy->DrawShadow(context, states, Resources::GetInstance()->GetJson(L"Enemy.json")["ShadowSize"]);

	// 軸の描画
	context->OMSetBlendState(states->Opaque(), nullptr, 0xFFFFFFFF);

	// 深度の設定
	context->OMSetDepthStencilState(states->DepthDefault(), 0);

	// カリングの設定
	context->RSSetState(states->CullNone());

	// 
	m_basicEffect->SetView(*view);
	m_basicEffect->SetProjection(*proj);
	m_basicEffect->Apply(context);

	// インプットレイアウトの設定
	context->IASetInputLayout(m_inputLayout.Get());

	DirectX::SimpleMath::Vector3 forward = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f), m_pEnemy->GetRotation());
	DirectX::SimpleMath::Vector3 horizontal = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f), m_pEnemy->GetRotation());
	DirectX::SimpleMath::Vector3 vertical = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f), m_pEnemy->GetRotation());

	/*m_primitiveBatch->Begin();
	DX::DrawRay(m_primitiveBatch.get(), m_pEnemy->GetPosition(), forward, false, DirectX::Colors::Yellow);
	DX::DrawRay(m_primitiveBatch.get(), m_pEnemy->GetPosition(), horizontal, false, DirectX::Colors::Red);
	DX::DrawRay(m_primitiveBatch.get(), m_pEnemy->GetPosition(), vertical, false, DirectX::Colors::Green);
	m_primitiveBatch->End();*/

	/*auto* debugFont = m_pUserResources->GetDebugFont();*/

	// デバック
	/*m_pEnemy->GetCollider().Draw(states, *view, *proj);*/
	/*debugFont->Render(L"EnemyRunning");*/
	/*m_pEnemy->GetCatchCollider().Draw(states, *view, *proj);*/
}



/// <summary>
/// 終了処理
/// </summary>
void EnemyRunning::Finalize()
{
}



/// <summary>
/// 特定のイベントの処理
/// </summary>
/// <param name="e">イベント</param>
void EnemyRunning::EventHandle(Event e)
{
	UNREFERENCED_PARAMETER(e);
}



/// <summary>
/// アニメーションの更新
/// </summary>
/// <param name="elapsedTime">経過時間</param>
void EnemyRunning::AnimationUpdate(float elapsedTime)
{
	// ボーン数を取得する
	size_t nbones = m_model->bones.size();
	// アニメションにモデルを適用する
	m_animation->Apply(*m_model, nbones, m_drawBones.get());

	// アニメーション時間がアニメーション終了時間より小さい場合はアニメーションを繰り返す
	if (m_animation->GetAnimTime() < m_animation->GetEndTime())
	{
		// アニメーションを更新する
		m_animation->Update(elapsedTime);
	}
	else
	{
		// アニメーションの開始時間を設定する
		m_animation->SetStartTime(0.0);
	}

	m_rightHandMatrix = m_drawBones[15];
	m_leftHandMatrix = m_drawBones[20];
}



/// <summary>
/// ボールの方向に走る
/// </summary>
void EnemyRunning::RunToBall()
{
	Ball* ball = m_pEnemy->GetBallManager()->GetBall(m_pEnemy->GetBallIndex());

	if (ball->GetCurrentState() != ball->GetStopping())
	{
		m_pEnemy->ChangeState(m_pEnemy->GetStanding());
	}


	// 方向
	DirectX::SimpleMath::Vector3 dir = m_pEnemy->GetPosition() - ball->GetPosition();
	dir.Normalize();

	// 方向ベクトルの反転
	DirectX::SimpleMath::Vector3 targetUp;
	targetUp = -dir;

	// 現在の姿勢制御
	DirectX::SimpleMath::Vector3 currentUp = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitX, m_pEnemy->GetRotation());

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

	// 回転の設定
	m_pEnemy->SetRotation(m_pEnemy->GetRotation() * q);

	// 速度の設定
	m_pEnemy->SetVelocity(m_pEnemy->GetVelocity() + DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitX, m_pEnemy->GetRotation()) * Resources::GetInstance()->GetJson(L"Enemy.json")["EnemySpeed"]);
}



/// <summary>
/// 実体の方向に走る
/// </summary>
void EnemyRunning::RunToEntity()
{
	if (dynamic_cast<Ball*>(m_pEnemy->GetTarget()))
	{
		RunToBall();
	}
	else
	{
		m_pEnemy->SetTarget(NearEntity());

		// 方向
		DirectX::SimpleMath::Vector3 dir = m_pEnemy->GetPosition() - m_pEnemy->GetTarget()->GetPosition();
		dir.Normalize();

		// 方向ベクトルの反転
		DirectX::SimpleMath::Vector3 targetUp;
		targetUp = -dir;

		// 現在の姿勢制御
		DirectX::SimpleMath::Vector3 currentUp = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitX, m_pEnemy->GetRotation());

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

		// 回転の設定
		m_pEnemy->SetRotation(m_pEnemy->GetRotation() * q);

		// 速度の設定
		m_pEnemy->SetVelocity(m_pEnemy->GetVelocity() + DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitX, m_pEnemy->GetRotation())
			* Resources::GetInstance()->GetJson(L"Enemy.json")["EnemySpeed"]
		);
	}
}



/// <summary>
/// ボールを投げる
/// </summary>
/// <param name="mouseTK">マウストラッカー</param>
void EnemyRunning::ThrowBall()
{
	if (m_pEnemy->GetCatchBall(Enemy::RIGHT))
	{
		Ball* ball = m_pEnemy->GetCatchBall(Enemy::RIGHT);
		m_pEnemy->SetBallPosition(ball, m_rightHandMatrix);

		if (!dynamic_cast<Ball*>(m_pEnemy->GetTarget()))
		{
			DirectX::SimpleMath::Vector3 dir = m_pEnemy->GetPosition() - m_pEnemy->GetTarget()->GetPosition();
			if (dir.Length() <= 2.0f)
			{
				m_pEnemy->ChangeState(m_pEnemy->GetThrowingR());
				return;
			}
		}
	}
	if (m_pEnemy->GetCatchBall(Enemy::LEFT))
	{
		Ball* ball = m_pEnemy->GetCatchBall(Enemy::LEFT);
		m_pEnemy->SetBallPosition(ball, m_leftHandMatrix);

		if (!dynamic_cast<Ball*>(m_pEnemy->GetTarget()))
		{
			DirectX::SimpleMath::Vector3 dir = m_pEnemy->GetPosition() - m_pEnemy->GetTarget()->GetPosition();
			if (dir.Length() <= 2.0f)
			{
				m_pEnemy->ChangeState(m_pEnemy->GetThrowingL());
			}
		}
	}
}




/// <summary>
/// 一番近い実体を探す
/// </summary>
/// <returns>実体</returns>
IEntity* EnemyRunning::NearEntity()
{
	Ball* ball = m_pEnemy->GetBallManager()->GetBall(m_pEnemy->GetBallIndex());
	Player* player = m_pEnemy->GetPlayer();

	DirectX::SimpleMath::Vector3 dir1 = m_pEnemy->GetPosition() - ball->GetPosition();
	DirectX::SimpleMath::Vector3 dir2 = m_pEnemy->GetPosition() - player->GetPosition();

	IEntity* entity;
	DirectX::SimpleMath::Vector3 nearDir;

	// 短いほうの距離を調べる
	if (ball->GetCurrentState() != ball->GetStopping())
	{
		entity = player;
		dir1 = dir2;
	}
	else
	{
		if (dir1.Length() < dir2.Length())
		{
			entity = ball;
		}
		else
		{
			entity = player;
			dir1 = dir2;
		}
	}

	AirTarget* airTarget = m_pEnemy->GetAirTarget();
	dir2 = m_pEnemy->GetPosition() - airTarget->GetPosition();

	// 短いほうの距離を調べる
	if (dir1.Length() > dir2.Length())
	{
		entity = airTarget;
	}

	return entity;
}



/// <summary>
/// ボールを持つ
/// </summary>
void EnemyRunning::CatchHandBall()
{
	Ball* ball = m_pEnemy->GetBallManager()->GetBall(m_pEnemy->GetBallIndex());
	if (IsHit(m_pEnemy->GetCollider(), ball->GetCollider()) && ball->GetCurrentState() == ball->GetStopping())
	{
		// 両手に持っていたら終了
		if (m_pEnemy->GetCatchBall(Enemy::RIGHT) && m_pEnemy->GetCatchBall(Enemy::LEFT))
		{
			return;
		}

		// ボールの状態の変更
		ball->ChangeState(ball->GetCatching());

		// 色を変更する
		ball->SetBallColorNum(Ball::BallColor::ENEMY);

		if (!m_pEnemy->GetCatchBall(Enemy::RIGHT))
		{
			m_pEnemy->SetCatchBall(Enemy::RIGHT, ball);
			m_pEnemy->SetTarget(nullptr);
			m_pEnemy->ChangeState(m_pEnemy->GetStanding());
		}
		else
		{
			m_pEnemy->SetCatchBall(Enemy::LEFT, ball);
			m_pEnemy->SetTarget(nullptr);
			m_pEnemy->ChangeState(m_pEnemy->GetStanding());
		}

	}
}
