/// <summary>
/// Standingに関するソースファイル
/// </summary>
/// <author>仲森智史</author>

// ヘッダファイルの読み込み
#include "pch.h"
#include "Standing.h"

#include "Game/GameObjects/Player/Player.h"
#include "Game/GameObjects/Field/Field.h"
#include "Game/GameObjects/Camera/Camera.h"
#include "DebugDraw.h"
#include "Game/Commons/Resources.h"



/// <summary>
/// コンストラクタ
/// </summary>
Standing::Standing(Player* pPlayer)
	: m_pPlayer(pPlayer)
	, m_pUserResources(nullptr)
	, m_model{}
{
	// モデルの作成
	m_model = Resources::GetInstance()->GetPlayerModel();

	// AnimationSDKMESH クラスのインスタンスを生成する
	m_animation = std::make_unique<DX::AnimationSDKMESH>();
	// アニメーションをロードする	
	m_animation->Load(L"Resources/Animations/Player_Idle.sdkmesh_anim");
	// アニメーションとモデルをバインドする
	m_animation->Bind(*m_model);
	// ボーン用のトランスフォーム配列を生成する
	m_drawBones = DirectX::ModelBone::MakeArray(m_model->bones.size());
	//ZeroMemory(m_drawBones.get(), sizeof(DirectX::ModelBone) * m_model->bones.size());

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
void Standing::Update(float elapsedTime)
{
	auto mouse = DirectX::Mouse::Get().GetState();

	// プロジェクション行列
	auto proj = m_pUserResources->GetProject();
	auto view = m_pUserResources->GetView();

	// アニメーションの更新
	AnimationUpdate(elapsedTime);

	// レイの設定
	auto const r = m_pUserResources->GetDeviceResources()->GetOutputSize();
	m_pPlayer->SetMouseRay(m_pPlayer->CreatePickingRay(mouse.x, mouse.y, r.right, r.bottom, *view, *proj));

	// マウス方向の回転の更新
	UpdateRotateToMouse();

	// ボールをキャッチする
	CatchHandBall();

	// ボールを手に持たせる
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

	// スコアを下げる
	m_pPlayer->ScoreDown();

	// プレイヤーの設定
	m_pPlayer->SetVelocity(m_pPlayer->GetGravity());
	m_pPlayer->SetPosition(m_pPlayer->GetPosition() + m_pPlayer->GetVelocity() * elapsedTime);
	m_pPlayer->GetCollider().SetPosition(m_pPlayer->GetPosition());
}



/// <summary>
/// 描画処理
/// </summary>
void Standing::Render()
{
	auto context = m_pUserResources->GetDeviceResources()->GetD3DDeviceContext();
	auto states = m_pUserResources->GetCommonStates();
	auto view = m_pUserResources->GetView();
	auto proj = m_pUserResources->GetProject();

	/*if (m_pPlayer->CalcRaySphere(m_pPlayer->GetMouseRay().position, m_pPlayer->GetMouseRay().direction, m_pPlayer->GetAirTarget()->GetPosition(), m_pPlayer->GetAirTarget()->GetCollider().GetRadius(), m_pPlayer->GetHitPos()))
	{
		m_pPlayer->DrawLockOn(m_pPlayer->GetAirTarget()->GetPosition());
	}*/

	// ワールド座標
	DirectX::SimpleMath::Matrix pos = DirectX::SimpleMath::Matrix::CreateTranslation(m_pPlayer->GetPosition());
	DirectX::SimpleMath::Matrix scale = DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(Resources::GetInstance()->GetJson(L"Player.json")["PlayerSize"]));
	DirectX::SimpleMath::Matrix rotate = DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_pPlayer->GetRotation());

	m_pPlayer->SetWorld(scale * rotate * pos);

	// 無敵時間中なら点滅描画
	if (m_pPlayer->GetInvincibleTime() >= 0.0 && sinf(m_pPlayer->GetInvincibleTime() * 10) <= 0.0f)
	{
		return;
	}

	// ボーン数を取得
	size_t nbones = m_model->bones.size();
	// アニメションにモデルを適用する
	m_animation->Apply(*m_model, m_model->bones.size(), m_drawBones.get());
	// スキン変形用行列を適用する(これを実行しないとアニメーションが崩れる)
	m_animation->ApplySkinMatrix(*m_model, nbones, m_drawBones.get());

	// アニメーションモデルを描画
	m_model->DrawSkinned(
		context,
		*states, nbones,
		m_drawBones.get(),
		m_pPlayer->GetWorld(),
		*view,
		*proj
	);

	// 影の描画
	m_pPlayer->DrawShadow(context, states, Resources::GetInstance()->GetJson(L"Player.json")["ShadowSize"]);

	// デバック
	/*m_model->Draw(context, *states, world, *view, *proj);*/

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
	//DirectX::SimpleMath::Vector3 vertical = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3(0.0f, 5.0f, 0.0f), m_pPlayer->GetRotation());

	//m_primitiveBatch->Begin();
	//DX::DrawRay(m_primitiveBatch.get(), m_pPlayer->GetPosition(), forward, false, DirectX::Colors::Yellow);
	//DX::DrawRay(m_primitiveBatch.get(), m_pPlayer->GetPosition(), horizontal, false, DirectX::Colors::Red);
	//DX::DrawRay(m_primitiveBatch.get(), m_pPlayer->GetPosition(), -vertical, false, DirectX::Colors::Green);
	////DX::DrawRay(m_primitiveBatch.get(), m_pPlayer->GetMouseRayHitPos(), DirectX::SimpleMath::Vector3::UnitY, false, DirectX::Colors::White);
	//m_primitiveBatch->End();

	/*auto* debugFont = m_pUserResources->GetDebugFont();

	debugFont->Render(L"Standing");*/
}



