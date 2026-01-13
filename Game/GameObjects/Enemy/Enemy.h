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
class Player;
class Field;
class AirTarget;
class BallManager;
class Ball;


// クラスの定義
class Enemy : public IEntity
{
// 定数
public:
	// 手
	enum HAND
	{
		NONE = 0,
		RIGHT,
		LEFT,
	};


// 変数
private:
	// ユーザーリソース
	UserResources* m_pUserResources;

	// プレイヤー
	Player* m_pPlayer; 

	// フィールド
	Field* m_pField;

	// 空中の的
	AirTarget* m_pAirTarget;

	// ワールド座標
	DirectX::SimpleMath::Matrix m_worldMatrix;

	// ボールマネージャーのポインタ
	BallManager* m_ballManager;

	// 現在のステート
	IState* m_currentState;  

	// ターゲット
	IEntity* m_target; 
	
	// ステート
	std::unique_ptr<EnemyStanding> m_standing;   // 「立つ」	
	std::unique_ptr<EnemyRunning> m_running;     // 「走る」	
	std::unique_ptr<EnemyThrowingR> m_throwingR; // 「右手で投げる」	
	std::unique_ptr<EnemyThrowingL> m_throwingL; // 「左手で投げる」	
	std::unique_ptr<EnemyDizzying> m_dizzying;   // 「くらくら」	
	std::unique_ptr<EnemyCatching> m_catching;   // 「とる」	

	// 座標
	DirectX::SimpleMath::Vector3 m_position; 
	// 速度
	DirectX::SimpleMath::Vector3 m_velocity; 
	// 回転
	DirectX::SimpleMath::Quaternion m_rotate; 
	// 重力
	DirectX::SimpleMath::Vector3 m_gravity;  
	// コライダー
	SphereCollider m_collider;  
	// キャッチコライダー
	SphereCollider m_catchCollider;

	// 影の当たった座標
	DirectX::SimpleMath::Vector3 m_shadowHitPos;

	// スコア
	std::unique_ptr<Score> m_score;  

	// ベーシックエフェクト
	std::unique_ptr<DirectX::BasicEffect> m_basicEffect; 

	// プリミティブバッチ
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionTexture>> m_primitiveBatch; 

	// 入力レイアウトへのポインタ
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

	// 影のテクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shadowTexture;  

	// ボールを持っているか
	std::map<int, Ball*> m_isBall;  

	// ボール用のインデックス
	int m_ballIndex;  

	// 無敵時間
	float m_invincibleTime;  

	// SE
	std::unique_ptr<DirectX::SoundEffectInstance> m_se;


// 関数
public:
	// コンストラクタ
	Enemy(Player* pPlayer, Field* pField, AirTarget* pAirTarget, BallManager* pBallManager);

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
	void CorrectOverlap(IEntity& iEntity);
	void CorrectOverlap(DirectX::SimpleMath::Vector3& pos) override;

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
	void DrawShadow(ID3D11DeviceContext* context, DirectX::CommonStates* states, float radius);

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

	// 影の当たった座標
	void SetShadowHitPos(DirectX::SimpleMath::Vector3 hitPos) override { m_shadowHitPos = hitPos; }   // 設定
	DirectX::SimpleMath::Vector3 GetShadowHitPos() const override { return m_shadowHitPos; }		  // 取得

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

	// プレイヤーの取得
	Player* GetPlayer() const { return m_pPlayer; }

	// 空中の的の取得
	AirTarget* GetAirTarget() const { return m_pAirTarget; }

	// ボールマネージャーの取得
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

	// 状態の取得
	IState* GetCurrentState() const { return m_currentState; }


// ステートの取得
public:
	EnemyStanding* GetStanding() const { return m_standing.get(); }
	EnemyRunning* GetRunning() const { return m_running.get(); }
	EnemyThrowingR* GetThrowingR() const { return m_throwingR.get(); }
	EnemyThrowingL* GetThrowingL() const { return m_throwingL.get(); }
	EnemyDizzying* GetDizzying() const { return m_dizzying.get(); }
	EnemyCatching* GetCatching() const { return m_catching.get(); }

	
};

