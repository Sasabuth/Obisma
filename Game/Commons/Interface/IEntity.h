/// <summary>
/// IEntityに関するヘッダファイル
/// </summary>
/// <author>仲森智史</author>

// 多重インクルードの防止
#pragma once

// ヘッダファイルの読み込み
#include "SimpleMath.h"


// クラスの定義
class SphereCollider;
class Field;


// クラスの定義
class IEntity
{
public:
	// デストラクタ
	virtual ~IEntity() = default;

	// 初期化
	virtual void Initialize(DirectX::SimpleMath::Vector3 position) = 0;

	// 更新
	virtual void Update(float elapsedTime) = 0;

	// 描画
	virtual void Render() = 0;

	// 終了処理
	virtual void Finalize() = 0;

	// 座標
	virtual void SetPosition(DirectX::SimpleMath::Vector3 position) = 0;  // 設定
	virtual const DirectX::SimpleMath::Vector3& GetPosition() = 0;		  // 取得

	// 速度
	virtual void SetVelocity(DirectX::SimpleMath::Vector3 velocity) = 0;  // 設定
	virtual const DirectX::SimpleMath::Vector3& GetVelocity() = 0;		  // 取得

	// 回転
	virtual void SetRotation(DirectX::SimpleMath::Quaternion rotation) = 0; // 設定
	virtual const DirectX::SimpleMath::Quaternion& GetRotation() = 0;		// 取得

	// 重力
	virtual void SetGravity(DirectX::SimpleMath::Vector3 gravity) = 0;  // 設定
	virtual const DirectX::SimpleMath::Vector3& GetGravity() = 0;       // 取得

	// 影の当たった座標
	virtual void SetShadowHitPos(DirectX::SimpleMath::Vector3 hitPos) = 0;  // 設定
	virtual const DirectX::SimpleMath::Vector3& GetShadowHitPos() = 0;      // 取得

	// 重なりの補填
	virtual void CorrectOverlap(DirectX::SimpleMath::Vector3& pos) = 0;


public:
	// コライダーの取得
	virtual SphereCollider& GetCollider() = 0;
};

