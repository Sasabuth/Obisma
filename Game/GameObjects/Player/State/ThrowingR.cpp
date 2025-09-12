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


// 名前の省略
using namespace DirectX;


/// <summary>
/// コンストラクタ
/// </summary>
ThrowingR::ThrowingR(Player* player)
	: m_player(player)
	, m_userResources(nullptr)
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
	m_userResources = UserResources::GetUserResource();

	auto device = m_userResources->GetDeviceResources()->GetD3DDevice();
	auto context = m_userResources->GetDeviceResources()->GetD3DDeviceContext();

	m_worldMatrix = SimpleMath::Matrix::Identity;

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
	CreateInputLayoutFromEffect<DirectX::VertexPositionColor>(device, m_basicEffect.get(), m_inputLayout.ReleaseAndGetAddressOf());

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

	auto kb = Keyboard::Get().GetState();
	auto mouseTK = m_userResources->GetMouseStateTracker();

	// 投げていなかったら手に持たせる
	if (!m_isThowing)
	{
		// 方向
		SimpleMath::Vector3 dir = m_player->GetPosition() - m_player->GetHitPos();
		dir.Normalize();

		// 方向ベクトルの反転
		SimpleMath::Vector3 targetUp;
		targetUp = -dir;

		// 現在の姿勢制御
		SimpleMath::Vector3 currentUp = SimpleMath::Vector3::Transform(SimpleMath::Vector3::UnitX, m_player->GetRotation());

		// 回転軸の計算
		SimpleMath::Vector3 axis = currentUp.Cross(targetUp);
		axis.Normalize();

		// 回転角の計算
		float dot = currentUp.Dot(targetUp);
		float angle = acosf(dot);

		// クォータニオンの作成
		SimpleMath::Quaternion q;

		// 角度が少しでもあれば軸を作る
		if (angle > 0.01f)
		{
			q = SimpleMath::Quaternion::CreateFromAxisAngle(axis, angle);
		}
		// なければ何もしない
		else
		{
			q = SimpleMath::Quaternion::Identity;
		}

		m_player->SetRotation(m_player->GetRotation() * q);

		// 右手に持たせる
		Ball* ball = m_player->GetCatchBall(Player::RIGHT);
		SetBallPosition(ball, m_rightHandMatrix);
		
		// 時間になったら投げる
		if (m_animation->GetAnimTime() > 0.6f)
		{
			ball->ChangeState(ball->GetMoving());
			SimpleMath::Vector3 forward = SimpleMath::Vector3::Transform(SimpleMath::Vector3::UnitZ, m_player->GetRotation());
			SimpleMath::Quaternion rotate = SimpleMath::Quaternion::CreateFromAxisAngle(forward, XMConvertToRadians(15));
			ball->SetSpeed(SimpleMath::Vector3::Transform(SimpleMath::Vector3::UnitX, m_player->GetRotation() * rotate) * Player::PLAYER_POWER);
			m_player->SetCatchBall(Player::RIGHT, nullptr);
			m_isThowing = true;
		}
	}
	
	// スコアを下げる
	m_player->ScoreDown();

	// プレイヤーの設定
	m_player->SetVelocity(m_player->GetGravity());
	m_player->SetPosition(m_player->GetPosition() + m_player->GetVelocity() * elapsedTime);
	m_player->GetCollider().SetPosition(m_player->GetPosition());

	// アニメーションを更新し終了したらステート変更
	if (m_animation->GetAnimTime() < m_animation->GetEndTime())
	{
		// 左手に持たせる
		Ball* ball = m_player->GetCatchBall(Player::LEFT);
		if(ball) SetBallPosition(ball, m_leftHandMatrix);

		// アニメーションを更新する
		m_animation->Update(elapsedTime);
	}
	else
	{
		if (kb.W) m_player->ChangeState(m_player->GetRunning());
		else if (mouseTK->rightButton == mouseTK->PRESSED)
		{
			m_player->ChangeState(m_player->GetCatching());
		}
		else m_player->ChangeState(m_player->GetStanding());
	}

	// アニメーションの更新
	AnimationUpdate();

}



/// <summary>
/// 描画処理
/// </summary>
void ThrowingR::Render()
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
	SimpleMath::Matrix rotate = SimpleMath::Matrix::CreateFromQuaternion(m_player->GetRotation());

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

	// 影の描画
	SimpleMath::Vector3 m_drawPos;
	m_player->DrawShadow(context, states, Player::SHADOW_SIZE, m_drawPos);

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

	SimpleMath::Vector3 dir = SimpleMath::Vector3::Transform(SimpleMath::Vector3::UnitZ, m_player->GetRotation());
	SimpleMath::Quaternion rot = SimpleMath::Quaternion::CreateFromAxisAngle(forward, XMConvertToRadians(15));
	SimpleMath::Vector3 horizontal = SimpleMath::Vector3::Transform(SimpleMath::Vector3(1.0f, 0.0f, 0.0f), m_player->GetRotation() * rot);

	SimpleMath::Vector3 vertical = SimpleMath::Vector3::Transform(SimpleMath::Vector3(0.0f, 1.0f, 0.0f), m_player->GetRotation());

	/*m_primitiveBatch->Begin();
	DX::DrawRay(m_primitiveBatch.get(), m_player->GetPosition(), forward, false, DirectX::Colors::Yellow);
	DX::DrawRay(m_primitiveBatch.get(), m_player->GetPosition(), horizontal, false, DirectX::Colors::Red);
	DX::DrawRay(m_primitiveBatch.get(), m_player->GetPosition(), vertical, false, DirectX::Colors::Green);
	m_primitiveBatch->End();*/

	//debugFont->Render(L"ThrowingR");
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



/// <summary>
/// ボールの座標の設定
/// </summary>
/// <param name="ball">ボールのポインタ</param>
/// <param name="handMatrix">手のマトリックス</param>
void ThrowingR::SetBallPosition(Ball* ball, DirectX::SimpleMath::Matrix handMatrix)
{
	// ボーンに設定した境界球のワールド計算を行う
	DirectX::SimpleMath::Matrix sphereMatrix = handMatrix * m_worldMatrix;
	// バウンディングスフィアの中心点を設定する
	SimpleMath::Vector3 dir = SimpleMath::Vector3(sphereMatrix._41, sphereMatrix._42, sphereMatrix._43);
	dir.Normalize();
	ball->SetPosition(SimpleMath::Vector3(dir.x * 3.2f, dir.y * 3.2f, dir.z * 3.2f));
}
