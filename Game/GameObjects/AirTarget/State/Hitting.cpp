/// <summary>
/// Hittingに関するソースファイル
/// </summary>
/// <author>仲森智史</author>
/// <date>2025/05/21</date>

// ヘッダファイルの読み込み
#include "pch.h"
#include "Hitting.h"

#include "Game/GameObjects/AirTarget/AirTarget.h"
#include "Game/GameObjects/Field/Field.h"
#include "DebugDraw.h"
#include "Game/Commons/Resources.h"




/// <summary>
/// コンストラクタ
/// </summary>
Hitting::Hitting(AirTarget* pAirTarget)
	: m_pAirTarget(pAirTarget)
	, m_isEffect(false)
	, m_isSetPosition(false)
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
	m_isEffect = false;

	m_isSetPosition = false;
}



/// <summary>
/// 更新処理
/// </summary>
/// <param name="elapsedTime">経過時間</param> 
void Hitting::Update(float elapsedTime)
{
	UNREFERENCED_PARAMETER(elapsedTime);

	// コンテキストの取得
	auto context = UserResources::GetUserResource()->GetDeviceResources()->GetD3DDeviceContext();

	// エフェクトがなかったら
	if (!m_isEffect)
	{
		// 指定数パーティクルを生成
		for (int i = 0; i < PARTICLE_COUNT; i++)
		{
			m_pAirTarget->GetParticle()->SetEffectPosition(context, PARTICLE_LIFE, m_pAirTarget->GetPosition(), m_pAirTarget->GetGravity());
		}

		// エフェクトを出した
		m_isEffect = true;
	}

	// ランダムで座標の取得
	if (!m_isSetPosition)
	{
		int index = -1;
		// 三角形のために3で割れる数にする
		while ((index + 3) % 3 != 0)
		{
			std::uniform_int_distribution<int> dist(0, (int)m_pAirTarget->GetField()->GetStageCollider().GetIndicesCount() - 1);
			std::mt19937 mt(m_rd());

			index = dist(mt);
		}

		// 三角形の中心を取得
		DirectX::SimpleMath::Vector3 center = m_pAirTarget->GetField()->GetStageCollider().GetCenterPosition(index);

		// 空中の的の設定
		m_pAirTarget->SetVelocity(DirectX::SimpleMath::Vector3::Zero);
		m_pAirTarget->SetPosition(center);
		m_pAirTarget->SetShadowHitPos(center);
		m_pAirTarget->SetGravity(m_pAirTarget->GetField()->CorrectUp(m_pAirTarget, m_pAirTarget->GetField()->GetStageCollider().GetNormalVector(index)));

		m_isSetPosition = true;
	}

	// 元の座標からY軸方向に高くして置く
	m_pAirTarget->SetPosition(m_pAirTarget->GetPosition() + DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitY, m_pAirTarget->GetRotation()) * 2);
	// コライダーの設定
	m_pAirTarget->GetCollider().SetPosition(m_pAirTarget->GetPosition());

	// ステート変更
	m_pAirTarget->ChangeState(m_pAirTarget->GetFloating());
}



/// <summary>
/// 描画処理
/// </summary>
void Hitting::Render()
{
}



/// <summary>
/// 終了処理
/// </summary>
void Hitting::Finalize()
{
}



/// <summary>
/// 特定のイベントの処理
/// </summary>
/// <param name="e">イベント</param>
void Hitting::EventHandle(Event e)
{
	UNREFERENCED_PARAMETER(e);
}
