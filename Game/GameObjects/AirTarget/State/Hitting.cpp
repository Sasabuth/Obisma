/// <summary>
/// Hittingに関するソースファイル
/// </summary>
/// <author>仲森智史</author>
/// <date>2025/05/21</date>

// ヘッダファイルの読み込み
#include "pch.h"
#include "Hitting.h"

#include "Game/Scenes/GameplayScene.h"
#include "Game/GameObjects/Field/Field.h"
#include "DebugDraw.h"
#include "Game/Commons/Resources.h"
#include "Game/GameObjects/AirTarget/AirTarget.h"



/// <summary>
/// コンストラクタ
/// </summary>
Hitting::Hitting(AirTarget* airTarget)
	: m_airTarget(airTarget)
	, m_userResources(nullptr)
{
}



/// <summary>
/// デストラクタ
/// </summary>
Hitting::~Hitting()
{
}


/// <summary>
/// 初期化処理
/// </summary>
void Hitting::Initialize()
{
	// ユーザーリソースの取得
	m_userResources = UserResources::GetUserResource();
}



/// <summary>
/// 更新処理
/// </summary>
/// <param name="elapsedTime">経過時間</param> 
void Hitting::Update(float elapsedTime)
{
	UNREFERENCED_PARAMETER(elapsedTime);

	std::uniform_int_distribution<int> dist(0, Resources::GetInstance()->GetJson(L"AirTarget.json")["RandPosCount"] - 1);

	// プレイヤーの設定
	m_airTarget->SetVelocity(DirectX::SimpleMath::Vector3::Zero);
	m_airTarget->SetPosition(DirectX::SimpleMath::Vector3(
		Resources::GetInstance()->GetJson(L"AirTarget.json")["RandPos"][std::to_string(dist(rd))],
		Resources::GetInstance()->GetJson(L"AirTarget.json")["RandPos"][std::to_string(dist(rd))],
		Resources::GetInstance()->GetJson(L"AirTarget.json")["RandPos"][std::to_string(dist(rd))])
	);

	m_airTarget->GetCollider().SetPosition(m_airTarget->GetPosition());

	float pos = m_airTarget->GetPosition().x * m_airTarget->GetPosition().y * m_airTarget->GetPosition().z;
	if (pos < 23.0f && pos > 12.0f || pos < -12.0f && pos > -23.0f)
	{
		m_airTarget->ChangeState(m_airTarget->GetFloating());
	}
	
}



/// <summary>
/// 描画処理
/// </summary>
void Hitting::Render()
{
	//auto context = m_userResources->GetDeviceResources()->GetD3DDeviceContext();
	//auto states = m_userResources->GetCommonStates();
	//auto view = m_userResources->GetView();
	//auto proj = m_userResources->GetProject();

	//// ワールド座標
	//SimpleMath::Matrix world;

	//SimpleMath::Matrix pos = SimpleMath::Matrix::CreateTranslation(m_airTarget->GetPosition());
	//SimpleMath::Matrix scale = SimpleMath::Matrix::CreateScale(SimpleMath::Vector3(AirTarget::BALL_SIZE));

	//SimpleMath::Matrix rotate = SimpleMath::Matrix::CreateFromQuaternion(m_airTarget->GetRotation()); // ※回転順に合わせて調整

	//world = scale * rotate * pos;

	//// モデルの描画
	//m_airTarget->GetModel()->Draw(context, *states, world, *view, *proj);

	//// 影の描画
	//m_airTarget->DrawShadow(context, states, AirTarget::SHADOW_SIZE);

	// デバック
	// デバックフォントの描画
	/*auto* debugFont = m_userResources->GetDebugFont();*/
	//debugFont->Render(L"Hitting");
	//debugFont->Render(L"Position", m_airTarget->GetPosition());

}



/// <summary>
/// 終了処理
/// </summary>
void Hitting::Finalize()
{
}