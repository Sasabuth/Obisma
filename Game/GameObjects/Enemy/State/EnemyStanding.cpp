/// <summary>
/// EnemyStandingに関するソースファイル
/// </summary>
/// <author>仲森智史</author>

// ヘッダファイルの読み込み
#include "pch.h"
#include "EnemyStanding.h"

#include "Game/Scenes/GameplayScene.h"
#include "Game/GameObjects/Field/Field.h"
#include "Game/GameObjects/Ball/Ball.h"
#include "DebugDraw.h"
#include "Game/Commons/Resources.h"


// 名前の省略
using namespace DirectX;


/// <summary>
/// コンストラクタ
/// </summary>
EnemyStanding::EnemyStanding(Enemy* enemy)
	: m_enemy(enemy)
	, m_userResources(nullptr)
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
	m_userResources = UserResources::GetUserResource();

	auto device = m_userResources->GetDeviceResources()->GetD3DDevice();
	auto context = m_userResources->GetDeviceResources()->GetD3DDeviceContext();

	m_worldMatrix = SimpleMath::Matrix::Identity;

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
	CreateInputLayoutFromEffect<DirectX::VertexPositionColor>(device, m_basicEffect.get(), m_inputLayout.ReleaseAndGetAddressOf());

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
	if (!m_enemy->GetCatchBall(Enemy::RIGHT)/*|| !m_enemy->GetCatchBall(Enemy::LEFT)*/)
	{
		Ball* ball = m_enemy->GetBallManager()->GetBall(0);
		m_enemy->SetBallIndex(0);
		for (int i = 1; i < m_enemy->GetBallManager()->GetObjectCount(); i++)
		{
			if (m_enemy->GetBallManager()->GetBall(i)->GetCurrentState() == m_enemy->GetBallManager()->GetBall(i)->GetStopping())
			{
				ball = GetNearBall(ball, i);
			}
		}

		if (ball->GetCurrentState() == ball->GetStopping())
		{
			m_enemy->ChangeState(m_enemy->GetRunning());
		}
		
	}

	// ボールを持つ
	CatchHandBall();

	// ボールを持っていたら投げる
	if (m_enemy->GetBallManager()->GetBall(m_enemy->GetBallIndex())->GetCurrentState() == m_enemy->GetBallManager()->GetBall(m_enemy->GetBallIndex())->GetCatching())
	{
		ThrowBall();
	}
	

	// 敵の設定
	m_enemy->SetVelocity(m_enemy->GetGravity());
	m_enemy->SetPosition(m_enemy->GetPosition() + m_enemy->GetVelocity() * elapsedTime);
	m_enemy->GetCollider().SetPosition(m_enemy->GetPosition());
}



