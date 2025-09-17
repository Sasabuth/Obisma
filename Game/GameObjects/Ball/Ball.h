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
	static constexpr float SHADOW_SIZE = 0.2f;

	enum BallColor
	{
		PLAYER = 0,
		ENEMY,
		NOMAL,
		MAXCOLOR
	};

	static constexpr DirectX::SimpleMath::Vector4  BALLCOLOR[MAXCOLOR] =
	{
		{ 1,1,0,1 },  // プレイヤー
		{ 1,0,0,1 },  // 敵
		{ 1,1,1,1 },  // ノーマル
	};

// 変数
private:
	UserResources* m_userResources;
	GameplayScene* m_pScene;

	std::unique_ptr<DirectX::Model> m_model;  // モデル

	IState* m_currentState;

	// 「止まる」状態
	std::unique_ptr<Stopping> m_stopping;
	// 「動く」状態
	std::unique_ptr<Moving> m_moving;
	// 「とっている」状態
	std::unique_ptr<Catching> m_catching;

	DirectX::SimpleMath::Vector3 m_position;
	DirectX::SimpleMath::Vector3 m_velocity;

	DirectX::SimpleMath::Quaternion m_rotate;
	DirectX::SimpleMath::Vector3 m_gravity;

	SphereCollider m_collider;

	DirectX::SimpleMath::Vector3 m_hitPos;

	int m_ballColorNum;  // ボールの色の番号

	std::unique_ptr<DirectX::BasicEffect> m_basicEffect;  // ベーシックエフェクト

	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionTexture>> m_primitiveBatch;  // プリミティブバッチ

	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;  // 入力レイアウトへのポインタ

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shadowTexture;  // 影のテクスチャ


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

	// ステートの変更
	void ChangeState(IState* newState);

	// 影の初期化
	void InitializeShadow(ID3D11Device* device, ID3D11DeviceContext* context);

	// 影の描画
	void DrawShadow(ID3D11DeviceContext* context, DirectX::CommonStates* states, float radius = 1.0f);


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

	// ボールの色
	void SetBallColorNum(int ballColorNum);                  // 設定
	int GetBallColorNum() const { return m_ballColorNum; }   // 取得

	// コライダー
	SphereCollider& GetCollider() override { return m_collider; }

	// シーン
	GameplayScene* GetScene() const { return m_pScene; }

	// シーン
	IState* GetCurrentState() const { return m_currentState; }

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
	void CalcRaySphere(DirectX::SimpleMath::Vector3 rayPos, DirectX::SimpleMath::Vector3 rayDir, DirectX::SimpleMath::Vector3 spherePos, float radius, DirectX::SimpleMath::Vector3& hitPos);
};

