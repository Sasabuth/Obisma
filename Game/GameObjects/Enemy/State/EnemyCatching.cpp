/// <summary>
/// EnemyCatchingに関するソースファイル
/// </summary>
/// <author>仲森智史</author>

// ヘッダファイルの読み込み
#include "pch.h"
#include "EnemyCatching.h"

#include "Game/GameObjects/Enemy/Enemy.h"
#include "Game/GameObjects/Field/Field.h"
#include "Game/GameObjects/Ball/BallManager.h"
#include "DebugDraw.h"
#include "Game/Commons/Resources.h"



/// <summary>
/// コンストラクタ
/// </summary>
EnemyCatching::EnemyCatching(Enemy* pEnemy)
	: m_pEnemy(pEnemy)
	, m_pUserResources(nullptr)
	, m_model{}
	, m_collider{}
{
	// モデルの作成
	m_model = Resources::GetInstance()->GetEnemyModel();

	// AnimationSDKMESH クラスのインスタンスを生成する
	m_animation = std::make_unique<DX::AnimationSDKMESH>();
	// サッカープレイヤー アイドリングアニメーションをロードする
	m_animation->Load(L"resources\\Animations\\Player_Catch.sdkmesh_anim");
	// アニメーションとモデルをバインドする
	m_animation->Bind(*m_model);
	// ボーン用のトランスフォーム配列を生成する
	m_drawBones = DirectX::ModelBone::MakeArray(m_model->bones.size());
	ZeroMemory(m_drawBones.get(), sizeof(DirectX::ModelBone) * m_model->bones.size());

	// アニメーションの初期化
	AnimationUpdate();
}



/// <summary>
/// デストラクタ
/// </summary>
EnemyCatching::~EnemyCatching()
{
}



/// <summary>
/// 初期化処理
/// </summary>
void EnemyCatching::Initialize()
{
	m_pUserResources = UserResources::GetUserResource();

	auto device = m_pUserResources->GetDeviceResources()->GetD3DDevice();
	auto context = m_pUserResources->GetDeviceResources()->GetD3DDeviceContext();

	// コライダーの初期化
	m_collider.Initialize(context, m_pEnemy->GetPosition(), COLLIDER_SIZE);

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
void EnemyCatching::Update(float elapsedTime)
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

	// キャッチ用コライダーの設定
	DirectX::SimpleMath::Vector3 catchPos =
		DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitX, m_pEnemy->GetRotation()) / 2.5 -
		DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitY, m_pEnemy->GetRotation()) / 3;

	m_collider.SetPosition(m_pEnemy->GetPosition() + catchPos);

	// ボールをキャッチする
	for (int i = 0; i < m_pEnemy->GetBallManager()->GetObjectCount(); i++)
	{
		Ball* ball = m_pEnemy->GetBallManager()->GetBall(i);
		if (ball->GetCurrentState() == ball->GetMoving())
		{
			if (IsHit(m_collider, ball->GetCollider()))
			{
				CatchHandBall(i);
			}
		}
	}

	// スコアを下げる
	m_pEnemy->ScoreDown();

	// プレイヤーの設定
	m_pEnemy->SetVelocity(m_pEnemy->GetGravity());
	m_pEnemy->SetPosition(m_pEnemy->GetPosition() + m_pEnemy->GetVelocity() * elapsedTime);
	m_pEnemy->GetCollider().SetPosition(m_pEnemy->GetPosition());

	// アニメーションを更新し終了したらステート変更
	if (m_animation->GetAnimTime() < m_animation->GetEndTime())
	{
		// アニメーションを更新する
		m_animation->Update(elapsedTime);
	}
	else
	{
		m_pEnemy->ChangeState(m_pEnemy->GetStanding());
		m_pEnemy->SetTarget(nullptr);
	}

	// アニメーションの更新
	AnimationUpdate();

	Resources::GetInstance()->Set3DSound(m_se.get(), m_pEnemy->GetPosition());
}



/// <summary>
/// 描画処理
/// </summary>
void EnemyCatching::Render()
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

	// デバック用
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

	/*debugFont->Render(L"EnemyCatching");
	debugFont->Render(L"CatchPos", DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitX, m_pEnemy->GetRotation()));

	m_collider.Draw(states, *view, *proj);*/
}



/// <summary>
/// 終了処理
/// </summary>
void EnemyCatching::Finalize()
{
}



/// <summary>
/// 特定のイベントの処理
/// </summary>
/// <param name="e">イベント</param>
void EnemyCatching::EventHandle(Event e)
{
	UNREFERENCED_PARAMETER(e);
}



/// <summary>
/// アニメーションの更新
/// </summary>
/// <param name="elapsedTime">経過時間</param>
void EnemyCatching::AnimationUpdate()
{
	// ボーン数を取得する
	size_t nbones = m_model->bones.size();
	// アニメションにモデルを適用する
	m_animation->Apply(*m_model, nbones, m_drawBones.get());
	// ボーンマトリクスを設定する
	m_rightHandMatrix = m_drawBones[15];
	m_leftHandMatrix = m_drawBones[20];
}



/// <summary>
/// ボールを持つ
/// </summary>
void EnemyCatching::CatchHandBall(int index)
{
	// SEを出す
	m_se = Resources::GetInstance()->GetSESound(L"BallCatch.wav", m_pEnemy->GetPosition(), false);

	// ボールのポインタを取得
	Ball* ball = m_pEnemy->GetBallManager()->GetBall(index);

	// 両手に持っていたら終了
	if (m_pEnemy->GetCatchBall(Enemy::RIGHT) && m_pEnemy->GetCatchBall(Enemy::LEFT))
	{
		// ボールの状態の変更
		ball->ChangeState(ball->GetStopping());
		return;
	}

	// ボールの状態の変更
	ball->ChangeState(ball->GetCatching());

	// 色を変更する
	ball->SetBallColorNum(Ball::BallColor::ENEMY);

	// 番号の設定
	m_pEnemy->SetBallIndex(index);

	// 右手に持っていなかったら右手に持たせる
	if (!m_pEnemy->GetCatchBall(Enemy::RIGHT))
	{
		m_pEnemy->SetCatchBall(Enemy::RIGHT, ball);
	}
	// それ以外なら左手に持たせる
	else
	{
		m_pEnemy->SetCatchBall(Enemy::LEFT, ball);
	}
}
