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
Running::Running(Player* player)
	: m_player(player)
	, m_userResources(nullptr)
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
	auto mouseTK = m_userResources->GetMouseStateTracker();

	// プロジェクション行列
	auto proj = m_userResources->GetProject();
	auto view = m_userResources->GetView();

	// 速度の設定
	m_player->SetVelocity(m_player->GetGravity());


	// アニメーションの更新
	AnimationUpdate(elapsedTime);

	// レイの設定
	auto const r = m_userResources->GetDeviceResources()->GetOutputSize();
	m_player->SetMouseRay(m_player->CreatePickingRay(mouse.x, mouse.y, r.right, r.bottom, *view, *proj));

	// マウス方向の回転の更新
	UpdateRotateToMouse();

	// ボールをキャッチ
	CatchHandBall();

	if (m_player->GetCatchBall(Player::RIGHT))
	{
		Ball* ball = m_player->GetCatchBall(Player::RIGHT);
		m_player->SetBallPosition(ball, m_rightHandMatrix);
	}
	if (m_player->GetCatchBall(Player::LEFT))
	{
		Ball* ball = m_player->GetCatchBall(Player::LEFT);
		m_player->SetBallPosition(ball, m_leftHandMatrix);
	}



	// キーによる移動
	if (kb.W)
	{
		m_player->SetVelocity(m_player->GetVelocity() - DirectX::SimpleMath::Vector3::Transform(-DirectX::SimpleMath::Vector3::UnitX, m_player->GetRotation()) * Resources::GetInstance()->GetJson(L"Player.json")["Speed"]);
	}
	else
	{
		m_player->ChangeState(m_player->GetStanding());
	}


	// 左クリックで投げる
	if (mouseTK->leftButton == mouseTK->PRESSED)
	{
		ThrowBall();
	}

	// 右クリックでキャッチ
	if (mouseTK->rightButton == mouseTK->PRESSED)
	{
		m_player->ChangeState(m_player->GetCatching());
	}

	// スコアを下げる
	m_player->ScoreDown();

	// プレイヤーの設定
	m_player->SetPosition(m_player->GetPosition() + m_player->GetVelocity() * elapsedTime);
	m_player->GetCollider().SetPosition(m_player->GetPosition());
}



