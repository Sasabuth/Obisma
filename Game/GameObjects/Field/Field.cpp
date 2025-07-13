/// <summary>
/// Fieldに関するソースファイル
/// </summary>
/// <author>仲森智史</author>
/// <date></date>

// ヘッダファイルの読み込み
#include "pch.h"
#include "Field.h"
#include "Game/Scenes/GameplayScene.h"
#include "Game/GameObjects//Camera/Camera.h"
#include "Game/Commons/Interface/IEntity.h"
#include "Game/Commons/Resources.h"

// 名前の省略
using namespace DirectX;

/// <summary>
/// コンストラクタ
/// </summary>
Field::Field(GameplayScene* pScene)
	: m_pScene(pScene)
	, m_userResource(nullptr)
	, m_position{}
	, m_model{}
{
}



/// <summary>
/// デストラクタ
/// </summary>
Field::~Field()
{
}


/// <summary>
/// 初期化処理
/// </summary>
void Field::Initialize()
{
	// ユーザーリソースの取得
	m_userResource = UserResources::GetUserResource();
	auto device = m_userResource->GetDeviceResources()->GetD3DDevice();
	auto context = m_userResource->GetDeviceResources()->GetD3DDeviceContext();

	// モデルの設定
	m_model = Resources::GetInstance()->GetFieldModel();

	m_model->UpdateEffects(
		// 引数にラムダ式として処理内容を指定する
		[&](IEffect* pEffect)
		{
			// BasicEffectにキャストする
			DirectX::BasicEffect* pBasicEffect = dynamic_cast<DirectX::BasicEffect*>(pEffect);

			//// ライトをオフにする
			pBasicEffect->SetLightEnabled(0, false);
			pBasicEffect->SetLightEnabled(1, false);
			pBasicEffect->SetLightEnabled(2, false);

			// 自己発光(引数はカラー)
			pBasicEffect->SetEmissiveColor(DirectX::SimpleMath::Vector3(1, 1, 1));
		}
	);

	// 座標の初期化
	m_position = SimpleMath::Vector3{ 0.0f,0.0f,0.0f };

	// コライダーの初期化
	m_collider.Initialize(context, m_position, MODEL_SCALE);
}



/// <summary>
/// 更新処理
/// </summary>
/// <param name="elapsedTime"></param> 経過時間
void Field::Update(float elapsedTime)
{
	UNREFERENCED_PARAMETER(elapsedTime);

	// コライダーの設定
	m_collider.SetPosition(m_position);
}



/// <summary>
/// 描画処理
/// </summary>
void Field::Render()
{
	// デバックフォントの描画
	/*auto* debugFont = m_pUserResource->GetDebugFont();*/

	auto context = m_userResource->GetDeviceResources()->GetD3DDeviceContext();
	auto states = m_userResource->GetCommonStates();
	auto view = m_userResource->GetView();
	auto proj = m_userResource->GetProject();

	// ワールド座標
	SimpleMath::Matrix world = SimpleMath::Matrix::CreateTranslation(m_position) * SimpleMath::Matrix::CreateScale(MODEL_SCALE);

	// モデルの描画
	m_model->Draw(context, *states, world, *view, *proj);

	// デバック用
	/*m_collider.Draw(states, *view, *proj);*/
}



/// <summary>
/// 終了処理
/// </summary>
void Field::Finalize()
{
}


SimpleMath::Vector3 Field::CorrectUp(IEntity* iEntity)
{
	using namespace DirectX::SimpleMath;

	// 重力の方向
	Vector3 gravityDir = m_position - iEntity->GetPosition();
	gravityDir.Normalize();

	// 方向ベクトルの反転
	Vector3 targetUp;
	targetUp = -gravityDir;

	// 現在の姿勢制御
	Vector3 currentUp = Vector3::Transform(Vector3::UnitY, iEntity->GetRotation());

	// 回転軸の計算
	SimpleMath::Vector3 axis = currentUp.Cross(targetUp);
	axis.Normalize();

	// 回転角の計算
	float dot = currentUp.Dot(targetUp);
	float angle = acosf(dot);

	// クォータニオンの作成
	Quaternion q;


	// 角度が少しでもあれば軸を作る
	if (angle > 0.01f) 
	{
		q = Quaternion::CreateFromAxisAngle(axis, angle);
	}
	// なければ何もしない
	else 
	{
		q = Quaternion::Identity;
	}



	
	// 回転の設定
	iEntity->SetRotation(iEntity->GetRotation() * q);

	// 重力の設定
	/*iEntity->SetGravity(gravityDir);*/

	// 速度
	return gravityDir * 2;
}



SphereCollider& Field::GetCollider()
{
	return m_collider;
}



