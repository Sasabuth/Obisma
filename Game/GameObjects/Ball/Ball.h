/// <summary>
/// Ballに関するヘッダファイル
/// </summary>
/// <author>仲森智史</author>
/// <date>2025/05/21</date>

// 多重インクルードの防止
#pragma once

// ヘッダファイルの読み込み
#include "Game/Commons/Interface/IEntity.h"
#include "Game/Commons/Interface/IState.h"
#include "Game/Commons/Collision.h"
#include "Game/Commons/UserResources.h"
#include "Game/GameObjects/Ball/State/Stopping.h"
#include "Game/GameObjects/Ball/State/Moving.h"
#include "Game/GameObjects/Ball/State/Catching.h"

// クラスの定義
class GameplayScene;
class Camera;


// クラスの定義
class Ball : public IEntity
{
public:
	static constexpr float BALL_SIZE = 0.15f;

// 変数
private:
	GameplayScene* m_pScene;

	IState* m_currentState;

	// 「止まる」状態
	std::unique_ptr<Stopping> m_stopping;
	// 「動く」状態
	std::unique_ptr<Moving> m_moving;
	// 「とっている」状態
	std::unique_ptr<Catching> m_catching;

	DirectX::SimpleMath::Vector3 m_position;
	DirectX::SimpleMath::Vector3 m_velocity;
	DirectX::SimpleMath::Vector3 m_speed;

	DirectX::SimpleMath::Quaternion m_rotate;
	DirectX::SimpleMath::Vector3 m_gravity;

	SphereCollider m_collider;

	std::unique_ptr<DirectX::BasicEffect> m_basicEffect;  // ベーシックエフェクト

	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_primitiveBatch;  // プリミティブバッチ

	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;  // 入力レイアウトへのポインタ

// 関数
public:
	// コンストラクタ
	Ball(GameplayScene* pScene);

	// デストラクタ
	~Ball() override;

	// 初期化
	void Initialize(DirectX::SimpleMath::Vector3 position) override;

	// 更新
	void Update(float elapsedTime) override;

	// 描画
	void Render() override;

	// 終了処理
	void Finalize() override;

	// 重なりの補填
	void CorrectOverlap(Field& field) override;

	// 新しい状態に遷移する
	void ChangeState(IState* newState);


// 設定/取得
public:
	// 座標
	void SetPosition(DirectX::SimpleMath::Vector3 position) override { m_position = position; }   // 設定
	DirectX::SimpleMath::Vector3 GetPosition() const override { return m_position; }		      // 取得

	// 速度
	void SetVelocity(DirectX::SimpleMath::Vector3 velocity) override { m_velocity = velocity; }   // 設定
	DirectX::SimpleMath::Vector3 GetVelocity() const override { return m_velocity; };		      // 取得

	// 回転
	void SetRotation(DirectX::SimpleMath::Quaternion rotation) override { m_rotate = rotation; }  // 設定
	DirectX::SimpleMath::Quaternion GetRotation() const override { return m_rotate; }		      // 取得

	// 速度
	void SetGravity(DirectX::SimpleMath::Vector3 gravity) override { m_gravity = gravity; }       // 設定
	DirectX::SimpleMath::Vector3 GetGravity() const override { return m_gravity; }		          // 取得

	// 速度
	void SetSpeed(DirectX::SimpleMath::Vector3 speed) { m_speed = speed; }       // 設定
	DirectX::SimpleMath::Vector3 GetSpeed() const { return m_speed; }		          // 取得

	// コライダー
	SphereCollider& GetCollider() override { return m_collider; }

	// シーン
	GameplayScene* GetScene() const { return m_pScene; }

	// シーン
	IState* GetCurrentState() const { return m_currentState; }

// ステートの取得
public:
	Stopping* GetStopping() const { return m_stopping.get(); }
	Moving* GetMoving() const { return m_moving.get(); }
	Catching* GetCatching() const { return m_catching.get(); }

	
};

