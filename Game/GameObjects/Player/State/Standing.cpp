/// <summary>
/// Standingに関するソースファイル
/// </summary>
/// <author>仲森智史</author>

// ヘッダファイルの読み込み
#include "pch.h"
#include "Standing.h"

#include "Game/Scenes/GameplayScene.h"
#include "Game/GameObjects/Field/Field.h"
#include "Game/GameObjects/Ball/Ball.h"
#include "DebugDraw.h"
#include "Game/Commons/Resources.h"



/// <summary>
/// コンストラクタ
/// </summary>
Standing::Standing(Player* player)
	: m_player(player)
	, m_userResources(nullptr)
	, m_model{}
{
	// モデルの作成
	m_model = Resources::GetInstance()->GetPlayerModel();

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
Standing::~Standing()
{
}



/// <summary>
/// 初期化処理
/// </summary>
void Standing::Initialize()
{
	m_userResources = UserResources::GetUserResource();

	auto device = m_userResources->GetDeviceResources()->GetD3DDevice();
	auto context = m_userResources->GetDeviceResources()->GetD3DDeviceContext();

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
void Standing::Update(float elapsedTime)
{
	UNREFERENCED_PARAMETER(elapsedTime);

	auto kbTracker = m_userResources->GetKeyboardStateTracker();
	auto mouse = DirectX::Mouse::Get().GetState();
	auto mouseTK = m_userResources->GetMouseStateTracker();

	// プロジェクション行列
	auto proj = m_userResources->GetProject();
	auto view = m_userResources->GetView();


	// アニメーションの更新
	AnimationUpdate(elapsedTime);

	// レイの設定
	auto const r = m_userResources->GetDeviceResources()->GetOutputSize();
	m_player->SetMouseRay(m_player->CreatePickingRay(mouse.x, mouse.y, r.right, r.bottom, *view, *proj));

	// マウス方向の回転の更新
	UpdateRotateToMouse();

	// ステートの変更
	if (kbTracker->pressed.W)
	{
		m_player->ChangeState(m_player->GetRunning());
	}

	// ボールをキャッチする
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

	// ボールを投げる
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
	m_player->SetVelocity(m_player->GetGravity());
	m_player->SetPosition(m_player->GetPosition() + m_player->GetVelocity() * elapsedTime);
	m_player->GetCollider().SetPosition(m_player->GetPosition());
}



/// <summary>
/// 描画処理
/// </summary>
void Standing::Render()
{
	// デバックフォントの描画
	auto* debugFont = m_userResources->GetDebugFont();

	auto context = m_userResources->GetDeviceResources()->GetD3DDeviceContext();
	auto states = m_userResources->GetCommonStates();
	auto view = m_userResources->GetView();
	auto proj = m_userResources->GetProject();

	/*if (m_player->CalcRaySphere(m_player->GetMouseRay().position, m_player->GetMouseRay().direction, m_player->GetScene()->GetAirTarget()->GetPosition(), m_player->GetScene()->GetAirTarget()->GetCollider().GetRadius(), m_player->GetHitPos()))
	{
		m_player->DrawLockOn(m_player->GetScene()->GetAirTarget()->GetPosition());
	}*/

	// ワールド座標
	DirectX::SimpleMath::Matrix pos = DirectX::SimpleMath::Matrix::CreateTranslation(m_player->GetPosition());
	DirectX::SimpleMath::Matrix scale = DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(Player::PLAYER_SIZE));
	DirectX::SimpleMath::Matrix rotate = DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_player->GetRotation());

	m_player->SetWorld(scale * rotate * pos);

	// 無敵時間中なら点滅描画
	if (m_player->GetInvincibleTime() >= 0.0f && sinf(m_player->GetInvincibleTime() * 10) <= 0.0f)
	{
		return;
	}

	// ボーン数を取得
	size_t nbones = m_model->bones.size();

	// アニメーションモデルを描画
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
	m_player->DrawShadow(context, states, Player::SHADOW_SIZE, m_drawPos);

	// デバック
	/*m_model->Draw(context, *states, world, *view, *proj);*/

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

	//m_primitiveBatch->Begin();
	//DX::DrawRay(m_primitiveBatch.get(), m_player->GetPosition(), forward, false, DirectX::Colors::Yellow);
	//DX::DrawRay(m_primitiveBatch.get(), m_player->GetPosition(), horizontal, false, DirectX::Colors::Red);
	//DX::DrawRay(m_primitiveBatch.get(), m_player->GetPosition(), vertical, false, DirectX::Colors::Green);
	//m_primitiveBatch->End();

	/*debugFont->Render(L"Standing");*/
}



/// <summary>
/// 終了処理
/// </summary>
void Standing::Finalize()
{
}



/// <summary>
/// アニメーションの更新
/// </summary>
/// <param name="elapsedTime">経過時間</param>
void Standing::AnimationUpdate(float elapsedTime)
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
	m_rightHandMatrix = m_drawBones[15];
	m_leftHandMatrix = m_drawBones[20];
	// スキン変形用行列を適用する(これを実行しないとアニメーションが崩れる)
	m_animation->ApplySkinMatrix(*m_model, nbones, m_drawBones.get());
}



/// <summary>
/// ボールを投げる
/// </summary>
void Standing::ThrowBall()
{
	if (m_player->GetCatchBall(Player::RIGHT))
	{
		Ball* ball = m_player->GetCatchBall(Player::RIGHT);
		m_player->SetBallPosition(ball, m_rightHandMatrix);
		m_player->ChangeState(m_player->GetThrowingR());
		return;
		
	}
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
void Standing::UpdateRotateToMouse()
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
void Standing::CatchHandBall()
{
	for (int i = 0; i < m_player->GetBallManager()->GetObjectCount(); i++)
	{
		// 両手に持っていたら終了
		if (m_player->GetCatchBall(Player::RIGHT) && m_player->GetCatchBall(Player::LEFT))
		{
			return;
		}

		Ball* ball = m_player->GetBallManager()->GetBall(i);
		if (IsHit(m_player->GetCollider(), ball->GetCollider()) && ball->GetCurrentState() == ball->GetStopping())
		{
			// ボールの状態の変更
			ball->ChangeState(ball->GetCatching());

			// 色を変更する
			ball->SetBallColorNum(Ball::BallColor::PLAYER);

			if (!m_player->GetCatchBall(Player::RIGHT))
			{
				m_player->SetCatchBall(Player::RIGHT, ball);
			}
			else
			{
				m_player->SetCatchBall(Player::LEFT, ball);
			}
		}
	}
}
