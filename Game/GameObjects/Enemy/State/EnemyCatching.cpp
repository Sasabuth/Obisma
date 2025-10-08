/// <summary>
/// EnemyCatchingに関するソースファイル
/// </summary>
/// <author>仲森智史</author>

// ヘッダファイルの読み込み
#include "pch.h"
#include "EnemyCatching.h"

#include "Game/Scenes/GameplayScene.h"
#include "Game/GameObjects/Field/Field.h"
#include "Game/GameObjects/Ball/Ball.h"
#include "DebugDraw.h"
#include "Game/Commons/Resources.h"



/// <summary>
/// コンストラクタ
/// </summary>
EnemyCatching::EnemyCatching(Enemy* enemy)
	: m_enemy(enemy)
	, m_userResources(nullptr)
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
	m_userResources = UserResources::GetUserResource();

	auto device = m_userResources->GetDeviceResources()->GetD3DDevice();
	auto context = m_userResources->GetDeviceResources()->GetD3DDeviceContext();

	// コライダーの初期化
	m_collider.Initialize(context, m_enemy->GetPosition(), COLLIDER_SIZE);

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
	if (m_enemy->GetCatchBall(Enemy::RIGHT))
	{
		Ball* ball = m_enemy->GetCatchBall(Enemy::RIGHT);
		m_enemy->SetBallPosition(ball, m_rightHandMatrix);
	}
	if (m_enemy->GetCatchBall(Enemy::LEFT))
	{
		Ball* ball = m_enemy->GetCatchBall(Enemy::LEFT);
		m_enemy->SetBallPosition(ball, m_leftHandMatrix);
	}

	// キャッチ用コライダーの設定
	DirectX::SimpleMath::Vector3 catchPos =
		DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitX, m_enemy->GetRotation()) / 2.5 -
		DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitY, m_enemy->GetRotation()) / 3;

	m_collider.SetPosition(m_enemy->GetPosition() + catchPos);

	// ボールをキャッチする
	for (int i = 0; i < m_enemy->GetBallManager()->GetObjectCount(); i++)
	{
		Ball* ball = m_enemy->GetBallManager()->GetBall(i);
		if (ball->GetCurrentState() == ball->GetMoving())
		{
			if (IsHit(m_collider, ball->GetCollider()))
			{
				CatchHandBall(i);
			}
		}
	}

	// プレイヤーの設定
	m_enemy->SetVelocity(m_enemy->GetGravity());
	m_enemy->SetPosition(m_enemy->GetPosition() + m_enemy->GetVelocity() * elapsedTime);
	m_enemy->GetCollider().SetPosition(m_enemy->GetPosition());

	// アニメーションを更新し終了したらステート変更
	if (m_animation->GetAnimTime() < m_animation->GetEndTime())
	{
		// アニメーションを更新する
		m_animation->Update(elapsedTime);
	}
	else
	{
		m_enemy->ChangeState(m_enemy->GetStanding());
		m_enemy->SetTarget(nullptr);
	}

	// アニメーションの更新
	AnimationUpdate();

}



/// <summary>
/// 描画処理
/// </summary>
void EnemyCatching::Render()
{
	// デバックフォントの描画
	auto* debugFont = m_userResources->GetDebugFont();

	auto context = m_userResources->GetDeviceResources()->GetD3DDeviceContext();
	auto states = m_userResources->GetCommonStates();
	auto view = m_userResources->GetView();
	auto proj = m_userResources->GetProject();

	// ワールド座標
	DirectX::SimpleMath::Matrix pos = DirectX::SimpleMath::Matrix::CreateTranslation(m_enemy->GetPosition());
	DirectX::SimpleMath::Matrix scale = DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(Enemy::ENEMY_SIZE));
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
	m_enemy->DrawShadow(context, states, Enemy::SHADOW_SIZE, m_drawPos);

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

	DirectX::SimpleMath::Vector3 forward = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f), m_enemy->GetRotation());
	DirectX::SimpleMath::Vector3 horizontal = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f), m_enemy->GetRotation());
	DirectX::SimpleMath::Vector3 vertical = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f), m_enemy->GetRotation());

	/*m_primitiveBatch->Begin();
	DX::DrawRay(m_primitiveBatch.get(), m_enemy->GetPosition(), forward, false, DirectX::Colors::Yellow);
	DX::DrawRay(m_primitiveBatch.get(), m_enemy->GetPosition(), horizontal, false, DirectX::Colors::Red);
	DX::DrawRay(m_primitiveBatch.get(), m_enemy->GetPosition(), vertical, false, DirectX::Colors::Green);
	m_primitiveBatch->End();*/

	/*debugFont->Render(L"EnemyCatching");
	debugFont->Render(L"CatchPos", DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitX, m_enemy->GetRotation()));

	m_collider.Draw(states, *view, *proj);*/
}



/// <summary>
/// 終了処理
/// </summary>
void EnemyCatching::Finalize()
{
}



/// <summary>
/// アニメーションの更新
/// </summary>
/// <param name="elapsedTime">経過時間</param>
void EnemyCatching::AnimationUpdate()
{
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
/// ボールを持つ
/// </summary>
void EnemyCatching::CatchHandBall(int index)
{
	// ボールのポインタを取得
	Ball* ball = m_enemy->GetBallManager()->GetBall(index);

	// 両手に持っていたら終了
	if (m_enemy->GetCatchBall(Enemy::RIGHT) && m_enemy->GetCatchBall(Enemy::LEFT))
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
	m_enemy->SetBallIndex(index);

	// 右手に持っていなかったら右手に持たせる
	if (!m_enemy->GetCatchBall(Enemy::RIGHT))
	{
		m_enemy->SetCatchBall(Enemy::RIGHT, ball);
	}
	// それ以外なら左手に持たせる
	else
	{
		m_enemy->SetCatchBall(Enemy::LEFT, ball);
	}
}
