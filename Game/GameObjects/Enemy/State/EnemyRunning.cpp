/// <summary>
/// EnemyRunningに関するソースファイル
/// </summary>
/// <author>仲森智史</author>

// ヘッダファイルの読み込み
#include "pch.h"
#include "EnemyRunning.h"

#include <random>
#include "Common/DebugDraw.h"
#include "Game/Commons/Resources.h"
#include "Game/Commons/Factory.h"
#include "Game/Commons/GameObjectMessenger.h"
#include "Game/GameObjects/Enemy/Enemy.h"
#include "Game/GameObjects/Player/Player.h"
#include "Game/GameObjects/Ball/Ball.h"
#include "Game/GameObjects/AirTarget/AirTarget.h"





/// <summary>
/// コンストラクタ
/// </summary>
EnemyRunning::EnemyRunning(Enemy* pEnemy)
	: m_pEnemy(pEnemy)
	, m_pUserResources(nullptr)
	, m_model{}
	, m_targetTime(0.0f)
	, m_throwDistance(0.0f)
{
	// モデルの作成
	m_model = pEnemy->GetModel();

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

	// ターゲット時間の初期化
	m_targetTime = 0.0f;

	// 投げる距離の初期化
	m_throwDistance = 0.0f;
}



/// <summary>
/// 更新処理
/// </summary>
/// <param name="elapsedTime">経過時間</param> 
void EnemyRunning::Update(float elapsedTime)
{
	// ボールがキャッチできるなら終了
	if (IsBallCatch())
	{
		return;
	}

	// 両方にボールを持っていなかったらボールの方向に走る
	if (!m_pEnemy->GetCatchBall(Enemy::RIGHT) && !m_pEnemy->GetCatchBall(Enemy::LEFT))
	{
		RunToBall();
	}
	// どちらかに持っていたら
	else
	{
		// ターゲットが設定されていなかったら設定
		if(!m_pEnemy->GetTarget()) m_pEnemy->SetTarget(NearEntity());

		// 実体のほうに走る
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

	//// 軸の描画
	//context->OMSetBlendState(states->Opaque(), nullptr, 0xFFFFFFFF);

	//// 深度の設定
	//context->OMSetDepthStencilState(states->DepthDefault(), 0);

	//// カリングの設定
	//context->RSSetState(states->CullNone());

	//// 
	//m_basicEffect->SetView(*view);
	//m_basicEffect->SetProjection(*proj);
	//m_basicEffect->Apply(context);

	//// インプットレイアウトの設定
	//context->IASetInputLayout(m_inputLayout.Get());

	//DirectX::SimpleMath::Vector3 forward = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f), m_pEnemy->GetRotation());
	//DirectX::SimpleMath::Vector3 horizontal = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f), m_pEnemy->GetRotation());
	//DirectX::SimpleMath::Vector3 vertical = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f), m_pEnemy->GetRotation());

	//m_primitiveBatch->Begin();
	//DX::DrawRay(m_primitiveBatch.get(), m_pEnemy->GetPosition(), forward, false, DirectX::Colors::Yellow);
	//DX::DrawRay(m_primitiveBatch.get(), m_pEnemy->GetPosition(), horizontal, false, DirectX::Colors::Red);
	//DX::DrawRay(m_primitiveBatch.get(), m_pEnemy->GetPosition(), vertical, false, DirectX::Colors::Green);
	//m_primitiveBatch->End();

	/*auto* debugFont = m_pUserResources->GetDebugFont();*/

	// デバック
	/*m_pEnemy->GetCollider().Draw(states, *view, *proj);*/
	/*debugFont->Render(L"EnemyRunning");*/
	/*debugFont->Render(L"ThrowDistance", m_throwDistance);*/
	/*m_pEnemy->GetCatchCollider().Draw(states, *view, *proj);*/
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
	// ボールの取得
	Ball* nearBall = dynamic_cast<Ball*>(GameObjectMessenger::GetInstance()->GetObject(Factory::BALL + m_pEnemy->GetBallIndex()));

	// 手に持っていなかったら一番近いボールを探す
	if (!m_pEnemy->GetCatchBall(Enemy::RIGHT) || !m_pEnemy->GetCatchBall(Enemy::LEFT))
	{
		// どのボールが一番近いかを調べる
		for (int i = 0; i < Resources::GetInstance()->GetJson(L"Ball.json")["BallCount"]; i++)
		{
			// ボールの取得
			Ball* ball = dynamic_cast<Ball*>(GameObjectMessenger::GetInstance()->GetObject(Factory::BALL + i));
			// ボールが止まっているたら近いボールを取得する
			if (ball->GetCurrentState() == ball->GetStopping() && i != m_pEnemy->GetBallIndex())
			{
				nearBall = m_pEnemy->FindNearBall(nearBall, ball, i);
			}
		}
	}

	// ボールが止まっていなかったらステート変更
	if (nearBall->GetCurrentState() != nearBall->GetStopping())
	{
		m_pEnemy->ChangeState(m_pEnemy->GetStanding());
	}

	// 方向
	DirectX::SimpleMath::Vector3 dir = m_pEnemy->GetPosition() - nearBall->GetPosition();
	dir.Normalize();

	// 方向ベクトルの反転
	DirectX::SimpleMath::Vector3 targetUp;
	targetUp = -dir;

	// 現在の姿勢制御
	DirectX::SimpleMath::Vector3 currentForward = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitX, m_pEnemy->GetRotation());
	DirectX::SimpleMath::Vector3 currentUp = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitY, m_pEnemy->GetRotation());

	// 回転軸の計算
	DirectX::SimpleMath::Vector3 axis = currentForward.Cross(targetUp);
	axis.Normalize();

	// 回転角(X軸)の計算
	float dotX = currentForward.Dot(targetUp);
	float angleX = acosf(dotX);

	// 回転角(Y軸)の計算
	float dotY = currentUp.Dot(-targetUp);
	float angleY = acosf(dotY);

	// クォータニオンの作成
	DirectX::SimpleMath::Quaternion q;

	// 角度が少しでもあるかつY軸が同じ角度でなければ軸を作る
	if (angleX > 0.01f && angleY > Resources::GetInstance()->GetJson(L"Enemy.json")["MaxAngleDiff"])
	{
		q = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(axis, angleX);
	}
	// なければ何もしない
	else
	{
		q = DirectX::SimpleMath::Quaternion::Identity;
	}

	m_pEnemy->SetRotation(m_pEnemy->GetRotation() * q);

	// 速度の設定
	m_pEnemy->SetVelocity(m_pEnemy->GetGravity() + DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitX, m_pEnemy->GetRotation())
		* Resources::GetInstance()->GetJson(L"Enemy.json")["EnemySpeed"]
	);
}



