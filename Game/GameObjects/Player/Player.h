/// <summary>
/// プレイヤーに関するヘッダファイル
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
#include "Game/GameObjects/Player/State/Standing.h"
#include "Game/GameObjects/Player/State/Running.h"


// クラスの定義
class GameplayScene;
class Camera;


// クラスの定義
class Player : public IEntity
{
private:
	static constexpr float ROTATE_SPEED = 0.5f;

// 変数
private:
	UserResources* m_userResources;

	GameplayScene* m_pScene;
	Camera* m_pCamera;

	IState* m_currentState;

	// 「立つ」状態
	std::unique_ptr<IState> m_standing;
	// 「走る」状態
	std::unique_ptr<IState> m_running;

	DirectX::Model* m_model;  // モデル

	DirectX::SimpleMath::Vector3 m_position;
	DirectX::SimpleMath::Vector3 m_velocity;

	DirectX::SimpleMath::Quaternion m_rotate;
	DirectX::SimpleMath::Vector3 m_gravity;

	SphereCollider m_collider;

	std::unique_ptr<DirectX::BasicEffect> m_basicEffect;  // ベーシックエフェクト

	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_primitiveBatch;  // プリミティブバッチ

	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;  // 入力レイアウトへのポインタ

	DirectX::SimpleMath::Ray m_mouseRay;
	DirectX::SimpleMath::Vector3 m_hitPos;

// 関数
public:
	// コンストラクタ
	Player(GameplayScene* pScene, Camera* pCamera);

	// デストラクタ
	~Player() override;

	// 初期化
	void Initialize() override;

	// 更新
	void Update(float elapsedTime) override;

	// 描画
	void Render() override;

	// 終了処理
	void Finalize() override;

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

	// 重なりの補填
	void CorrectOverlap(Field& field) override;

	// 新しい状態に遷移する
	void ChangeState(IState* newState) { m_currentState = newState; }

public:
	SphereCollider& GetCollider() override { return m_collider; }

private:
	DirectX::SimpleMath::Ray CreatePickingRay(
		int mouseX, int mouseY, 
		int screenWidth, int screenHeight,
		const DirectX::SimpleMath::Matrix& view, 
		const DirectX::SimpleMath::Matrix& proj
	);

	// レイと球体の交差
	bool CalcRaySphere(
		DirectX::SimpleMath::Vector3 rayPos,
		DirectX::SimpleMath::Vector3 rayDir,
		DirectX::SimpleMath::Vector3 spherePos,
		float radius,
		DirectX::SimpleMath::Vector3& hitPos
	);
};