/// <summary>
/// 終了処理
/// </summary>
void Standing::Finalize()
{
}



/// <summary>
/// 特定のイベントの処理
/// </summary>
/// <param name="e">イベント</param>
void Standing::EventHandle(Event e)
{
	switch (e)
	{
	// 走る
	case IState::Event::RUN:
		// ステート変更
		m_pPlayer->ChangeState(m_pPlayer->GetRunning());
		break;

	// 投げる
	case IState::Event::THROW:
		// ボールを投げる
		ThrowBall();
		break;

	// 捕る
	case IState::Event::CATCH:
		// ステート変更
		m_pPlayer->ChangeState(m_pPlayer->GetCatching());
		break;
	}
}



/// <summary>
/// アニメーションの更新
/// </summary>
/// <param name="elapsedTime">経過時間</param>
void Standing::AnimationUpdate(float elapsedTime)
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

	// 手の行列の設定
	m_rightHandMatrix = m_drawBones[15];
	m_leftHandMatrix = m_drawBones[20];
}



/// <summary>
/// ボールを投げる
/// </summary>
void Standing::ThrowBall()
{
	// 左手に持っていたら投げる
	if (m_pPlayer->GetCatchBall(Player::LEFT))
	{
		Ball* ball = m_pPlayer->GetCatchBall(Player::LEFT);
		m_pPlayer->SetBallPosition(ball, m_leftHandMatrix);
		m_pPlayer->ChangeState(m_pPlayer->GetThrowingL());
		return;
	}
	// 右手に持っていたら投げる
	if (m_pPlayer->GetCatchBall(Player::RIGHT))
	{
		Ball* ball = m_pPlayer->GetCatchBall(Player::RIGHT);
		m_pPlayer->SetBallPosition(ball, m_rightHandMatrix);
		m_pPlayer->ChangeState(m_pPlayer->GetThrowingR());
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

	// 両方当たっていた場合どちらが先に当たったか調べる
	hitPos2 = DirectX::SimpleMath::Vector3(10000);

	// 空中の的の取得
	AirTarget* airTarget = m_pPlayer->GetField()->GetAirTarget();

	// カメラの取得
	Camera* camera = m_pPlayer->GetField()->GetCamera();

	// ワールド座標
	DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::CreateScale(m_pPlayer->GetField()->GetStageCollider().GetScale()) *
		DirectX::SimpleMath::Matrix::CreateTranslation(m_pPlayer->GetField()->GetStageCollider().GetPosition());

	// フィールドの三角形の数分回す
	for (size_t i = 0; i + 2 < m_pPlayer->GetField()->GetStageCollider().GetIndicesCount(); i += 3)
	{
		// マウスレイと三角が当たっているかを調べる
		DirectX::SimpleMath::Vector3 pos;
		if (IsHit(m_pPlayer->GetMouseRay().position, m_pPlayer->GetMouseRay().direction, world, m_pPlayer->GetField()->GetStageCollider(), (int)i, pos))
		{
			// 前と今の当たった座標の長さを調べる
			DirectX::SimpleMath::Vector3 d0 = camera->GetEyePosition() - hitPos2;
			DirectX::SimpleMath::Vector3 d1 = camera->GetEyePosition() - pos;

			// 今のほうが短かったら座標を入れる
			if (d0.Length() > d1.Length())
			{
				hitPos2 = pos;
			}
		}
	}

	// 空中の的と三角形に当たっていたら
	if (m_pPlayer->CalcRaySphere(airTarget->GetPosition(), airTarget->GetCollider().GetRadius(), hitPos1) &&
		hitPos2 != DirectX::SimpleMath::Vector3(10000))
	{
		// どちらのほうが短いか調べる
		DirectX::SimpleMath::Vector3 a;
		DirectX::SimpleMath::Vector3 b;

		a = m_pPlayer->GetMouseRay().position - hitPos1;
		b = m_pPlayer->GetMouseRay().position - hitPos2;

		// 空中の的のほうが短かったらロックオンを出す
		if (a.Length() < b.Length())
		{
			m_pPlayer->SetMouseRayHitPos(airTarget->GetPosition());
			m_pPlayer->SetIsLockOn(true);
		}
		// 違ったらロックオンを出さない
		else
		{
			m_pPlayer->SetMouseRayHitPos(hitPos2);
			m_pPlayer->SetIsLockOn(false);
		}

		// マウス方向に回転
		m_pPlayer->RotateToMouse();
	}
	// 違ったら
	else
	{
		// 空中の的に当たっていたらロックオンを出す
		if (m_pPlayer->CalcRaySphere(airTarget->GetPosition(), airTarget->GetCollider().GetRadius(), m_pPlayer->GetMouseRayHitPos()))
		{
			m_pPlayer->SetIsLockOn(true);
			// マウスレイの当たった座標の設定
			m_pPlayer->SetMouseRayHitPos(airTarget->GetPosition());
			// マウス方向に回転
			m_pPlayer->RotateToMouse();
		}
		// 三角形に当たっていたらロックオンを出さない
		else if (hitPos2 != DirectX::SimpleMath::Vector3(10000))
		{
			m_pPlayer->SetIsLockOn(false);
			// マウスレイの当たった座標の設定
			m_pPlayer->SetMouseRayHitPos(hitPos2);
			// マウス方向に回転
			m_pPlayer->RotateToMouse();
			
		}
		// 当たっていない時は何もしない
		else
		{
			m_pPlayer->SetMouseRayHitPos(DirectX::SimpleMath::Vector3::Zero);
			m_pPlayer->SetIsLockOn(false);
		}
	}
}



/// <summary>
/// ボールを持つ
/// </summary>
void Standing::CatchHandBall()
{
	// ボールマネージャーの取得
	BallManager* ballManager = m_pPlayer->GetField()->GetBallManager();

	for (int i = 0; i < ballManager->GetObjectCount(); i++)
	{
		// 両手に持っていたら終了
		if (m_pPlayer->GetCatchBall(Player::RIGHT) && m_pPlayer->GetCatchBall(Player::LEFT))
		{
			return;
		}

		Ball* ball = ballManager->GetBall(i);
		if (IsHit(m_pPlayer->GetCollider(), ball->GetCollider()) && ball->GetCurrentState() == ball->GetStopping())
		{
			// ボールの状態の変更
			ball->ChangeState(ball->GetCatching());

			// 色を変更する
			ball->SetBallColorNum(Ball::BallColor::PLAYER);

			if (!m_pPlayer->GetCatchBall(Player::RIGHT))
			{
				m_pPlayer->SetCatchBall(Player::RIGHT, ball);
			}
			else
			{
				m_pPlayer->SetCatchBall(Player::LEFT, ball);
			}
		}
	}
}