/// <summary>
/// 実体の方向に走る
/// </summary>
void EnemyRunning::RunToEntity()
{
	m_targetTime += (float)UserResources::GetUserResource()->GetStepTimer()->GetElapsedSeconds();

	if (m_targetTime > Resources::GetInstance()->GetJson(L"EnemyAI.json")["MaxTargetTime"])
	{
		// つねに近い方向に行く
		m_pEnemy->SetTarget(NearEntity());
		m_targetTime = 0.0f;
	}
	
	// 方向
	DirectX::SimpleMath::Vector3 dir = m_pEnemy->GetPosition() - m_pEnemy->GetTarget()->GetPosition();
	dir.Normalize();

	// 方向ベクトルの反転
	DirectX::SimpleMath::Vector3 targetUp;
	targetUp = -dir;

	// 現在の姿勢制御
	DirectX::SimpleMath::Vector3 currentForward = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitX, m_pEnemy->GetRotation());
	DirectX::SimpleMath::Vector3 currentUp = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitY, m_pEnemy->GetRotation());

	// 回転軸の計算
	DirectX::SimpleMath::Vector3 axis = currentForward.Cross(targetUp);
	axis.Normalize();

	// 回転角(X軸)の計算
	float dotX = currentForward.Dot(targetUp);
	float angleX = acosf(dotX);

	// 回転角(Y軸)の計算
	float dotY = currentUp.Dot(-targetUp);
	float angleY = acosf(dotY);

	// クォータニオンの作成
	DirectX::SimpleMath::Quaternion q;

	// 角度が少しでもあるかつY軸が同じ角度でなければ軸を作る
	if (angleX > 0.01f && angleY > Resources::GetInstance()->GetJson(L"Enemy.json")["MaxAngleDiff"])
	{
		q = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(axis, angleX);
	}
	// なければ何もしない
	else
	{
		q = DirectX::SimpleMath::Quaternion::Identity;
	}

	m_pEnemy->SetRotation(m_pEnemy->GetRotation() * q);

	// 速度の設定
	m_pEnemy->SetVelocity(m_pEnemy->GetGravity() + DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitX, m_pEnemy->GetRotation())
		* Resources::GetInstance()->GetJson(L"Enemy.json")["EnemySpeed"]
	);
}



