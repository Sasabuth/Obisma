//--------------------------------------------------------------------------------------
// File: ParticleUtil.h
//
// パーティクルユーティリティクラス
//
//-------------------------------------------------------------------------------------

#pragma once

#include "StepTimer.h"
#include <DeviceResources.h>
#include <SimpleMath.h>
#include <Effects.h>
#include <PrimitiveBatch.h>
#include <VertexTypes.h>
#include <WICTextureLoader.h>
#include <CommonStates.h>
#include <vector>
#include "Game/Commons/Collision.h"

class Field;


class ParticleUtility
{
private:


//	変数
private:
	//	座標
	DirectX::SimpleMath::Vector3 m_position;

	//	速度
	DirectX::SimpleMath::Vector3 m_velocity;
	//	加速度
	DirectX::SimpleMath::Vector3 m_accele;
	// コライダー
	SphereCollider m_collider;

	//	スケール
	DirectX::SimpleMath::Vector3 m_nowScale;
	DirectX::SimpleMath::Vector3 m_startScale;
	DirectX::SimpleMath::Vector3 m_endScale;

	//	生存時間
	float m_life;
	float m_startLife;

	//	カラー
	DirectX::SimpleMath::Color m_nowColor;
	DirectX::SimpleMath::Color m_startColor;
	DirectX::SimpleMath::Color m_endColor;

//	関数
public:
	//	コンストラクタ（初期設定）
	ParticleUtility(
		ID3D11DeviceContext* pContext,
		float life,
		DirectX::SimpleMath::Vector3 pos,
		DirectX::SimpleMath::Vector3 velocity,
		DirectX::SimpleMath::Vector3 accele,
		DirectX::SimpleMath::Vector3 startScale, DirectX::SimpleMath::Vector3 endScale,
		DirectX::SimpleMath::Color startColor, DirectX::SimpleMath::Color endColor);
	//	デストラクタ
	~ParticleUtility();

	//	更新
	bool Update(float elapsedTime);

	// 重なりの補填
	void CorrectOverlap(Field& field);
	void CorrectOverlap(DirectX::SimpleMath::Vector3& pos);


// 設定/取得
public:
	// 座標の取得
	const DirectX::SimpleMath::Vector3 GetPosition() { return m_position; }

	// 速度の取得
	const DirectX::SimpleMath::Vector3 GetVelocity() { return m_velocity; }

	// 加速度の取得
	const DirectX::SimpleMath::Vector3 GetAccele() { return m_accele; }
	
	// 
	const DirectX::SimpleMath::Vector3 GetNowScale() { return m_nowScale; }
	const DirectX::SimpleMath::Vector3 GetStartScale() { return m_startScale; }
	const DirectX::SimpleMath::Vector3 GetEndScale() { return m_endScale; }

	const float GetLife() { return m_life; }
	const float GetStartLife() { return m_startLife; }
	const DirectX::SimpleMath::Color GetNowColor() { return m_nowColor; }
	const DirectX::SimpleMath::Color GetStartColor() { return m_startColor; }
	const DirectX::SimpleMath::Color GetEndColor() { return m_endColor; }

	// コライダー
	SphereCollider& GetCollider() { return m_collider; }

// 内部処理
private:


};

