#pragma once

#include "SimpleMath.h"

class SphereCollider;
class Field;

class IEntity
{
public:
	// デストラクタ
	virtual ~IEntity() = default;

	// 初期化
	virtual void Initialize() = 0;

	// 更新
	virtual void Update(float elapsedTime) = 0;

	// 描画
	virtual void Render() = 0;

	// 終了処理
	virtual void Finalize() = 0;

	// 座標
	virtual void SetPosition(DirectX::SimpleMath::Vector3 position) = 0;  // 設定
	virtual DirectX::SimpleMath::Vector3 GetPosition() const = 0;		  // 取得

	// 速度
	virtual void SetVelocity(DirectX::SimpleMath::Vector3 velocity) = 0;  // 設定
	virtual DirectX::SimpleMath::Vector3 GetVelocity() const = 0;		  // 取得

	// 回転
	virtual void SetRotation(DirectX::SimpleMath::Quaternion rotation) = 0;
	virtual DirectX::SimpleMath::Quaternion GetRotation() const = 0;

	// 重力
	virtual void SetGravity(DirectX::SimpleMath::Vector3 gravity) = 0;  // 設定
	virtual DirectX::SimpleMath::Vector3 GetGravity() const = 0;          // 取得

	// 重なりの補填
	virtual void CorrectOverlap(Field& field) = 0;

public:

	virtual SphereCollider& GetCollider() = 0;
};

