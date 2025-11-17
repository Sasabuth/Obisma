/// <summary>
/// Hittingに関するソースファイル
/// </summary>
/// <author>仲森智史</author>
/// <date>2025/05/21</date>

// ヘッダファイルの読み込み
#include "pch.h"
#include "Hitting.h"

#include "Game/GameObjects/AirTarget/AirTarget.h"
#include "DebugDraw.h"
#include "Game/Commons/Resources.h"




/// <summary>
/// コンストラクタ
/// </summary>
Hitting::Hitting(AirTarget* pAirTarget)
	: m_pAirTarget(pAirTarget)
	, m_isEffect(false)
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
	std::uniform_int_distribution<int> dist(0, Resources::GetInstance()->GetJson(L"AirTarget.json")["RandPosCount"] - 1);
	std::mt19937 mt(m_rd());

	// ランダムで空中の的の座標の設定
	m_pAirTarget->SetVelocity(DirectX::SimpleMath::Vector3::Zero);
	m_pAirTarget->SetPosition(DirectX::SimpleMath::Vector3(
		Resources::GetInstance()->GetJson(L"AirTarget.json")["RandPos"][std::to_string(dist(mt))],
		Resources::GetInstance()->GetJson(L"AirTarget.json")["RandPos"][std::to_string(dist(mt))],
		Resources::GetInstance()->GetJson(L"AirTarget.json")["RandPos"][std::to_string(dist(mt))])
	);

	// コライダーの設定
	m_pAirTarget->GetCollider().SetPosition(m_pAirTarget->GetPosition());

	// 座標成分の合計の計算
	float pos = m_pAirTarget->GetPosition().x * m_pAirTarget->GetPosition().y * m_pAirTarget->GetPosition().z;

	// 20.0fか16.0fの範囲外なら
	if ((std::fabs(pos) <= 19.9f || std::fabs(pos) >= 20.1f) &&
		(std::fabs(pos) <= 15.9f || std::fabs(pos) >= 16.1f))
	{
		// 高さが当たる距離だったらステート変更
		if (std::fabs(pos) < 22.0f && std::fabs(pos) > 12.0f)
		{
			m_pAirTarget->ChangeState(m_pAirTarget->GetFloating());
		}
	}


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