/// <summary>
/// 描画処理
/// </summary>
void EnemyStanding::Render()
{
	// デバックフォントの描画
	auto* debugFont = m_userResources->GetDebugFont();

	auto context = m_userResources->GetDeviceResources()->GetD3DDeviceContext();
	auto states = m_userResources->GetCommonStates();
	auto view = m_userResources->GetView();
	auto proj = m_userResources->GetProject();

	// ワールド座標
	SimpleMath::Matrix pos = SimpleMath::Matrix::CreateTranslation(m_enemy->GetPosition());
	SimpleMath::Matrix scale = SimpleMath::Matrix::CreateScale(SimpleMath::Vector3(Player::PLAYER_SIZE));

	SimpleMath::Matrix rotate = SimpleMath::Matrix::CreateFromQuaternion(m_enemy->GetRotation()); // ※回転順に合わせて調整

	m_worldMatrix = scale * rotate * pos;

	// ボーン数を取得
	size_t nbones = m_model->bones.size();
	// アニメーションモデルを描画
	m_model->DrawSkinned(
		context,
		*states, nbones,
		m_drawBones.get(),
		m_worldMatrix,
		*view,
		*proj
	);

	SimpleMath::Vector3 m_drawPos;

	// 影の描画
	m_enemy->DrawShadow(context, states, Player::SHADOW_SIZE, m_drawPos);


	// デバック
	//m_model->Draw(context, *states, m_worldMatrix, *view, *proj);

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

	SimpleMath::Vector3 forward = SimpleMath::Vector3::Transform(SimpleMath::Vector3(0.0f, 0.0f, 1.0f), m_enemy->GetRotation());
	SimpleMath::Vector3 horizontal = SimpleMath::Vector3::Transform(SimpleMath::Vector3(1.0f, 0.0f, 0.0f), m_enemy->GetRotation());
	SimpleMath::Vector3 vertical = SimpleMath::Vector3::Transform(SimpleMath::Vector3(0.0f, 1.0f, 0.0f), m_enemy->GetRotation());

	m_primitiveBatch->Begin();
	DX::DrawRay(m_primitiveBatch.get(), m_enemy->GetPosition(), forward, false, DirectX::Colors::Yellow);
	DX::DrawRay(m_primitiveBatch.get(), m_enemy->GetPosition(), horizontal, false, DirectX::Colors::Red);
	DX::DrawRay(m_primitiveBatch.get(), m_enemy->GetPosition(), vertical, false, DirectX::Colors::Green);
	m_primitiveBatch->End();

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
/// ボールを投げる
/// </summary>
/// <param name="mouseTK">マウストラッカー</param>
void EnemyStanding::ThrowBall()
{
	if (m_enemy->GetCatchBall(Player::RIGHT))
	{
		Ball* ball = m_enemy->GetCatchBall(Player::RIGHT);
		SetBallPosition(ball, m_rightHandMatrix);

		m_enemy->ChangeState(m_enemy->GetThrowingR());
		return;
	}
	if (m_enemy->GetCatchBall(Player::LEFT))
	{
		Ball* ball = m_enemy->GetCatchBall(Player::LEFT);
		SetBallPosition(ball, m_leftHandMatrix);

		/*if (mouseTK->leftButton)
		{
			m_enemy->ChangeState(m_enemy->GetThrowingL());
		}*/
	}
}



/// <summary>
/// ボールの座標の設定
/// </summary>
/// <param name="ball">ボールのポインタ</param>
/// <param name="handMatrix">手のマトリックス</param>
void EnemyStanding::SetBallPosition(Ball* ball, DirectX::SimpleMath::Matrix handMatrix)
{
	// ボーンに設定した境界球のワールド計算を行う
	DirectX::SimpleMath::Matrix sphereMatrix = handMatrix * m_worldMatrix;
	// バウンディングスフィアの中心点を設定する
	SimpleMath::Vector3 dir = SimpleMath::Vector3(sphereMatrix._41, sphereMatrix._42, sphereMatrix._43);
	dir.Normalize();
	ball->SetPosition(SimpleMath::Vector3(dir.x * 3.2f, dir.y * 3.2f, dir.z * 3.2f));
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
	if (m_enemy->GetBallManager()->GetBall(m_enemy->GetBallIndex())->GetCurrentState() != m_enemy->GetBallManager()->GetBall(m_enemy->GetBallIndex())->GetStopping())
	{
		m_enemy->SetBallIndex(index);
		return m_enemy->GetBallManager()->GetBall(index);
	}

	Ball* ball1 = m_enemy->GetBallManager()->GetBall(index);

	SimpleMath::Vector3 dir1 = m_enemy->GetPosition() - ball->GetPosition();
	SimpleMath::Vector3 dir2 = m_enemy->GetPosition() - ball1->GetPosition();

	// 短いほうの距離を調べる
	if (dir1.Length() > dir2.Length())
	{
		m_enemy->SetBallIndex(index);
		return ball1;
	}

	return ball;
}



/// <summary>
/// ボールを持つ
/// </summary>
void EnemyStanding::CatchHandBall()
{
	// 両手に持っていたら終了
	if (m_enemy->GetCatchBall(Player::RIGHT) && m_enemy->GetCatchBall(Player::LEFT))
	{
		return;
	}

	for (int i = 0; i < m_enemy->GetBallManager()->GetObjectCount(); i++)
	{
		// ボールのポインタを取得
		Ball* ball = m_enemy->GetBallManager()->GetBall(i);

		// 止まっているボールに当たったらボールを拾う
		if (IsHit(m_enemy->GetCollider(), ball->GetCollider()) && ball->GetCurrentState() == ball->GetStopping())
		{
			// ボールの状態の変更
			ball->ChangeState(ball->GetCatching());

			// 色を変更する
			ball->SetBallColorNum(Ball::BallColor::ENEMY);


			if (!m_enemy->GetCatchBall(Enemy::RIGHT))
			{
				m_enemy->SetCatchBall(Enemy::RIGHT, ball);
			}
			else
			{
				m_enemy->SetCatchBall(Enemy::LEFT, ball);
			}
		}
	}
}
