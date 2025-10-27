/// <summary>
/// Playerに関するヘッダファイル
/// </summary>
/// <author>仲森智史</author>

// 多重インクルードの防止
#pragma once

// ヘッダファイルの読み込み
#include "Game/Commons/Interface/IEntity.h"
#include "Game/Commons/Interface/IState.h"
#include "Game/Commons/Collision.h"
#include "Game/Commons/UserResources.h"
#include "Game/GameObjects/Player/State/Standing.h"
#include "Game/GameObjects/Player/State/Running.h"
#include "Game/GameObjects/Player/State/ThrowingR.h"
#include "Game/GameObjects/Player/State/ThrowingL.h"
#include "Game/GameObjects/Player/State/PlayerCatching.h"
#include "Game/GameObjects/Player/State/Dizzying.h"
#include "Game/GameObjects/Score/Score.h"
#include "Game/Commons/Sprite.h"
#include <map>


// クラスの定義
class GameplayScene;
class Camera;
class BallManager;
class Ball;


// クラスの定義
class Player : public IEntity
{
// 定数
public:
	// 手
	enum HAND
	{
		NONE=0,
		RIGHT,
		LEFT,
	};


// 変数
private:
	// ユーザーリソース
	UserResources* m_userResources;  

	// シーン
	GameplayScene* m_pScene;  

	// ボールマネージャー
	BallManager* m_ballManager;

	// 現在のステート
	IState* m_currentState;  

	// ステート
	std::unique_ptr<Standing> m_standing;        //「立つ」
	std::unique_ptr<Running> m_running;          //「走る」
	std::unique_ptr<ThrowingR> m_throwingR;      //「右で投げる」
	std::unique_ptr<ThrowingL> m_throwingL;      //「左で投げる」
	std::unique_ptr<PlayerCatching> m_catching;  //「キャッチ」
	std::unique_ptr<Dizzying> m_dizzying;        //「くらくら」

	// 座標
	DirectX::SimpleMath::Vector3 m_position; 
	// 速度
	DirectX::SimpleMath::Vector3 m_velocity;
	// 回転
	DirectX::SimpleMath::Quaternion m_rotate;  
	// 重力
	DirectX::SimpleMath::Vector3 m_gravity;   
	// ワールド座標
	DirectX::SimpleMath::Matrix m_worldMatrix; 
	// コライダー
	SphereCollider m_collider;

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
	// ロックオンのテクスチャ
	Sprite m_lockOnTexture; 

	// マウスのレイ
	DirectX::SimpleMath::Ray m_mouseRay;  
	// 当たった点
	DirectX::SimpleMath::Vector3 m_hitPos;  

	// ボールを持っているか
	std::map<int, Ball*> m_isBall;  

	// 無敵時間
	float m_invincibleTime;

	// SE
	std::unique_ptr<DirectX::SoundEffectInstance> m_se;


// 関数
public:
	// コンストラクタ
	Player(GameplayScene* pScene, BallManager* ballManager);

	// デストラクタ
	~Player() override;

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
		DirectX::SimpleMath::Vector3 spherePos,
		float radius,
		DirectX::SimpleMath::Vector3& hitPos
	);

	// レイと球体の交差
	bool CalcRaySphere(
		DirectX::SimpleMath::Vector3 rayPos,
		DirectX::SimpleMath::Vector3 rayDir,
		DirectX::SimpleMath::Vector3 spherePos,
		float radius,
		DirectX::SimpleMath::Vector3& hitPos
	);

	// マウスの方向に回転
	void RotateToMouse();

	// ボールの座標の設定
	void SetBallPosition(Ball* ball, DirectX::SimpleMath::Matrix handMatrix);

	// 影の初期化
	void InitializeShadow(ID3D11Device* device, ID3D11DeviceContext* context);

	// 影の描画
	void DrawShadow(ID3D11DeviceContext* context, DirectX::CommonStates* states, float radius, DirectX::SimpleMath::Vector3& hitPos);

	// ロックオンの描画
	void DrawLockOn(const DirectX::SimpleMath::Vector3& pos);

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
	void SetWorld(DirectX::SimpleMath::Matrix world)  { m_worldMatrix = world; }       // 設定
	DirectX::SimpleMath::Matrix GetWorld() const  { return m_worldMatrix; }		       // 取得

	// コライダー
	SphereCollider& GetCollider() override { return m_collider; }

	// マウスのレイ
	void SetMouseRay(DirectX::SimpleMath::Ray ray) { m_mouseRay = ray; }
	DirectX::SimpleMath::Ray GetMouseRay() const { return m_mouseRay; }

	// 当たった座標
	void SetHitPos(DirectX::SimpleMath::Vector3 hitPos) { m_hitPos = hitPos; }
	DirectX::SimpleMath::Vector3& GetHitPos() { return m_hitPos; }

	// シーン
	GameplayScene* GetScene() const { return m_pScene; }

	// ボールマネージャー
	BallManager* GetBallManager() const { return m_ballManager; }

	// ボール
	void SetCatchBall(int key, Ball* ball);  // 設定
	Ball* GetCatchBall(int key) const;       // 取得

	// 無敵時間
	void SetInvincibleTime(float time) { m_invincibleTime = time; }
	float GetInvincibleTime() const { return m_invincibleTime; }

	// スコア
	Score* GetScore() { return m_score.get(); }


// ステートの取得
public:
	Standing* GetStanding() const { return m_standing.get(); }
	Running* GetRunning() const { return m_running.get(); }
	ThrowingR* GetThrowingR() const { return m_throwingR.get(); }
	ThrowingL* GetThrowingL() const { return m_throwingL.get(); }
	PlayerCatching* GetCatching() const { return m_catching.get(); }
	Dizzying* GetDizzying() const { return m_dizzying.get(); }

	
};

