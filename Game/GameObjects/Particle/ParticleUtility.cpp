//--------------------------------------------------------------------------------------
// File: ParticleUtil.cpp
//
// パーティクルユーティリティクラス
// positionや速度などパーティクル１つに必要な要素を格納
// 更新時に生存時間や座標、大きさなどを再計算し、
// 寿命が尽きたらfalseを返して削除を促す
//
//-------------------------------------------------------------------------------------

#include "pch.h"
#include "Particleutility.h"

#include "Game/Commons/Collision.h"
#include "Game/GameObjects/Field/Field.h"

const static float ENDLESS = -100.0f;

/// <summary>
/// コンストラクタ
/// </summary>
/// <param name="life">生存時間</param>
/// <param name="pos">座標</param>
/// <param name="velocity">速度</param>
/// <param name="accele">加速度</param>
/// <param name="startScale">初期サイズ</param>
/// <param name="endScale">最終サイズ</param>
/// <param name="startColor">初期色</param>
/// <param name="endColor">最終色</param>
ParticleUtility::ParticleUtility(
	ID3D11DeviceContext* pContext, 
	float life,
	DirectX::SimpleMath::Vector3 pos,
	DirectX::SimpleMath::Vector3 velocity,
	DirectX::SimpleMath::Vector3 accele,
	DirectX::SimpleMath::Vector3 startScale, DirectX::SimpleMath::Vector3 endScale,
	DirectX::SimpleMath::Color startColor, DirectX::SimpleMath::Color endColor)
{
	//	与えられた生存時間（s）を取得
	m_startLife =
		m_life = life;

	//	初期位置を取得
	m_position = pos;
	//	移動ベクトルを取得
	m_velocity = velocity;
	//	加速度ベクトルを取得
	m_accele = accele;

	//	初めのサイズを取得
	m_startScale =
		m_nowScale = startScale;
	//	終了のサイズを取得
	m_endScale = endScale;


	//	初めの色を取得
	m_startColor =
		m_nowColor = startColor;
	//	終了の色を取得
	m_endColor = endColor;

	// コライダーの初期化
	m_collider.Initialize(pContext, m_position, m_startScale.x);
}



/// <summary>
/// デストラクタ
/// </summary>
ParticleUtility::~ParticleUtility()
{
}



/// <summary>
/// 更新関数
/// </summary>
/// <param name="timer">Game等からStepTimerを受け取る</param>
/// <returns>生存時間(life)がある間はtrue</returns>
bool ParticleUtility::Update(float elapsedTime)
{
	// 拡大率をLerpを使って算出する
	m_nowScale = DirectX::SimpleMath::Vector3::Lerp(m_startScale, m_endScale, 1.0f - m_life / m_startLife);
	// 色をLerpを使って算出する
	m_nowColor = DirectX::SimpleMath::Color::Lerp(m_startColor, m_endColor, 1.0f - m_life / m_startLife);
	// 速度の計算
	m_velocity += m_accele * elapsedTime;
	// 座標の計算
	m_position += m_velocity * elapsedTime;
	//	ライフを減らしていく
	m_life -= elapsedTime;

	// コライダーの設定
	m_collider.SetPosition(m_position);
	m_collider.SetRadius(m_nowScale.x);

	//	ライフが0未満なら自身を消してもらう
	if (m_life < 0.0f)
	{
		return false;
	}

	return true;
}



/// <summary>
/// 重なりの補填
/// </summary>
/// <param name="field"></param>
void ParticleUtility::CorrectOverlap(Field& field)
{
	// 差分を求める
	DirectX::SimpleMath::Vector3 delta = m_position - field.GetCollider().GetPosition();

	// 長さを求める
	float distance = delta.Length();
	float minDistance = m_collider.GetRadius() + field.GetCollider().GetRadius();

	// 差分を求める
	float pushLength = minDistance - distance;

	// 正規化
	delta.Normalize();

	// 押し出しする
	m_position += delta * pushLength;

	// 法線ベクトル
	DirectX::SimpleMath::Vector3 normalVec = m_accele * -1.0f;
	normalVec.Normalize();

	// 反射ベクトル
	DirectX::SimpleMath::Vector3 reflVec = m_velocity - 2.0f * (m_velocity.Dot(normalVec)) * normalVec;

	// 摩擦
	reflVec *= 0.6f;

	// 速度の設定
	m_velocity = reflVec;
}


