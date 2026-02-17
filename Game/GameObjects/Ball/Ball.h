/// <summary>
/// Ballに関するヘッダファイル
/// </summary>
/// <author>仲森智史</author>

// 多重インクルードの防止
#pragma once

// ヘッダファイルの読み込み
#include "Game/Commons/Interface/IEntity.h"
#include "Game/Commons/Interface/IState.h"
#include "Game/Commons/Collision.h"
#include "Game/Commons/UserResources.h"
#include "Game/GameObjects/Particle/Particle.h"
#include "Game/GameObjects/Ball/State/Stopping.h"
#include "Game/GameObjects/Ball/State/Moving.h"
#include "Game/GameObjects/Ball/State/Catching.h"

// クラスの定義
class Field;


// クラスの定義
class Ball : public IEntity
{
// 定数
public:
	// ボールカラー
	enum BallColor
	{
		PLAYER = 0,
		ENEMY,
		NOMAL,
		MAXCOLOR
	};


// 変数
private:
	// ユーザーリソース
	UserResources* m_pUserResources;

	// フィールド
	Field* m_pField;

	// モデル
	std::unique_ptr<DirectX::Model> m_model;  

	// 現在のステート
	IState* m_currentState;

	// ステート
	std::unique_ptr<Stopping> m_stopping;  // 「止まる」
	std::unique_ptr<Moving> m_moving;      // 「動く」
	std::unique_ptr<Catching> m_catching;  // 「とっている」

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

	// 当たった座標
	DirectX::SimpleMath::Vector3 m_shadowHitPos;

	// ボールの色の番号
	int m_ballColorNum; 

	// ベーシックエフェクト
	std::unique_ptr<DirectX::BasicEffect> m_basicEffect;  

	// パーティクル
	std::unique_ptr<Particle> m_particle;

	// プリミティブバッチ
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionTexture>> m_primitiveBatch;  

	// 入力レイアウトへのポインタ
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout; 

	// 影のテクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shadowTexture;  

	// SE
	std::unique_ptr<DirectX::SoundEffectInstance> m_se;

	// 音が鳴ったか
	bool m_isSound;

	// 音の間隔
	float m_soundSpan;

	// 無敵時間
	float m_invincibleTime;


// 関数
public:
	// コンストラクタ
	Ball(Field* pField);

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
	void CorrectOverlap(DirectX::SimpleMath::Vector3& pos) override;

	// ステートの変更
	void ChangeState(IState* newState);

	// 影の初期化
	void InitializeShadow(ID3D11Device* device, ID3D11DeviceContext* context);

	// 影の描画
	void DrawShadow(ID3D11DeviceContext* context, DirectX::CommonStates* states, float radius = 1.0f);

	void NoSound() { m_isSound = false; }


// 設定/取得
public:
	// 座標
	void SetPosition(DirectX::SimpleMath::Vector3 position) override { m_position = position; }   // 設定
	const DirectX::SimpleMath::Vector3& GetPosition() override { return m_position; }		      // 取得

	// 速度
	void SetVelocity(DirectX::SimpleMath::Vector3 velocity) override { m_velocity = velocity; }   // 設定
	const DirectX::SimpleMath::Vector3& GetVelocity() override { return m_velocity; };		      // 取得

	// 回転
	void SetRotation(DirectX::SimpleMath::Quaternion rotation) override { m_rotate = rotation; }  // 設定
	const DirectX::SimpleMath::Quaternion& GetRotation() override { return m_rotate; }		      // 取得

	// 速度
	void SetGravity(DirectX::SimpleMath::Vector3 gravity) override { m_gravity = gravity; }       // 設定
	const DirectX::SimpleMath::Vector3& GetGravity() override { return m_gravity; }		          // 取得

	// 影の当たった座標
	void SetShadowHitPos(DirectX::SimpleMath::Vector3 hitPos) override { m_shadowHitPos = hitPos; }   // 設定
	const DirectX::SimpleMath::Vector3& GetShadowHitPos() override { return m_shadowHitPos; }		  // 取得

	// ボールの色
	void SetBallColorNum(int ballColorNum);                  // 設定
	int GetBallColorNum() const { return m_ballColorNum; }   // 取得

	// 無敵時間
	void SetInvincibleTime(float time) { m_invincibleTime = time; }
	float GetInvincibleTime() const { return m_invincibleTime; }

	// 音が鳴っているか
	bool GetIsSound() const { return m_isSound; }

	// コライダー
	SphereCollider& GetCollider() override { return m_collider; }

	// 状態の取得
	IState* GetCurrentState() const { return m_currentState; }

	// パーティクルの取得
	Particle* GetParticle() const { return m_particle.get(); }

	// モデルの取得
	DirectX::Model* GetModel() const { return m_model.get(); }


// ステートの取得
public:
	Stopping* GetStopping() const { return m_stopping.get(); }
	Moving* GetMoving() const { return m_moving.get(); }
	Catching* GetCatching() const { return m_catching.get(); }


// 内部処理
private:
	// レイと球体の交差
	void CalcRaySphere(DirectX::SimpleMath::Vector3 rayPos, DirectX::SimpleMath::Vector3 rayDir, DirectX::SimpleMath::Vector3 spherePos, 
		float radius, DirectX::SimpleMath::Vector3& hitPos
	);
};

