/// <summary>
/// EnemyRunningに関するソースファイル
/// </summary>
/// <author>仲森智史</author>

// ヘッダファイルの読み込み
#include "pch.h"
#include "EnemyRunning.h"

#include "Game/Scenes/GameplayScene.h"
#include "Game/GameObjects/Camera/Camera.h"
#include "Game/GameObjects/Field/Field.h"
#include "Game/GameObjects/Ball/Ball.h"
#include "DebugDraw.h"
#include "Game/Commons/Resources.h"



/// <summary>
/// コンストラクタ
/// </summary>
EnemyRunning::EnemyRunning(Enemy* enemy)
	: m_enemy(enemy)
	, m_userResources(nullptr)
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
	m_userResources = UserResources::GetUserResource();

	auto device = m_userResources->GetDeviceResources()->GetD3DDevice();
	auto context = m_userResources->GetDeviceResources()->GetD3DDeviceContext();

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
	m_enemy->SetVelocity(m_enemy->GetGravity());

	// ボールの方向に走る
	if (!m_enemy->GetCatchBall(Enemy::RIGHT) && !m_enemy->GetCatchBall(Enemy::LEFT))
	{
		RunToBall();
	}
	else
	{
		if(!m_enemy->GetTarget())  m_enemy->SetTarget(NearEntity());

		RunToEntity();
	}

	// ボールを投げる
	ThrowBall();

	// ボールを持つ
	CatchHandBall();

	// アニメーションの更新
	AnimationUpdate(elapsedTime);

	// スコアを下げる
	m_enemy->ScoreDown();

	// 敵の設定
	m_enemy->SetPosition(m_enemy->GetPosition() + m_enemy->GetVelocity() * elapsedTime);
	m_enemy->GetCollider().SetPosition(m_enemy->GetPosition());

	DirectX::SimpleMath::Vector3 catchPos = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitX, m_enemy->GetRotation()) / 2.5;

	m_enemy->GetCatchCollider().SetPosition(m_enemy->GetPosition() + catchPos);

	for (int i = 0; i < m_enemy->GetBallManager()->GetObjectCount(); i++)
	{
		Ball* ball = m_enemy->GetBallManager()->GetBall(i);
		if (IsHit(m_enemy->GetCatchCollider(), ball->GetCollider()))
		{
			if (ball->GetBallColorNum() != Ball::ENEMY && ball->GetCurrentState() == ball->GetMoving())
			{
				m_enemy->ChangeState(m_enemy->GetCatching());
			}
		}
	}
}



/// <summary>
/// 描画処理
/// </summary>
void EnemyRunning::Render()
{
	auto context = m_userResources->GetDeviceResources()->GetD3DDeviceContext();
	auto states = m_userResources->GetCommonStates();
	auto view = m_userResources->GetView();
	auto proj = m_userResources->GetProject();

	// ワールド座標
	DirectX::SimpleMath::Matrix pos = DirectX::SimpleMath::Matrix::CreateTranslation(m_enemy->GetPosition());
	DirectX::SimpleMath::Matrix scale = DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(Resources::GetInstance()->GetJson(L"Enemy.json")["EnemySize"]));
	DirectX::SimpleMath::Matrix rotate = DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_enemy->GetRotation());

	m_enemy->SetWorld(scale * rotate * pos);

	// アニメーションモデルを描画
	if (m_enemy->GetInvincibleTime() >= 0.0f && sinf(m_enemy->GetInvincibleTime() * 10) <= 0.0f)
	{
		return;
	}

	// ボーン数を取得
	size_t nbones = m_model->bones.size();

	m_model->DrawSkinned(
		context,
		*states, nbones,
		m_drawBones.get(),
		m_enemy->GetWorld(),
		*view,
		*proj
	);

	// 影の描画
	DirectX::SimpleMath::Vector3 m_drawPos;
	m_enemy->DrawShadow(context, states, Resources::GetInstance()->GetJson(L"Enemy.json")["ShadowSize"], m_drawPos);

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

	DirectX::SimpleMath::Vector3 forward = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f), m_enemy->GetRotation());
	DirectX::SimpleMath::Vector3 horizontal = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f), m_enemy->GetRotation());
	DirectX::SimpleMath::Vector3 vertical = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f), m_enemy->GetRotation());

	/*m_primitiveBatch->Begin();
	DX::DrawRay(m_primitiveBatch.get(), m_enemy->GetPosition(), forward, false, DirectX::Colors::Yellow);
	DX::DrawRay(m_primitiveBatch.get(), m_enemy->GetPosition(), horizontal, false, DirectX::Colors::Red);
	DX::DrawRay(m_primitiveBatch.get(), m_enemy->GetPosition(), vertical, false, DirectX::Colors::Green);
	m_primitiveBatch->End();*/

	/*auto* debugFont = m_userResources->GetDebugFont();*/

	// デバック
	/*m_enemy->GetCollider().Draw(states, *view, *proj);*/
	/*debugFont->Render(L"EnemyRunning");
	m_enemy->GetCatchCollider().Draw(states, *view, *proj);*/
}



/// <summary>
/// 終了処理
/// </summary>
void EnemyRunning::Finalize()
{
}



