/// <summary>
/// Hittingに関するソースファイル
/// </summary>
/// <author>仲森智史</author>
/// <date>2025/05/21</date>

// ヘッダファイルの読み込み
#include "pch.h"
#include "Hitting.h"

#include "Common/DebugDraw.h"
#include "Game/Commons/Resources.h"
#include "Game/GameObjects/AirTarget/AirTarget.h"



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
	// エフェクトをつけていない
	m_isEffect = false;

	// 座標を設定していない
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
			m_pAirTarget->GetParticle()->SetEffectPosition(context, PARTICLE_LIFE, m_pAirTarget->GetPosition(), m_pAirTarget->GetGravity(), DirectX::Colors::Yellow);
		}

		// エフェクトを出した
		m_isEffect = true;
	}

	// ランダムに座標を設定する
	m_pAirTarget->RandomPosition();

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

