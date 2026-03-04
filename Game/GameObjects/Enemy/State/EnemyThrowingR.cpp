/// <summary>
/// EnemyThrowingRに関するソースファイル
/// </summary>
/// <author>仲森智史</author>

// ヘッダファイルの読み込み
#include "pch.h"
#include "EnemyThrowingR.h"

#include "Game/GameObjects/Enemy/Enemy.h"
#include "Game/GameObjects/Field/Field.h"
#include "Game/GameObjects/Ball/Ball.h"
#include "Common/DebugDraw.h"
#include "Game/Commons/Resources.h"



/// <summary>
/// コンストラクタ
/// </summary>
EnemyThrowingR::EnemyThrowingR(Enemy* pEnemy)
	: m_pEnemy(pEnemy)
	, m_pUserResources(nullptr)
	, m_model{}
	, m_time{}
	, m_isThowing(false)
{
	// モデルの作成
	m_model = pEnemy->GetModel();

	// AnimationSDKMESH クラスのインスタンスを生成する
	m_animation = std::make_unique<DX::AnimationSDKMESH>();
	// サッカープレイヤー アイドリングアニメーションをロードする
	m_animation->Load(L"resources\\Animations\\Player_ThrowR.sdkmesh_anim");
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
EnemyThrowingR::~EnemyThrowingR()
{
}



/// <summary>
/// 初期化処理
/// </summary>
void EnemyThrowingR::Initialize()
{
	m_pUserResources = UserResources::GetUserResource();

	auto device = m_pUserResources->GetDeviceResources()->GetD3DDevice();
	auto context = m_pUserResources->GetDeviceResources()->GetD3DDeviceContext();

	// アイドリングアニメーションの開始時間を設定する
	m_animation->SetStartTime(0.0f);
	// アイドリングアニメーションの終了時間を設定する
	m_animation->SetEndTime(1.42f);

	// ベーシックエフェクトの作成
	m_basicEffect = std::make_unique<DirectX::BasicEffect>(device);
	m_basicEffect->SetVertexColorEnabled(true);

	// プリミティブバッチの作成
	m_primitiveBatch = std::make_unique<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>>(context);

	// 入力レイアウトの作成
	DirectX::CreateInputLayoutFromEffect<DirectX::VertexPositionColor>(device, m_basicEffect.get(), m_inputLayout.ReleaseAndGetAddressOf());

	// 時間の初期化
	m_time = 0.0f;

	// 投げたかの初期化
	m_isThowing = false;
}



/// <summary>
/// 更新処理
/// </summary>
/// <param name="elapsedTime">経過時間</param> 
void EnemyThrowingR::Update(float elapsedTime)
{
	// 投げていなかったら手に持たせる
	if (!m_isThowing)
	{
		auto* entity = m_pEnemy->GetTarget();

		// 方向
		DirectX::SimpleMath::Vector3 dir = m_pEnemy->GetPosition() - entity->GetPosition();
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

		m_pEnemy->SetRotation(m_pEnemy->GetRotation() * q);

		// 右手に持たせる
		Ball* ball = m_pEnemy->GetCatchBall(Enemy::RIGHT);
		m_pEnemy->SetBallPosition(ball, m_rightHandMatrix);

		// 時間になったら投げる
		if (m_animation->GetAnimTime() > 0.58f)
		{
			// ボールのステートを変更
			ball->ChangeState(ball->GetMoving());

			// 敵の向いている方向の取得
			DirectX::SimpleMath::Vector3 forward = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitZ, m_pEnemy->GetRotation());

			float angleDeg = DirectX::XMConvertToDegrees(angle);
			m_debugAngle = DirectX::XMConvertToDegrees(angle);

			// 投げる角度の取得
			DirectX::SimpleMath::Quaternion rotate;
			// 角度に応じて投げる角度を調整
			if (angleDeg < 35.0f)
			{
				rotate = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(forward, DirectX::XMConvertToRadians(
					Resources::GetInstance()->GetJson(L"Enemy.json")["AngleLow"])
				);
			}
			else
			{
				rotate = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(forward, DirectX::XMConvertToRadians(
					Resources::GetInstance()->GetJson(L"Enemy.json")["AngleHigh"])
				);
			}

			// ボールの速度の取得
			ball->SetVelocity(DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitX, m_pEnemy->GetRotation() * rotate) *
				Resources::GetInstance()->GetJson(L"Enemy.json")["BallSpeed"]
			);

			// 右手から投げたことにする
			m_pEnemy->SetCatchBall(Enemy::RIGHT, nullptr);
			m_isThowing = true;
		}
	}

	// スコアを下げる
	m_pEnemy->ScoreDown();

	// 敵の設定
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
		m_pEnemy->SetTarget(nullptr);
		m_pEnemy->ChangeState(m_pEnemy->GetStanding());
	}

	// アニメーションの更新
	AnimationUpdate();

}



/// <summary>
/// 描画処理
/// </summary>
void EnemyThrowingR::Render()
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

	DirectX::SimpleMath::Vector3 dir = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitZ, m_pEnemy->GetRotation());
	DirectX::SimpleMath::Quaternion rot = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(forward, DirectX::XMConvertToRadians(15));
	DirectX::SimpleMath::Vector3 horizontal = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f), m_pEnemy->GetRotation() * rot);

	DirectX::SimpleMath::Vector3 vertical = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f), m_pEnemy->GetRotation());

	/*m_primitiveBatch->Begin();
	DX::DrawRay(m_primitiveBatch.get(), m_pEnemy->GetPosition(), forward, false, DirectX::Colors::Yellow);
	DX::DrawRay(m_primitiveBatch.get(), m_pEnemy->GetPosition(), horizontal, false, DirectX::Colors::Red);
	DX::DrawRay(m_primitiveBatch.get(), m_pEnemy->GetPosition(), vertical, false, DirectX::Colors::Green);
	m_primitiveBatch->End();*/

	//auto* debugFont = m_pUserResources->GetDebugFont();

	/*debugFont->Render(L"EnemyThrowingR");*/
	/*debugFont->Render(L"debugAngle", m_debugAngle);*/
}



/// <summary>
/// 終了処理
/// </summary>
void EnemyThrowingR::Finalize()
{
}



/// <summary>
/// 特定のイベントの処理
/// </summary>
/// <param name="e">イベント</param>
void EnemyThrowingR::EventHandle(Event e)
{
	UNREFERENCED_PARAMETER(e);
}



/// <summary>
/// アニメーションの更新
/// </summary>
/// <param name="elapsedTime">経過時間</param>
void EnemyThrowingR::AnimationUpdate()
{
	// ボーン数を取得する
	size_t nbones = m_model->bones.size();
	// アニメションにモデルを適用する
	m_animation->Apply(*m_model, nbones, m_drawBones.get());
	// ボーンマトリクスを設定する
	m_rightHandMatrix = m_drawBones[15];
}