/// <summary>
/// 描画処理
/// </summary>
void Running::Render()
{
	auto context = m_userResources->GetDeviceResources()->GetD3DDeviceContext();
	auto states = m_userResources->GetCommonStates();
	auto view = m_userResources->GetView();
	auto proj = m_userResources->GetProject();

	// ワールド座標
	DirectX::SimpleMath::Matrix pos = DirectX::SimpleMath::Matrix::CreateTranslation(m_player->GetPosition());
	DirectX::SimpleMath::Matrix scale = DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(Resources::GetInstance()->GetJson(L"Player.json")["PlayerSize"]));
	DirectX::SimpleMath::Matrix rotate = DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_player->GetRotation());

	m_player->SetWorld(scale * rotate * pos);

	// アニメーションモデルを描画
	if (m_player->GetInvincibleTime() >= 0.0f && sinf(m_player->GetInvincibleTime() * 10) <= 0.0f)
	{
		return;
	}

	// ボーン数を取得
	size_t nbones = m_model->bones.size();

	m_model->DrawSkinned(
		context,
		*states, nbones,
		m_drawBones.get(),
		m_player->GetWorld(),
		*view,
		*proj
	);

	// 影の描画
	DirectX::SimpleMath::Vector3 m_drawPos;
	m_player->DrawShadow(context, states, Resources::GetInstance()->GetJson(L"Player.json")["ShadowSize"], m_drawPos);

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

	DirectX::SimpleMath::Vector3 forward = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f), m_player->GetRotation());
	DirectX::SimpleMath::Vector3 horizontal = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f), m_player->GetRotation());
	DirectX::SimpleMath::Vector3 vertical = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f), m_player->GetRotation());

	m_primitiveBatch->Begin();
	/*DX::DrawRay(m_primitiveBatch.get(), m_player->GetPosition(), forward, false, DirectX::Colors::Blue);
	DX::DrawRay(m_primitiveBatch.get(), m_player->GetPosition(), horizontal, false, DirectX::Colors::Red);
	DX::DrawRay(m_primitiveBatch.get(), m_player->GetPosition(), vertical, false, DirectX::Colors::Green);*/
	m_primitiveBatch->End();

	// デバック
	// auto* debugFont = m_userResources->GetDebugFont();

	/*m_player->GetCollider().Draw(states, *view, *proj);*/
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
	if (m_player->GetCatchBall(Player::RIGHT))
	{
		Ball* ball = m_player->GetCatchBall(Player::RIGHT);
		m_player->SetBallPosition(ball, m_rightHandMatrix);
		m_player->ChangeState(m_player->GetThrowingR());
		return;
	}
	// 左手に持っていたら投げる
	if (m_player->GetCatchBall(Player::LEFT))
	{
		Ball* ball = m_player->GetCatchBall(Player::LEFT);
		m_player->SetBallPosition(ball, m_leftHandMatrix);
		m_player->ChangeState(m_player->GetThrowingL());
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
	if (m_player->CalcRaySphere(m_player->GetScene()->GetAirTarget()->GetPosition(), m_player->GetScene()->GetAirTarget()->GetCollider().GetRadius(), hitPos1) &&
		m_player->CalcRaySphere(m_player->GetScene()->GetField().GetCollider().GetPosition(), m_player->GetScene()->GetField().GetCollider().GetRadius(), hitPos2))
	{
		DirectX::SimpleMath::Vector3 a;
		DirectX::SimpleMath::Vector3 b;

		a = m_player->GetMouseRay().position - hitPos1;
		b = m_player->GetMouseRay().position - hitPos2;

		if (a.Length() < b.Length())
		{
			m_player->SetHitPos(hitPos1);
		}
		else
		{
			m_player->SetHitPos(hitPos2);
		}

		// マウス方向に回転
		m_player->RotateToMouse();
	}
	else
	{
		// マウス方向に回転
		if (m_player->CalcRaySphere(m_player->GetScene()->GetAirTarget()->GetPosition(), m_player->GetScene()->GetAirTarget()->GetCollider().GetRadius(), m_player->GetHitPos()))
		{
			m_player->RotateToMouse();
		}
		else if (m_player->CalcRaySphere(m_player->GetScene()->GetField().GetCollider().GetPosition(), m_player->GetScene()->GetField().GetCollider().GetRadius(), m_player->GetHitPos()))
		{
			m_player->RotateToMouse();
		}
		else
		{
			m_player->SetHitPos(DirectX::SimpleMath::Vector3::Zero);
		}
	}
}



/// <summary>
/// ボールを持つ
/// </summary>
void Running::CatchHandBall()
{
	// どのボールが当たったか調べる
	for (int i = 0; i < m_player->GetBallManager()->GetObjectCount(); i++)
	{
		// 両手に持っていたら終了
		if (m_player->GetCatchBall(Player::RIGHT) && m_player->GetCatchBall(Player::LEFT))
		{
			return;
		}

		Ball* ball = m_player->GetBallManager()->GetBall(i);

		// 止まっているボールに当たったら
		if (IsHit(m_player->GetCollider(), ball->GetCollider()) && ball->GetCurrentState() == ball->GetStopping())
		{
			// ボールの状態の変更
			ball->ChangeState(ball->GetCatching());

			// 色を変更する
			ball->SetBallColorNum(Ball::BallColor::PLAYER);

			// 右手に持っていなかったら右手に持たせる
     		if (!m_player->GetCatchBall(Player::RIGHT))
			{
				m_player->SetCatchBall(Player::RIGHT, ball);
			}
			// それ以外なら左手に持たせる
			else
			{
				m_player->SetCatchBall(Player::LEFT, ball);
			}
		}
	}
}
