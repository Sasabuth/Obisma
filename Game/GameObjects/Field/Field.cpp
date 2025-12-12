/// <summary>
/// Fieldに関するソースファイル
/// </summary>
/// <author>仲森智史</author>

// ヘッダファイルの読み込み
#include "pch.h"
#include "Field.h"

#include "Game/Commons/Interface/IEntity.h"
#include "Game/Commons/Resources.h"



/// <summary>
/// コンストラクタ
/// </summary>
Field::Field()
	: m_pUserResources(nullptr)
	, m_position{}
	, m_model{}
	, m_skydomeModel{}
	, m_rotate(0)
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
	m_pUserResources = UserResources::GetUserResource();
	auto device = m_pUserResources->GetDeviceResources()->GetD3DDevice();
	auto context = m_pUserResources->GetDeviceResources()->GetD3DDeviceContext();

	// モデルの設定
	m_model = Resources::GetInstance()->GetModel(L"Stage1.sdkmesh");
	m_model->UpdateEffects(
		// 引数にラムダ式として処理内容を指定する
		[&](DirectX::IEffect* pEffect)
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

	m_skydomeModel = Resources::GetInstance()->GetSkydome();

	// 座標の初期化
	m_position = DirectX::SimpleMath::Vector3{ 0.0f,0.0f,0.0f };

	// コライダーの初期化
	m_collider.Initialize(context, m_position, MODEL_SCALE);

	m_stageCollider.Initialize(device, context, m_model);
	m_stageCollider.SetPosition(m_position);
	m_stageCollider.SetScale(MODEL_SCALE);
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

	auto context = m_pUserResources->GetDeviceResources()->GetD3DDeviceContext();
	auto states = m_pUserResources->GetCommonStates();
	auto view = m_pUserResources->GetView();
	auto proj = m_pUserResources->GetProject();



	// ワールド座標
	DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_rotate)) * DirectX::SimpleMath::Matrix::CreateTranslation(m_position) * DirectX::SimpleMath::Matrix::CreateScale(MODEL_SCALE);

	// モデルの描画
	m_model->Draw(context, *states, world, *view, *proj);

	// スカイドームの描画
	DirectX::SimpleMath::Matrix sWorld = DirectX::SimpleMath::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_rotate / 4)) * DirectX::SimpleMath::Matrix::CreateTranslation(m_position) * DirectX::SimpleMath::Matrix::CreateScale(SKYDOME_SCALE);
	m_skydomeModel->Draw(context, *states, sWorld, *view, *proj);

	// デバック
	//m_stageCollider.Draw(context, *view, *proj);
}



/// <summary>
/// 終了処理
/// </summary>
void Field::Finalize()
{
}


DirectX::SimpleMath::Vector3 Field::CorrectUp(IEntity* iEntity)
{
	// 重力の方向
	DirectX::SimpleMath::Vector3 gravityDir = m_position - iEntity->GetPosition();
	gravityDir.Normalize();

	// 方向ベクトルの反転
	DirectX::SimpleMath::Vector3 targetUp;
	targetUp = -gravityDir;

	// 現在の姿勢制御
	DirectX::SimpleMath::Vector3 currentUp = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitY, iEntity->GetRotation());

	// 回転軸の計算
	DirectX::SimpleMath::Vector3 axis = currentUp.Cross(targetUp);
	axis.Normalize();

	// 回転角の計算
	float dot = currentUp.Dot(targetUp);
	float angle = acosf(dot);

	// クォータニオンの作成
	DirectX::SimpleMath::Quaternion q;


	// 角度が少しでもあれば軸を作る
	if (angle > 0.01f && axis.Length() > 0.01f)
	{
		q = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(axis, angle);
	}
	// なければ何もしない
	else 
	{
		q = DirectX::SimpleMath::Quaternion::Identity;
	}
	
	// 回転の設定
	iEntity->SetRotation(iEntity->GetRotation() * q);

	// 重力を返す
	return gravityDir * 3;
}

DirectX::SimpleMath::Vector3 Field::CorrectUp(IEntity* iEntity, DirectX::SimpleMath::Vector3 pos)
{
	// 重力の方向
	DirectX::SimpleMath::Vector3 gravityDir = -pos;
	gravityDir.Normalize();

	// 方向ベクトルの反転
	DirectX::SimpleMath::Vector3 targetUp;
	targetUp = -gravityDir;

	// 現在の姿勢制御
	DirectX::SimpleMath::Vector3 currentUp = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitY, iEntity->GetRotation());

	// 回転軸の計算
	DirectX::SimpleMath::Vector3 axis = currentUp.Cross(targetUp);
	axis.Normalize();

	// 回転角の計算
	float dot = currentUp.Dot(targetUp);
	float angle = acosf(dot);

	// クォータニオンの作成
	DirectX::SimpleMath::Quaternion q;


	// 角度が少しでもあれば軸を作る
	if (angle > 0.01f && axis.Length() > 0.01f)
	{
		q = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(axis, angle);
	}
	// なければ何もしない
	else
	{
		q = DirectX::SimpleMath::Quaternion::Identity;
	}

	// 回転の設定
	iEntity->SetRotation(iEntity->GetRotation() * q);

	// 重力を返す
	return gravityDir * 3;
}



/// <summary>
/// コライダーの取得
/// </summary>
/// <returns>コライダー</returns>
SphereCollider& Field::GetCollider()
{
	return m_collider;
}