/// <summary>
/// ボールを投げる
/// </summary>
/// <param name="mouseTK">マウストラッカー</param>
void EnemyRunning::ThrowBall()
{
	// 左手に持っていたら
	if (m_pEnemy->GetCatchBall(Enemy::LEFT))
	{
		// ボールを手に持たせる
		Ball* ball = m_pEnemy->GetCatchBall(Enemy::LEFT);
		m_pEnemy->SetBallPosition(ball, m_leftHandMatrix);

		// ターゲットがボール以外なら
		if (!dynamic_cast<Ball*>(m_pEnemy->GetTarget()))
		{
			// 距離が一定以内になったら投げる
			DirectX::SimpleMath::Vector3 dir = m_pEnemy->GetPosition() - m_pEnemy->GetTarget()->GetPosition();
			if (dir.Length() <= m_throwDistance)
			{
				m_pEnemy->ChangeState(m_pEnemy->GetThrowingL());
				return;
			}
		}
	}
	// 右手に持っていたら
	if (m_pEnemy->GetCatchBall(Enemy::RIGHT))
	{
		// ボールを手に持たせる
		Ball* ball = m_pEnemy->GetCatchBall(Enemy::RIGHT);
		m_pEnemy->SetBallPosition(ball, m_rightHandMatrix);

		// ターゲットがボール以外なら
		if (!dynamic_cast<Ball*>(m_pEnemy->GetTarget()))
		{
			// 距離が一定以内になったら投げる
			DirectX::SimpleMath::Vector3 dir = m_pEnemy->GetPosition() - m_pEnemy->GetTarget()->GetPosition();
			if (dir.Length() <= m_throwDistance)
			{
				m_pEnemy->ChangeState(m_pEnemy->GetThrowingR());
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
	// スコア郡の初期化
	float playerScore    = 0.0f;
	float ballScore      = 0.0f;
	float airTargetScore = 0.0f;

	// ボールの取得
	Ball* nearBall = dynamic_cast<Ball*>(GameObjectMessenger::GetInstance()->GetObject(Factory::BALL + m_pEnemy->GetBallIndex()));

	// 手に持っていなかったら一番近いボールを探す
	if (!m_pEnemy->GetCatchBall(Enemy::RIGHT) || !m_pEnemy->GetCatchBall(Enemy::LEFT))
	{
		// どのボールが一番近いかを調べる
		for (int i = 0; i < Resources::GetInstance()->GetJson(L"Ball.json")["BallCount"]; i++)
		{
			// ボールの取得
			Ball* ball = dynamic_cast<Ball*>(GameObjectMessenger::GetInstance()->GetObject(Factory::BALL + i));
			// ボールが止まっているたら近いボールを取得する
			if (ball->GetCurrentState() == ball->GetStopping() && i != m_pEnemy->GetBallIndex())
			{
				nearBall = m_pEnemy->FindNearBall(nearBall, ball, i);
			}
		}
	}
	// 手に持っているならスコア減少
	else
	{
		ballScore -= FLT_MAX;
	}

	// プレイヤーの取得
	Player* player = dynamic_cast<Player*>(GameObjectMessenger::GetInstance()->GetObject(Factory::PLAYER));
	// 空中の的の取得
	AirTarget* airTarget = dynamic_cast<AirTarget*>(GameObjectMessenger::GetInstance()->GetObject(Factory::AIRTARGET));

	// どちらが近いか距離で調べる
	DirectX::SimpleMath::Vector3 ballDir      = m_pEnemy->GetPosition() - nearBall->GetPosition();
	DirectX::SimpleMath::Vector3 playerDir    = m_pEnemy->GetPosition() - player->GetPosition();
	DirectX::SimpleMath::Vector3 airTargetDir = m_pEnemy->GetPosition() - airTarget->GetPosition();

	// 距離でスコア計算
	ballScore      += Resources::GetInstance()->GetJson(L"EnemyAI.json")["DistanceScore"] / ballDir.Length();
	playerScore    += Resources::GetInstance()->GetJson(L"EnemyAI.json")["DistanceScore"] / playerDir.Length();
	airTargetScore += Resources::GetInstance()->GetJson(L"EnemyAI.json")["DistanceScore"] / airTargetDir.Length();

	// スコア差を調べる
	float scoreDifference = player->GetScore()->GetScore() - m_pEnemy->GetScore()->GetScore();
	// 点差が開いているならスコア加算
	if (scoreDifference >= Resources::GetInstance()->GetJson(L"EnemyAI.json")["TrailScoreDifference"])
	{
		airTargetScore += Resources::GetInstance()->GetJson(L"EnemyAI.json")["AdvantageScore"];
	}

	// 一番近いボールが持っているならスコア減少
	if (nearBall->GetCurrentState() == nearBall->GetCatching())
	{
		ballScore -= FLT_MAX;
	}

	// プレイヤーがボールを投げているならスコア加算
	if (player->GetCurrentState() == player->GetThrowingR() || player->GetCurrentState() == player->GetThrowingL())
	{
		playerScore += Resources::GetInstance()->GetJson(L"EnemyAI.json")["AdvantageScore"];
	}

	// 無敵時間じゃなかったらスコア加算
	if (player->GetCurrentState() == player->GetDizzying() || player->GetInvincibleTime() > 0.1f)
	{
		playerScore -= FLT_MAX;
		airTargetScore += Resources::GetInstance()->GetJson(L"EnemyAI.json")["AirTargetBonusScore"];
	}

	// 実体の宣言
	IEntity* entity = nullptr;

	// 乱数のシード
	std::random_device rd;
	std::mt19937 engine(rd());

	// 一番高いスコアをターゲットにする
	if (ballScore > playerScore &&
		ballScore > airTargetScore)
	{
		// ターゲットをボールにする
		entity = nearBall;
	}
	else if (playerScore > airTargetScore)
	{
		// ターゲットをプレイヤーにする
		entity = player;

		// 範囲内をランダムに変更
		std::uniform_real_distribution<float> dist(
			Resources::GetInstance()->GetJson(L"EnemyAI.json")["PlayerThrowMinLength"], 
			Resources::GetInstance()->GetJson(L"EnemyAI.json")["PlayerThrowMaxLength"]
		);
		m_throwDistance = dist(engine);
	}
	else
	{
		// ターゲットを空中の的にする
		entity = airTarget;

		// 範囲内をランダムに変更
		std::uniform_real_distribution<float> dist(
			Resources::GetInstance()->GetJson(L"EnemyAI.json")["AirTargetThrowMinLength"],
			Resources::GetInstance()->GetJson(L"EnemyAI.json")["AirTargetThrowMaxLength"]
		);
		m_throwDistance = dist(engine);
	}

	// ターゲットを返す
	return entity;
}



/// <summary>
/// ボールがキャッチできるか
/// </summary>
/// <returns>できる</returns>
bool EnemyRunning::IsBallCatch() const
{
	for (int i = 0; i < Resources::GetInstance()->GetJson(L"Ball.json")["BallCount"]; i++)
	{
		// ボールの取得
		Ball* ball = dynamic_cast<Ball*>(GameObjectMessenger::GetInstance()->GetObject(Factory::BALL + i));

		// キャッチ用コライダーの設定
		DirectX::SimpleMath::Vector3 catchPos =
			DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitX, m_pEnemy->GetRotation()) / 2.5;

		m_pEnemy->GetCatchCollider().SetPosition(m_pEnemy->GetPosition() + catchPos);

		// 敵のボールではないかつボールが動いているなら
		if (ball->GetBallColorNum() != Ball::ENEMY && ball->GetCurrentState() == ball->GetMoving())
		{
			DirectX::SimpleMath::Vector3 ballDir = ball->GetPosition() - m_pEnemy->GetPosition();

			// 現在の姿勢制御
			DirectX::SimpleMath::Vector3 forward = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitX, m_pEnemy->GetRotation());
			forward.Normalize();

			// 回転角(X軸)の計算
			float dot = forward.Dot(ballDir);

			// 距離の取得
			float length = ballDir.Length();

			// 一定距離内かつ視界の範囲内だったら
			if (length <= (float)Resources::GetInstance()->GetJson(L"EnemyAI.json")["ViewDistance"] && dot > (float)Resources::GetInstance()->GetJson(L"EnemyAI.json")["ViewAngle"])
			{
				// 方向
				DirectX::SimpleMath::Vector3 dir = m_pEnemy->GetPosition() - ball->GetPosition();
				dir.Normalize();

				// 方向ベクトルの反転
				DirectX::SimpleMath::Vector3 targetUp;
				targetUp = -dir;

				// 現在の姿勢制御
				DirectX::SimpleMath::Vector3 currentForward = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitX, m_pEnemy->GetRotation());
				DirectX::SimpleMath::Vector3 currentUp = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitY, m_pEnemy->GetRotation());

				// 回転軸の計算
				DirectX::SimpleMath::Vector3 axis = currentForward.Cross(targetUp);
				axis.Normalize();

				// 回転角(X軸)の計算
				float dotX = currentForward.Dot(targetUp);
				float angleX = acosf(dotX);

				// 回転角(Y軸)の計算
				float dotY = currentUp.Dot(-targetUp);
				float angleY = acosf(dotY);

				// クォータニオンの作成
				DirectX::SimpleMath::Quaternion q;

				// 角度が少しでもあるかつY軸が同じ角度でなければ軸を作る
				if (angleX > 0.01f && angleY > (float)Resources::GetInstance()->GetJson(L"Enemy.json")["MaxAngleDiff"])
				{
					q = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(axis, angleX);
				}
				// なければ何もしない
				else
				{
					q = DirectX::SimpleMath::Quaternion::Identity;
				}

				m_pEnemy->SetRotation(m_pEnemy->GetRotation() * q);

				// キャッチ用のコライダーとボールが当たっていたら
				if (IsHit(m_pEnemy->GetCatchCollider(), ball->GetCollider()))
				{
					// ステート変更
					m_pEnemy->ChangeState(m_pEnemy->GetCatching());
					return true;
				}
			}
		}
	}

	return false;
}



/// <summary>
/// ボールを持つ
/// </summary>
void EnemyRunning::CatchHandBall()
{
	// ボールの取得
	Ball* ball = dynamic_cast<Ball*>(GameObjectMessenger::GetInstance()->GetObject(Factory::BALL + m_pEnemy->GetBallIndex()));

	// ボールが止まっているときにボールに当たったら
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

		// ボールを持っていなかったらどちらかに持たせる
		if (!m_pEnemy->GetCatchBall(Enemy::RIGHT))
		{
			m_pEnemy->SetCatchBall(Enemy::RIGHT, ball);
			// ターゲットの設定をなくす
			m_pEnemy->SetTarget(nullptr);
			m_pEnemy->ChangeState(m_pEnemy->GetStanding());
		}
		else
		{
			m_pEnemy->SetCatchBall(Enemy::LEFT, ball);
			// ターゲットの設定をなくす
			m_pEnemy->SetTarget(nullptr);
			m_pEnemy->ChangeState(m_pEnemy->GetStanding());
		}

		// ターゲット時間を最大にする
		m_targetTime = Resources::GetInstance()->GetJson(L"EnemyAI.json")["MaxTargetTime"];
	}
}