/// <summary>
/// アニメーションの更新
/// </summary>
/// <param name="elapsedTime">経過時間</param>
void EnemyRunning::AnimationUpdate(float elapsedTime)
{
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

	// アニメションにモデルを適用する
	m_animation->Apply(*m_model, m_model->bones.size(), m_drawBones.get());
	// ボーン数を取得する
	size_t nbones = m_model->bones.size();
	// ボーンマトリクスを設定する
	m_rightHandMatrix = m_drawBones[15];
	m_leftHandMatrix = m_drawBones[20];
	// スキン変形用行列を適用する(これを実行しないとアニメーションが崩れる)
	m_animation->ApplySkinMatrix(*m_model, nbones, m_drawBones.get());
}



/// <summary>
/// ボールの方向に走る
/// </summary>
void EnemyRunning::RunToBall()
{
	Ball* ball = m_enemy->GetBallManager()->GetBall(m_enemy->GetBallIndex());

	if (ball->GetCurrentState() != ball->GetStopping())
	{
		m_enemy->ChangeState(m_enemy->GetStanding());
	}


	// 方向
	DirectX::SimpleMath::Vector3 dir = m_enemy->GetPosition() - ball->GetPosition();
	dir.Normalize();

	// 方向ベクトルの反転
	DirectX::SimpleMath::Vector3 targetUp;
	targetUp = -dir;

	// 現在の姿勢制御
	DirectX::SimpleMath::Vector3 currentUp = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitX, m_enemy->GetRotation());

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
	m_enemy->SetRotation(m_enemy->GetRotation() * q);

	// 速度の設定
	m_enemy->SetVelocity(m_enemy->GetVelocity() - DirectX::SimpleMath::Vector3::Transform(-DirectX::SimpleMath::Vector3::UnitX, m_enemy->GetRotation()) * Resources::GetInstance()->GetJson(L"Enemy.json")["EnemySpeed"]);
}



/// <summary>
/// 実体の方向に走る
/// </summary>
void EnemyRunning::RunToEntity()
{
	if (dynamic_cast<Ball*>(m_enemy->GetTarget()))
	{
		RunToBall();
	}
	else
	{
		m_enemy->SetTarget(NearEntity());

		// 方向
		DirectX::SimpleMath::Vector3 dir = m_enemy->GetPosition() - m_enemy->GetTarget()->GetPosition();
		dir.Normalize();

		// 方向ベクトルの反転
		DirectX::SimpleMath::Vector3 targetUp;
		targetUp = -dir;

		// 現在の姿勢制御
		DirectX::SimpleMath::Vector3 currentUp = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitX, m_enemy->GetRotation());

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
		m_enemy->SetRotation(m_enemy->GetRotation() * q);

		// 速度の設定
		m_enemy->SetVelocity(m_enemy->GetVelocity() - DirectX::SimpleMath::Vector3::Transform(-DirectX::SimpleMath::Vector3::UnitX, m_enemy->GetRotation())
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
	if (m_enemy->GetCatchBall(Enemy::RIGHT))
	{
		Ball* ball = m_enemy->GetCatchBall(Enemy::RIGHT);
		m_enemy->SetBallPosition(ball, m_rightHandMatrix);

		if (!dynamic_cast<Ball*>(m_enemy->GetTarget()))
		{
			DirectX::SimpleMath::Vector3 dir = m_enemy->GetPosition() - m_enemy->GetTarget()->GetPosition();
			if (dir.Length() <= 2.0f)
			{
				m_enemy->ChangeState(m_enemy->GetThrowingR());
				return;
			}
		}
	}
	if (m_enemy->GetCatchBall(Enemy::LEFT))
	{
		Ball* ball = m_enemy->GetCatchBall(Enemy::LEFT);
		m_enemy->SetBallPosition(ball, m_leftHandMatrix);

		if (!dynamic_cast<Ball*>(m_enemy->GetTarget()))
		{
			DirectX::SimpleMath::Vector3 dir = m_enemy->GetPosition() - m_enemy->GetTarget()->GetPosition();
			if (dir.Length() <= 2.0f)
			{
				m_enemy->ChangeState(m_enemy->GetThrowingL());
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
	Ball* ball = m_enemy->GetBallManager()->GetBall(m_enemy->GetBallIndex());
	Player* player = m_enemy->GetScene()->GetPlayer();

	DirectX::SimpleMath::Vector3 dir1 = m_enemy->GetPosition() - ball->GetPosition();
	DirectX::SimpleMath::Vector3 dir2 = m_enemy->GetPosition() - player->GetPosition();

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

	AirTarget* airTarget = m_enemy->GetScene()->GetAirTarget();
	dir2 = m_enemy->GetPosition() - airTarget->GetPosition();

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
	Ball* ball = m_enemy->GetBallManager()->GetBall(m_enemy->GetBallIndex());
	if (IsHit(m_enemy->GetCollider(), ball->GetCollider()) && ball->GetCurrentState() == ball->GetStopping())
	{
		// 両手に持っていたら終了
		if (m_enemy->GetCatchBall(Enemy::RIGHT) && m_enemy->GetCatchBall(Enemy::LEFT))
		{
			return;
		}

		// ボールの状態の変更
		ball->ChangeState(ball->GetCatching());

		// 色を変更する
		ball->SetBallColorNum(Ball::BallColor::ENEMY);

		if (!m_enemy->GetCatchBall(Enemy::RIGHT))
		{
			m_enemy->SetCatchBall(Enemy::RIGHT, ball);
			m_enemy->SetTarget(nullptr);
			m_enemy->ChangeState(m_enemy->GetStanding());
		}
		else
		{
			m_enemy->SetCatchBall(Enemy::LEFT, ball);
			m_enemy->SetTarget(nullptr);
			m_enemy->ChangeState(m_enemy->GetStanding());
		}

	}
}
