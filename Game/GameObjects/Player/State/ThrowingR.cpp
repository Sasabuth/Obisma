/// <summary>
/// ThrowingRに関するソースファイル
/// </summary>
/// <author>仲森智史</author>

// ヘッダファイルの読み込み
#include "pch.h"
#include "ThrowingR.h"

#include "Game/Scenes/GameplayScene.h"
#include "Game/GameObjects/Field/Field.h"
#include "Game/GameObjects/Ball/Ball.h"
#include "DebugDraw.h"
#include "Game/Commons/Resources.h"



/// <summary>
/// コンストラクタ
/// </summary>
ThrowingR::ThrowingR(Player* pPlayer)
	: m_pPlayer(pPlayer)
	, m_pUserResources(nullptr)
	, m_model{}
	, m_time{}
	, m_isThowing(false)
{
	// モデルの作成
	m_model = Resources::GetInstance()->GetPlayerModel();

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
ThrowingR::~ThrowingR()
{
}



/// <summary>
/// 初期化処理
/// </summary>
void ThrowingR::Initialize()
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

	m_time = 0.0f;
	m_isThowing = false;
}



/// <summary>
/// 更新処理
/// </summary>
/// <param name="elapsedTime">経過時間</param> 
void ThrowingR::Update(float elapsedTime)
{
	UNREFERENCED_PARAMETER(elapsedTime);

	auto kb = DirectX::Keyboard::Get().GetState();
	auto mouseTK = m_pUserResources->GetMouseStateTracker();

	// 投げていなかったら手に持たせる
	if (!m_isThowing)
	{
		// 方向
		DirectX::SimpleMath::Vector3 dir = m_pPlayer->GetPosition() - m_pPlayer->GetHitPos();
		dir.Normalize();

		// 方向ベクトルの反転
		DirectX::SimpleMath::Vector3 targetUp;
		targetUp = -dir;

		// 現在の姿勢制御
		DirectX::SimpleMath::Vector3 currentUp = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitX, m_pPlayer->GetRotation());

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

		m_pPlayer->SetRotation(m_pPlayer->GetRotation() * q);

		// 右手に持たせる
		Ball* ball = m_pPlayer->GetCatchBall(Player::RIGHT);
		m_pPlayer->SetBallPosition(ball, m_rightHandMatrix);

		// 時間になったら投げる
		if (m_animation->GetAnimTime() > ANIM_TIME)
		{
			// ボールのステートを変更
			ball->ChangeState(ball->GetMoving());

			// プレイヤーの向いている方向の取得
			DirectX::SimpleMath::Vector3 forward = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitZ, m_pPlayer->GetRotation());

			float angleDeg = DirectX::XMConvertToDegrees(angle);

			// 投げる角度の取得
			DirectX::SimpleMath::Quaternion rotate;
			// 角度に応じて投げる角度を調整
			if (angleDeg < 35.0f)
			{
				rotate = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(forward, DirectX::XMConvertToRadians(
					Resources::GetInstance()->GetJson(L"Player.json")["AngleLow"])
				);
			}
			else
			{
				rotate = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(forward, DirectX::XMConvertToRadians(
					Resources::GetInstance()->GetJson(L"Player.json")["AngleHigh"])
				);
			}

			// ボールの速度の取得
			float speed = Resources::GetInstance()->GetJson(L"Player.json")["BallSpeed"];

			// 当たる範囲外ならボールの速度を遅くする
			if (!m_pPlayer->IsInHitRange())
			{
				speed *= (float)Resources::GetInstance()->GetJson(L"Player.json")["Decay"];
			}

			// ボールの速度の設定
			ball->SetVelocity(DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitX, m_pPlayer->GetRotation() * rotate) * speed);

			// 右手から投げたことにする
			m_pPlayer->SetCatchBall(Player::RIGHT, nullptr);
			m_isThowing = true;
		}
	}

	// スコアを下げる
	m_pPlayer->ScoreDown();

	// プレイヤーの設定
	m_pPlayer->SetVelocity(m_pPlayer->GetGravity());
	m_pPlayer->SetPosition(m_pPlayer->GetPosition() + m_pPlayer->GetVelocity() * elapsedTime);
	m_pPlayer->GetCollider().SetPosition(m_pPlayer->GetPosition());

	// アニメーションを更新し終了したらステート変更
	if (m_animation->GetAnimTime() < m_animation->GetEndTime())
	{
		// 左手に持たせる
		Ball* ball = m_pPlayer->GetCatchBall(Player::LEFT);
		if (ball) m_pPlayer->SetBallPosition(ball, m_leftHandMatrix);

		// アニメーションを更新する
		m_animation->Update(elapsedTime);
	}
	else
	{
		// 移動ステートに変更
		if (kb.W) m_pPlayer->ChangeState(m_pPlayer->GetRunning());

		// 右クリックで捕るステートに変更
		else if (mouseTK->rightButton == mouseTK->PRESSED) m_pPlayer->ChangeState(m_pPlayer->GetCatching());

		// 何もしてないなら立ち状態にする
		else m_pPlayer->ChangeState(m_pPlayer->GetStanding());
	}

	// アニメーションの更新
	AnimationUpdate();

}



