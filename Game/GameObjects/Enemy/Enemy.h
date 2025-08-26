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
#include "Game/GameObjects/Enemy/State/EnemyStanding.h"
#include "Game/GameObjects/Enemy/State/EnemyRunning.h"
#include "Game/GameObjects/Enemy/State/EnemyThrowing.h"
#include <map>


// クラスの定義
class GameplayScene;
class Camera;
class BallManager;
class Ball;


// クラスの定義
class Enemy : public IEntity
{
public:
	static constexpr float PLAYER_SIZE = 0.003f;
	static constexpr float SHADOW_SIZE = 0.4f;

	enum HAND
	{
		NONE = 0,
		RIGHT,
		LEFT,
	};


// 変数
private:
	UserResources* m_userResources;  // ユーザーリソース

	GameplayScene* m_pScene;  // シーン

	BallManager* m_ballManager;

	IState* m_currentState;  // 現在のステート

	
	std::unique_ptr<EnemyStanding> m_standing;  // 「立つ」状態
	std::unique_ptr<EnemyRunning> m_running;    // 「走る」状態
	std::unique_ptr<EnemyThrowing> m_throwing;  // 「投げる」状態

	DirectX::SimpleMath::Vector3 m_position; // 座標
	DirectX::SimpleMath::Vector3 m_velocity; // 速度

	DirectX::SimpleMath::Quaternion m_rotate; // 回転
	DirectX::SimpleMath::Vector3 m_gravity;  // 重力

	SphereCollider m_collider;

	std::unique_ptr<DirectX::BasicEffect> m_basicEffect;  // ベーシックエフェクト

	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionTexture>> m_primitiveBatch;  // プリミティブバッチ

	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;  // 入力レイアウトへのポインタ

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shadowTexture;  // 影のテクスチャ

	DirectX::SimpleMath::Ray m_mouseRay;  // マウスのレイ
	DirectX::SimpleMath::Vector3 m_hitPos;  // 当たった点

	std::map<int, Ball*> m_isBall;  // ボールを持っているか

	int m_ballIndex;

// 関数
public:
	// コンストラクタ
	Enemy(GameplayScene* pScene, BallManager* ballManager);

	// デストラクタ
	~Enemy() override;

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

	// ステートの変更
	void ChangeState(IState* newState);

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

	// 影の初期化
	void InitializeShadow(ID3D11Device* device, ID3D11DeviceContext* context);

	// 影の描画
	void DrawShadow(ID3D11DeviceContext* context, DirectX::CommonStates* states, float radius, DirectX::SimpleMath::Vector3& hitPos);


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

	// コライダー
	SphereCollider& GetCollider() override { return m_collider; }

	// マウスのレイ
	void SetMouseRay(DirectX::SimpleMath::Ray ray) { m_mouseRay = ray; }
	DirectX::SimpleMath::Ray GetMouseRay() const { return m_mouseRay; }

	// 当たった座標
	DirectX::SimpleMath::Vector3& GetHitPos() { return m_hitPos; }

	// シーン
	GameplayScene* GetScene() const { return m_pScene; }

	// シーン
	BallManager* GetBallManager() const { return m_ballManager; }

	// ボール
	void SetCatchBall(int key, Ball* ball);  // 設定
	Ball* GetCatchBall(int key) const;       // 取得

	// ボールインデックス
	void SetBallIndex(int index) { m_ballIndex = index; }  // 設定
	int GetBallIndex() const { return m_ballIndex; }       // 取得


// ステートの取得
public:
	EnemyStanding* GetStanding() const { return m_standing.get(); }
	EnemyRunning* GetRunning() const { return m_running.get(); }
	EnemyThrowing* GetThrowing() const { return m_throwing.get(); }

	
};

