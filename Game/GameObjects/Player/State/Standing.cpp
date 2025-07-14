/// <summary>
/// Standingに関するソースファイル
/// </summary>
/// <author>仲森智史</author>
/// <date>2025/05/21</date>

// ヘッダファイルの読み込み
#include "pch.h"
#include "Standing.h"

#include "Game/Scenes/GameplayScene.h"
#include "Game/GameObjects/Field/Field.h"
#include "DebugDraw.h"
#include "Game/Commons/Resources.h"


// 名前の省略
using namespace DirectX;


/// <summary>
/// コンストラクタ
/// </summary>
Standing::Standing(Player* player)
	: m_player(player)
	, m_userResources(nullptr)
	, m_model{}
{
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

	m_model = Resources::GetInstance()->GetPlayerModel();

	m_worldMatrix = SimpleMath::Matrix::Identity;

	// AnimationSDKMESH クラスのインスタンスを生成する
	m_animation = std::make_unique<DX::AnimationSDKMESH>();
	// サッカープレイヤー アイドリングアニメーションをロードする
	m_animation->Load(L"resources\\Animations\\Player_Idle.sdkmesh_anim");
	// アニメーションとモデルをバインドする
	m_animation->Bind(*m_model);
	// ボーン用のトランスフォーム配列を生成する
	m_drawBones = DirectX::ModelBone::MakeArray(m_model->bones.size());
	ZeroMemory(m_drawBones.get(), sizeof(DirectX::ModelBone) * m_model->bones.size());
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
	CreateInputLayoutFromEffect<DirectX::VertexPositionColor>(device, m_basicEffect.get(), m_inputLayout.ReleaseAndGetAddressOf());
}



/// <summary>
/// 更新処理
/// </summary>
/// <param name="elapsedTime">経過時間</param> 
void Standing::Update(float elapsedTime)
{
	UNREFERENCED_PARAMETER(elapsedTime);

	auto kbTracker = m_userResources->GetKeyboardStateTracker();
	auto mouse = Mouse::Get().GetState();
	auto mouseTK = m_userResources->GetMouseStateTracker();

	// プロジェクション行列
	auto proj = m_userResources->GetProject();
	auto view = m_userResources->GetView();

	// レイの設定
	auto const r = m_userResources->GetDeviceResources()->GetOutputSize();
	m_player->SetMouseRay(m_player->CreatePickingRay(mouse.x, mouse.y, r.right, r.bottom, *view, *proj));

	// マウスの方向に回転
	if (m_player->CalcRaySphere(m_player->GetMouseRay().position, m_player->GetMouseRay().direction, m_player->GetScene()->GetField().GetCollider().GetPosition(), m_player->GetScene()->GetField().GetCollider().GetRadius(), m_player->GetHitPos()))
	{
		m_player->RotateToMouse();
	}



	// ステートの変更
	if (kbTracker->pressed.W || kbTracker->pressed.A || kbTracker->pressed.S || kbTracker->pressed.D)
	{
		m_player->ChangeState(m_player->GetRunning());
	}

	// アニメーションの更新
	AnimationUpdate(elapsedTime);

	if (IsHit(m_player->GetCollider(), m_player->GetScene()->GetBall().GetCollider()) && m_player->GetScene()->GetBall().GetCurrentState() != m_player->GetScene()->GetBall().GetMoving())
	{
		Ball& ball = m_player->GetScene()->GetBall();
		ball.ChangeState(ball.GetCatching());

		// ボーンに設定した境界球のワールド計算を行う
		DirectX::SimpleMath::Matrix sphereMatrix = m_boneMatrix * m_worldMatrix;
		// バウンディングスフィアの中心点を設定する
		SimpleMath::Vector3 dir = SimpleMath::Vector3(sphereMatrix._41, sphereMatrix._42, sphereMatrix._43);
		dir.Normalize();
		ball.SetPosition(SimpleMath::Vector3(dir.x * 3.2f, dir.y * 3.2f, dir.z * 3.2f));

		if (mouseTK->leftButton)
		{
			Ball& ball = m_player->GetScene()->GetBall();

			ball.ChangeState(ball.GetMoving());
			ball.SetSpeed(SimpleMath::Vector3::Transform(SimpleMath::Vector3::UnitX, m_player->GetRotation()));
		}
	}

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

	// ワールド座標
	SimpleMath::Matrix pos = SimpleMath::Matrix::CreateTranslation(m_player->GetPosition());
	SimpleMath::Matrix scale = SimpleMath::Matrix::CreateScale(SimpleMath::Vector3(Player::PLAYER_SIZE));

	SimpleMath::Matrix rotate = SimpleMath::Matrix::CreateFromQuaternion(m_player->GetRotation()); // ※回転順に合わせて調整

	m_worldMatrix = scale * rotate * pos;

	// ① ボーン数を取得する
	size_t nbones = m_model->bones.size();
	// ④ アニメーションモデルを描画する
	m_model->DrawSkinned(
		context,
		*states, nbones,
		m_drawBones.get(),
		m_worldMatrix,
		*view,
		*proj
	);

	/*m_model->Draw(context, *states, world, *view, *proj);*/

	// デバック

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

	SimpleMath::Vector3 forward = SimpleMath::Vector3::Transform(SimpleMath::Vector3(0.0f, 0.0f, 1.0f), m_player->GetRotation());
	SimpleMath::Vector3 horizontal = SimpleMath::Vector3::Transform(SimpleMath::Vector3(1.0f, 0.0f, 0.0f), m_player->GetRotation());
	SimpleMath::Vector3 vertical = SimpleMath::Vector3::Transform(SimpleMath::Vector3(0.0f, 1.0f, 0.0f), m_player->GetRotation());

	m_primitiveBatch->Begin();
	DX::DrawRay(m_primitiveBatch.get(), m_player->GetPosition(), forward, false, DirectX::Colors::Yellow);
	DX::DrawRay(m_primitiveBatch.get(), m_player->GetPosition(), horizontal, false, DirectX::Colors::Red);
	DX::DrawRay(m_primitiveBatch.get(), m_player->GetPosition(), vertical, false, DirectX::Colors::Green);
	m_primitiveBatch->End();

	debugFont->Render(L"Standing");
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
	// ボーンマトリクスを設定する
	m_boneMatrix = m_drawBones[15];
	// スキン変形用行列を適用する(これを実行しないとアニメーションが崩れる)
	m_animation->ApplySkinMatrix(*m_model, nbones, m_drawBones.get());
}
