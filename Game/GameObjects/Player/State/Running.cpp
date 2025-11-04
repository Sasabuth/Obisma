/// <summary>
/// Runningに関するソースファイル
/// </summary>
/// <author>仲森智史</author>

// ヘッダファイルの読み込み
#include "pch.h"
#include "Running.h"

#include "Game/Scenes/GameplayScene.h"
#include "Game/GameObjects//Camera/Camera.h"
#include "Game/GameObjects/Field/Field.h"
#include "Game/GameObjects/Ball/Ball.h"
#include "DebugDraw.h"
#include "Game/Commons/Resources.h"



/// <summary>
/// コンストラクタ
/// </summary>
Running::Running(Player* pPlayer)
	: m_pPlayer(pPlayer)
	, m_pUserResources(nullptr)
	, m_model{}
{
	// モデルの作成
	m_model = Resources::GetInstance()->GetPlayerModel();

	// AnimationSDKMESH クラスのインスタンスを生成する
	m_animation = std::make_unique<DX::AnimationSDKMESH>();
	// サッカープレイヤー アイドリングアニメーションをロードする
	m_animation->Load(L"resources/Animations/Player_Run.sdkmesh_anim");
	// アニメーションとモデルをバインドする
	m_animation->Bind(*m_model);

	// ボーン用のトランスフォーム配列を生成する
	m_drawBones = DirectX::ModelBone::MakeArray(m_model->bones.size());
	ZeroMemory(m_drawBones.get(), sizeof(DirectX::ModelBone) * m_model->bones.size());
}



/// <summary>
/// デストラクタ
/// </summary>
Running::~Running()
{
}


/// <summary>
/// 初期化処理
/// </summary>
void Running::Initialize()
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

	// アニメーションの初期化
	AnimationUpdate(0.0f);
}



/// <summary>
/// 更新処理
/// </summary>
/// <param name="elapsedTime">経過時間</param> 
void Running::Update(float elapsedTime)
{
	auto kb = DirectX::Keyboard::Get().GetState();
	auto mouse = DirectX::Mouse::Get().GetState();
	auto mouseTK = m_pUserResources->GetMouseStateTracker();

	// プロジェクション行列
	auto proj = m_pUserResources->GetProject();
	auto view = m_pUserResources->GetView();

	// 速度の設定
	m_pPlayer->SetVelocity(m_pPlayer->GetGravity());


	// アニメーションの更新
	AnimationUpdate(elapsedTime);

	// レイの設定
	auto const r = m_pUserResources->GetDeviceResources()->GetOutputSize();
	m_pPlayer->SetMouseRay(m_pPlayer->CreatePickingRay(mouse.x, mouse.y, r.right, r.bottom, *view, *proj));

	// マウス方向の回転の更新
	UpdateRotateToMouse();

	// ボールをキャッチ
	CatchHandBall();

	if (m_pPlayer->GetCatchBall(Player::RIGHT))
	{
		Ball* ball = m_pPlayer->GetCatchBall(Player::RIGHT);
		m_pPlayer->SetBallPosition(ball, m_rightHandMatrix);
	}
	if (m_pPlayer->GetCatchBall(Player::LEFT))
	{
		Ball* ball = m_pPlayer->GetCatchBall(Player::LEFT);
		m_pPlayer->SetBallPosition(ball, m_leftHandMatrix);
	}



	// キーによる移動
	if (kb.W)
	{
		m_pPlayer->SetVelocity(m_pPlayer->GetVelocity() - DirectX::SimpleMath::Vector3::Transform(-DirectX::SimpleMath::Vector3::UnitX, m_pPlayer->GetRotation()) *
			Resources::GetInstance()->GetJson(L"Player.json")["PlayerSpeed"]
		);
	}
	else
	{
		m_pPlayer->ChangeState(m_pPlayer->GetStanding());
	}


	// 左クリックで投げる
	if (mouseTK->leftButton == mouseTK->PRESSED)
	{
		ThrowBall();
	}

	// 右クリックでキャッチ
	if (mouseTK->rightButton == mouseTK->PRESSED)
	{
		m_pPlayer->ChangeState(m_pPlayer->GetCatching());
	}

	// スコアを下げる
	m_pPlayer->ScoreDown();

	// プレイヤーの設定
	m_pPlayer->SetPosition(m_pPlayer->GetPosition() + m_pPlayer->GetVelocity() * elapsedTime);
	m_pPlayer->GetCollider().SetPosition(m_pPlayer->GetPosition());
}



