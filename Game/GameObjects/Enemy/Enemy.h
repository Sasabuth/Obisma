/// <summary>
/// Enemyに関するヘッダファイル
/// </summary>
/// <author>仲森智史</author>

// 多重インクルードの防止
#pragma once

// ヘッダファイルの読み込み
#include "Game/Commons/Interface/IEntity.h"
#include "Game/Commons/Interface/IState.h"
#include "Game/Commons/Collision.h"
#include "Game/Commons/UserResources.h"
#include "Game/GameObjects/Enemy/State/EnemyStanding.h"
#include "Game/GameObjects/Enemy/State/EnemyRunning.h"
#include "Game/GameObjects/Enemy/State/EnemyThrowingR.h"
#include "Game/GameObjects/Enemy/State/EnemyThrowingL.h"
#include "Game/GameObjects/Enemy/State/EnemyDizzying.h"
#include "Game/GameObjects/Enemy/State/EnemyCatching.h"
#include "Game/GameObjects/Score/Score.h"
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
	static constexpr float SHADOW_SIZE = 0.4f; // 影の大きさ
	static constexpr float BALL_SPEED = 3.0f;  // ボールの速度
	static constexpr float BALL_POS = 3.2f;  // ボールの座標
	static constexpr float ENEMY_SIZE = 0.003f;
	static constexpr float COLLIDER_SIZE = 0.5f;

	// 手
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

	DirectX::SimpleMath::Matrix m_worldMatrix;

	BallManager* m_ballManager; // ボールマネージャーのポインタ

	IState* m_currentState;  // 現在のステート

	IEntity* m_target;  // ターゲット
	
	std::unique_ptr<EnemyStanding> m_standing;  // 「立つ」状態
	std::unique_ptr<EnemyRunning> m_running;    // 「走る」状態
	std::unique_ptr<EnemyThrowingR> m_throwingR;  // 「右手で投げる」状態
	std::unique_ptr<EnemyThrowingL> m_throwingL;  // 「左手で投げる」状態
	std::unique_ptr<EnemyDizzying> m_dizzying;  // 「くらくら」状態
	std::unique_ptr<EnemyCatching> m_catching;  // 「とる」状態

	DirectX::SimpleMath::Vector3 m_position; // 座標
	DirectX::SimpleMath::Vector3 m_velocity; // 速度

	DirectX::SimpleMath::Quaternion m_rotate; // 回転
	DirectX::SimpleMath::Vector3 m_gravity;  // 重力

	SphereCollider m_collider;  // コライダー
	SphereCollider m_catchCollider;

	std::unique_ptr<Score> m_score;  // スコア

	std::unique_ptr<DirectX::BasicEffect> m_basicEffect;  // ベーシックエフェクト

	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionTexture>> m_primitiveBatch;  // プリミティブバッチ

	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;  // 入力レイアウトへのポインタ

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shadowTexture;  // 影のテクスチャ

	std::map<int, Ball*> m_isBall;  // ボールを持っているか

	int m_ballIndex;  // ボール用のインデックス

	float m_invincibleTime;  // 無敵時間



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

	// レイと球体の交差
	bool CalcRaySphere(
		DirectX::SimpleMath::Vector3 rayPos,
		DirectX::SimpleMath::Vector3 rayDir,
		DirectX::SimpleMath::Vector3 spherePos,
		float radius,
		DirectX::SimpleMath::Vector3& hitPos
	);

	// ボールの座標の設定
	void SetBallPosition(Ball* ball, DirectX::SimpleMath::Matrix handMatrix);

	// 影の初期化
	void InitializeShadow(ID3D11Device* device, ID3D11DeviceContext* context);

	// 影の描画
	void DrawShadow(ID3D11DeviceContext* context, DirectX::CommonStates* states, float radius, DirectX::SimpleMath::Vector3& hitPos);

	// スコアを下げる
	void ScoreDown();


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

	// ワールド
	void SetWorld(DirectX::SimpleMath::Matrix world) { m_worldMatrix = world; }       // 設定
	DirectX::SimpleMath::Matrix GetWorld() const { return m_worldMatrix; }		       // 取得

	// ターゲット
	void SetTarget(IEntity* target) { m_target = target; }       // 設定
	IEntity* GetTarget() const { return m_target; }		         // 取得

	// コライダー
	SphereCollider& GetCollider() override { return m_collider; }

	// キャッチ用コライダー
	SphereCollider& GetCatchCollider() { return m_catchCollider; }

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

	// 無敵時間
	void SetInvincibleTime(float time) { m_invincibleTime = time; }
	float GetInvincibleTime() const { return m_invincibleTime; }

	// スコア
	Score* GetScore() const { return m_score.get(); }


// ステートの取得
public:
	EnemyStanding* GetStanding() const { return m_standing.get(); }
	EnemyRunning* GetRunning() const { return m_running.get(); }
	EnemyThrowingR* GetThrowingR() const { return m_throwingR.get(); }
	EnemyThrowingL* GetThrowingL() const { return m_throwingL.get(); }
	EnemyDizzying* GetDizzying() const { return m_dizzying.get(); }
	EnemyCatching* GetCatching() const { return m_catching.get(); }

	
};

