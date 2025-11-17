/// <summary>
/// EnemyStandingに関するソースファイル
/// </summary>
/// <author>仲森智史</author>

// ヘッダファイルの読み込み
#include "pch.h"
#include "EnemyStanding.h"

#include "Game/GameObjects/Enemy/Enemy.h"
#include "Game/GameObjects/Field/Field.h"
#include "Game/GameObjects/Ball/BallManager.h"
#include "DebugDraw.h"
#include "Game/Commons/Resources.h"



/// <summary>
/// コンストラクタ
/// </summary>
EnemyStanding::EnemyStanding(Enemy* pEnemy)
	: m_pEnemy(pEnemy)
	, m_pUserResources(nullptr)
	, m_model{}
{
	// モデルの作成
	m_model = Resources::GetInstance()->GetEnemyModel();

	// AnimationSDKMESH クラスのインスタンスを生成する
	m_animation = std::make_unique<DX::AnimationSDKMESH>();
	// サッカープレイヤー アイドリングアニメーションをロードする
	m_animation->Load(L"resources\\Animations\\Player_Idle.sdkmesh_anim");
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
EnemyStanding::~EnemyStanding()
{
}



/// <summary>
/// 初期化処理
/// </summary>
void EnemyStanding::Initialize()
{
	m_pUserResources = UserResources::GetUserResource();

	auto device = m_pUserResources->GetDeviceResources()->GetD3DDevice();
	auto context = m_pUserResources->GetDeviceResources()->GetD3DDeviceContext();

	// アイドリングアニメーションの開始時間を設定する
	m_animation->SetStartTime(0.0f);
	// アイドリングアニメーションの終了時間を設定する
	m_animation->SetEndTime(1.4f);

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
void EnemyStanding::Update(float elapsedTime)
{
	// アニメーションの更新
	AnimationUpdate(elapsedTime);

	// 手に持っていなかったら一番近いボールを探す
	if (!m_pEnemy->GetCatchBall(Enemy::RIGHT) && !m_pEnemy->GetCatchBall(Enemy::LEFT))
	{
		Ball* ball = m_pEnemy->GetBallManager()->GetBall(0);
		m_pEnemy->SetBallIndex(0);
		for (int i = 1; i < m_pEnemy->GetBallManager()->GetObjectCount(); i++)
		{
			if (m_pEnemy->GetBallManager()->GetBall(i)->GetCurrentState() == m_pEnemy->GetBallManager()->GetBall(i)->GetStopping())
			{
				ball = GetNearBall(ball, i);
			}
		}

		if (ball->GetCurrentState() == ball->GetStopping())
		{
			m_pEnemy->ChangeState(m_pEnemy->GetRunning());
		}
	}
	else if (!m_pEnemy->GetCatchBall(Enemy::RIGHT) || !m_pEnemy->GetCatchBall(Enemy::LEFT))
	{
		Ball* ball = m_pEnemy->GetBallManager()->GetBall(0);
		m_pEnemy->SetBallIndex(0);
		for (int i = 1; i < m_pEnemy->GetBallManager()->GetObjectCount(); i++)
		{
			if (m_pEnemy->GetBallManager()->GetBall(i)->GetCurrentState() == m_pEnemy->GetBallManager()->GetBall(i)->GetStopping())
			{
				ball = GetNearBall(ball, i);
			}
		}

		m_pEnemy->ChangeState(m_pEnemy->GetRunning());
	}
	else
	{
		m_pEnemy->ChangeState(m_pEnemy->GetRunning());
	}

	// ボールを持つ
	CatchHandBall();

	// ボールを持っていたら投げる
	/*ThrowBall();*/

	// スコアを下げる
	m_pEnemy->ScoreDown();

	// 敵の設定
	m_pEnemy->SetVelocity(m_pEnemy->GetGravity());
	m_pEnemy->SetPosition(m_pEnemy->GetPosition() + m_pEnemy->GetVelocity() * elapsedTime);
	m_pEnemy->GetCollider().SetPosition(m_pEnemy->GetPosition());

	// キャッチ用コライダーの設定
	DirectX::SimpleMath::Vector3 catchPos =
		DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitX, m_pEnemy->GetRotation()) / 2.5 -
		DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitY, m_pEnemy->GetRotation()) / 3;

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
void EnemyStanding::Render()
{
	auto context = m_pUserResources->GetDeviceResources()->GetD3DDeviceContext();
	auto states = m_pUserResources->GetCommonStates();
	auto view = m_pUserResources->GetView();
	auto proj = m_pUserResources->GetProject();

	/*m_pEnemy->GetCollider().Draw(states, *view, *proj);*/

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

	m_model->DrawSkinned(
		context,
		*states, nbones,
		m_drawBones.get(),
		m_pEnemy->GetWorld(),
		*view,
		*proj
	);

	// 影の描画
	DirectX::SimpleMath::Vector3 m_drawPos;
	m_pEnemy->DrawShadow(context, states, Resources::GetInstance()->GetJson(L"Enemy.json")["ShadowSize"], m_drawPos);

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

	/*debugFont->Render(L"EnemyStanding");*/
}



/// <summary>
/// 終了処理
/// </summary>
void EnemyStanding::Finalize()
{
}



/// <summary>
/// アニメーションの更新
/// </summary>
/// <param name="elapsedTime">経過時間</param>
void EnemyStanding::AnimationUpdate(float elapsedTime)
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
/// 近い距離のボールを取得
/// </summary>
/// <param name="ball">ボールのポインタ</param>
/// <param name="index">インデックス</param>
/// <returns>近いボール</returns>
Ball* EnemyStanding::GetNearBall(Ball* ball, int index)
{
	// 止まっていなかったらボールを返す
	if (m_pEnemy->GetBallManager()->GetBall(m_pEnemy->GetBallIndex())->GetCurrentState() != m_pEnemy->GetBallManager()->GetBall(m_pEnemy->GetBallIndex())->GetStopping())
	{
		m_pEnemy->SetBallIndex(index);
		return m_pEnemy->GetBallManager()->GetBall(index);
	}

	Ball* ball1 = m_pEnemy->GetBallManager()->GetBall(index);

	DirectX::SimpleMath::Vector3 dir1 = m_pEnemy->GetPosition() - ball->GetPosition();
	DirectX::SimpleMath::Vector3 dir2 = m_pEnemy->GetPosition() - ball1->GetPosition();

	// 短いほうの距離を調べる
	if (dir1.Length() > dir2.Length())
	{
		m_pEnemy->SetBallIndex(index);
		return ball1;
	}

	return ball;
}



/// <summary>
/// ボールを持つ
/// </summary>
void EnemyStanding::CatchHandBall()
{
	if (m_pEnemy->GetCatchBall(Enemy::RIGHT))
	{
		Ball* ball = m_pEnemy->GetCatchBall(Enemy::RIGHT);
		m_pEnemy->SetBallPosition(ball, m_rightHandMatrix);
	}
	if (m_pEnemy->GetCatchBall(Enemy::LEFT))
	{
		Ball* ball = m_pEnemy->GetCatchBall(Enemy::LEFT);
		m_pEnemy->SetBallPosition(ball, m_leftHandMatrix);
	}

	for (int i = 0; i < m_pEnemy->GetBallManager()->GetObjectCount(); i++)
	{
		// 両手に持っていたら終了
		if (m_pEnemy->GetCatchBall(Enemy::RIGHT) && m_pEnemy->GetCatchBall(Enemy::LEFT))
		{
			return;
		}

		// ボールのポインタを取得
		Ball* ball = m_pEnemy->GetBallManager()->GetBall(i);

		// 止まっているボールに当たったらボールを拾う
		if (IsHit(m_pEnemy->GetCollider(), ball->GetCollider()) && ball->GetCurrentState() == ball->GetStopping())
		{
			// ボールの状態の変更
			ball->ChangeState(ball->GetCatching());

			// 色を変更する
			ball->SetBallColorNum(Ball::BallColor::ENEMY);


			if (!m_pEnemy->GetCatchBall(Enemy::RIGHT))
			{
				m_pEnemy->SetCatchBall(Enemy::RIGHT, ball);
			}
			else
			{
				m_pEnemy->SetCatchBall(Enemy::LEFT, ball);
			}
		}
	}
}