/// <summary>
/// 描画処理
/// </summary>
void ThrowingR::Render()
{
	auto context = m_pUserResources->GetDeviceResources()->GetD3DDeviceContext();
	auto states = m_pUserResources->GetCommonStates();
	auto view = m_pUserResources->GetView();
	auto proj = m_pUserResources->GetProject();

	// ワールド座標
	DirectX::SimpleMath::Matrix pos = DirectX::SimpleMath::Matrix::CreateTranslation(m_pPlayer->GetPosition());
	DirectX::SimpleMath::Matrix scale = DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(Resources::GetInstance()->GetJson(L"Player.json")["PlayerSize"]));
	DirectX::SimpleMath::Matrix rotate = DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_pPlayer->GetRotation());

	m_pPlayer->SetWorld(scale * rotate * pos);

	// アニメーションモデルを描画
	if (m_pPlayer->GetInvincibleTime() >= 0.0f && sinf(m_pPlayer->GetInvincibleTime() * 10) <= 0.0f)
	{
		return;
	}

	// ボーン数を取得
	size_t nbones = m_model->bones.size();

	m_model->DrawSkinned(
		context,
		*states, nbones,
		m_drawBones.get(),
		m_pPlayer->GetWorld(),
		*view,
		*proj
	);

	// 影の描画
	DirectX::SimpleMath::Vector3 m_drawPos;
	m_pPlayer->DrawShadow(context, states, Resources::GetInstance()->GetJson(L"Player.json")["ShadowSize"], m_drawPos);

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

	//DirectX::SimpleMath::Vector3 forward = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f), m_pPlayer->GetRotation());
	//DirectX::SimpleMath::Vector3 horizontal = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f), m_pPlayer->GetRotation());
	//DirectX::SimpleMath::Vector3 vertical = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f), m_pPlayer->GetRotation());

	//m_primitiveBatch->Begin();
	//DX::DrawRay(m_primitiveBatch.get(), m_pPlayer->GetPosition(), forward, false, DirectX::Colors::Yellow);
	//DX::DrawRay(m_primitiveBatch.get(), m_pPlayer->GetPosition(), horizontal, false, DirectX::Colors::Red);
	//DX::DrawRay(m_primitiveBatch.get(), m_pPlayer->GetPosition(), vertical, false, DirectX::Colors::Green);
	//m_primitiveBatch->End();

	// デバックフォントの描画
	//auto* debugFont = m_pUserResources->GetDebugFont();
}



/// <summary>
/// 終了処理
/// </summary>
void ThrowingR::Finalize()
{
}



/// <summary>
/// アニメーションの更新
/// </summary>
/// <param name="elapsedTime">経過時間</param>
void ThrowingR::AnimationUpdate()
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