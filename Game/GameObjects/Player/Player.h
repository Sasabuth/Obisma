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
class AirTarget;
class Field;
class BallManager;
class Ball;
class Enemy;


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

	static constexpr float BASE_WIDTH = 1280.0f;
	static constexpr float BASE_HEIGHT = 720.0f;

	static constexpr float MIN_AIRPOS = 12.0f;

	static constexpr float LOCKON_HEIGHT_RATE = 0.177f;

	static constexpr Sprite::Format LOCKON =
	{
		DirectX::SimpleMath::Vector2(-1.0f, -1.0f),      // 使わない
		DirectX::SimpleMath::Vector2(1256.0f, 1244.0f),  // サイズ
		0.1f                                             // 拡大率
	};


// 変数
private:
	// ユーザーリソース
	UserResources* m_pUserResources;  

	// フィールド
	Field* m_pField;  

	// 空中の的
	AirTarget* m_pAirTarget;

	// ボールマネージャー
	BallManager* m_pBallManager;

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
	// コライダー
	SphereCollider m_collider;

	DirectX::SimpleMath::Matrix m_world;

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
	DirectX::SimpleMath::Vector3 m_mouseRayHitPos;  

	// 影の当たった点
	DirectX::SimpleMath::Vector3 m_shadowHitPos;

	// ボールを持っているか
	std::map<int, Ball*> m_isBall;  

	// 無敵時間
	float m_invincibleTime;

	// SE
	std::unique_ptr<DirectX::SoundEffectInstance> m_se;


// 関数
public:
	// コンストラクタ
	Player(Field* pField, AirTarget* pAirTarget, BallManager* pBallManager);

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
	void CorrectOverlap(DirectX::SimpleMath::Vector3& pos) override;

	// ステートの変更
	void ChangeState(IState* newState);

	void OnEvents(const std::vector<IState::Event>& events);

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
	void DrawShadow(ID3D11DeviceContext* context, DirectX::CommonStates* states, float radius);

	// ロックオンの描画
	void DrawLockOn(const DirectX::SimpleMath::Vector3& pos);

	// スコアを下げる
	void ScoreDown();

	// 当たる距離か
	bool IsInHitRange(float offset = 0.0f);


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

	// コライダー
	SphereCollider& GetCollider() override { return m_collider; }

	void SetWorld(DirectX::SimpleMath::Matrix world) { m_world = world; }
	DirectX::SimpleMath::Matrix GetWorld() { return m_world; }

	// マウスのレイ
	void SetMouseRay(DirectX::SimpleMath::Ray ray) { m_mouseRay = ray; }
	DirectX::SimpleMath::Ray GetMouseRay() const { return m_mouseRay; }

	// 当たった座標
	void SetMouseRayHitPos(DirectX::SimpleMath::Vector3 hitPos) { m_mouseRayHitPos = hitPos; }
	DirectX::SimpleMath::Vector3& GetMouseRayHitPos() { return m_mouseRayHitPos; }

	// フィールドの取得
	Field* GetField() const { return m_pField; }

	// 空中の的の取得
	AirTarget* GetAirTarget() const { return m_pAirTarget; }

	// ボールマネージャー
	BallManager* GetBallManager() const { return m_pBallManager; }

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