/// <summary>
/// 描画処理
/// </summary>
void Running::Render()
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

	DirectX::SimpleMath::Vector3 forward = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f), m_pPlayer->GetRotation());
	DirectX::SimpleMath::Vector3 horizontal = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f), m_pPlayer->GetRotation());
	DirectX::SimpleMath::Vector3 vertical = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f), m_pPlayer->GetRotation());

	m_primitiveBatch->Begin();
	/*DX::DrawRay(m_primitiveBatch.get(), m_pPlayer->GetPosition(), forward, false, DirectX::Colors::Blue);
	DX::DrawRay(m_primitiveBatch.get(), m_pPlayer->GetPosition(), horizontal, false, DirectX::Colors::Red);
	DX::DrawRay(m_primitiveBatch.get(), m_pPlayer->GetPosition(), vertical, false, DirectX::Colors::Green);*/
	m_primitiveBatch->End();

	// デバック
	// auto* debugFont = m_pUserResources->GetDebugFont();

	/*m_pPlayer->GetCollider().Draw(states, *view, *proj);*/
	/*debugFont->Render(L"Running");*/
}



/// <summary>
/// 終了処理
/// </summary>
void Running::Finalize()
{
}



/// <summary>
/// アニメーションの更新
/// </summary>
/// <param name="elapsedTime">経過時間</param>
void Running::AnimationUpdate(float elapsedTime)
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
void Running::ThrowBall()
{
	// 右手に持っていたら投げる
	if (m_pPlayer->GetCatchBall(Player::RIGHT))
	{
		Ball* ball = m_pPlayer->GetCatchBall(Player::RIGHT);
		m_pPlayer->SetBallPosition(ball, m_rightHandMatrix);
		m_pPlayer->ChangeState(m_pPlayer->GetThrowingR());
		return;
	}
	// 左手に持っていたら投げる
	if (m_pPlayer->GetCatchBall(Player::LEFT))
	{
		Ball* ball = m_pPlayer->GetCatchBall(Player::LEFT);
		m_pPlayer->SetBallPosition(ball, m_leftHandMatrix);
		m_pPlayer->ChangeState(m_pPlayer->GetThrowingL());
	}
}



/// <summary>
/// マウス方向の回転の更新
/// </summary>
void Running::UpdateRotateToMouse()
{
	// 当たった座標
	DirectX::SimpleMath::Vector3 hitPos1;
	DirectX::SimpleMath::Vector3 hitPos2;

	// どちらが先に当たったか
	if (m_pPlayer->CalcRaySphere(m_pPlayer->GetAirTarget()->GetPosition(), m_pPlayer->GetAirTarget()->GetCollider().GetRadius(), hitPos1) &&
		m_pPlayer->CalcRaySphere(m_pPlayer->GetField()->GetCollider().GetPosition(), m_pPlayer->GetField()->GetCollider().GetRadius(), hitPos2))
	{
		DirectX::SimpleMath::Vector3 a;
		DirectX::SimpleMath::Vector3 b;

		a = m_pPlayer->GetMouseRay().position - hitPos1;
		b = m_pPlayer->GetMouseRay().position - hitPos2;

		if (a.Length() < b.Length())
		{
			m_pPlayer->SetHitPos(hitPos1);
		}
		else
		{
			m_pPlayer->SetHitPos(hitPos2);
		}

		// マウス方向に回転
		m_pPlayer->RotateToMouse();
	}
	else
	{
		// マウス方向に回転
		if (m_pPlayer->CalcRaySphere(m_pPlayer->GetAirTarget()->GetPosition(), m_pPlayer->GetAirTarget()->GetCollider().GetRadius(), m_pPlayer->GetHitPos()))
		{
			m_pPlayer->RotateToMouse();
		}
		else if (m_pPlayer->CalcRaySphere(m_pPlayer->GetField()->GetCollider().GetPosition(), m_pPlayer->GetField()->GetCollider().GetRadius(), m_pPlayer->GetHitPos()))
		{
			m_pPlayer->RotateToMouse();
		}
		else
		{
			m_pPlayer->SetHitPos(DirectX::SimpleMath::Vector3::Zero);
		}
	}
}



/// <summary>
/// ボールを持つ
/// </summary>
void Running::CatchHandBall()
{
	// どのボールが当たったか調べる
	for (int i = 0; i < m_pPlayer->GetBallManager()->GetObjectCount(); i++)
	{
		// 両手に持っていたら終了
		if (m_pPlayer->GetCatchBall(Player::RIGHT) && m_pPlayer->GetCatchBall(Player::LEFT))
		{
			return;
		}

		Ball* ball = m_pPlayer->GetBallManager()->GetBall(i);

		// 止まっているボールに当たったら
		if (IsHit(m_pPlayer->GetCollider(), ball->GetCollider()) && ball->GetCurrentState() == ball->GetStopping())
		{
			// ボールの状態の変更
			ball->ChangeState(ball->GetCatching());

			// 色を変更する
			ball->SetBallColorNum(Ball::BallColor::PLAYER);

			// 右手に持っていなかったら右手に持たせる
			if (!m_pPlayer->GetCatchBall(Player::RIGHT))
			{
				m_pPlayer->SetCatchBall(Player::RIGHT, ball);
			}
			// それ以外なら左手に持たせる
			else
			{
				m_pPlayer->SetCatchBall(Player::LEFT, ball);
			}
		}
	}
}
