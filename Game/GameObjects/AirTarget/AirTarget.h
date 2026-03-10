/// <summary>
/// AirTargetに関するヘッダファイル
/// </summary>
/// <author>仲森智史</author>

// 多重インクルードの防止
#pragma once

// ヘッダファイルの読み込み
#include "Game/Commons/Interface/IEntity.h"
#include "Game/Commons/Interface/IState.h"
#include "Game/Commons/Collision.h"
#include "Game/Commons/UserResources.h"
#include "Game/GameObjects/AirTarget/State/Floating.h"
#include "Game/GameObjects/AirTarget/State/Hitting.h"
#include "Game/GameObjects/Particle/Particle.h"


// クラスの定義
class AirTarget : public IEntity
{
// 定数
public:
	// オフセット
	static constexpr float OFFSET = 1.9f;
	// 影のオフセット
	static constexpr float SHADOW_OFFSET = 0.045f;


// 変数
private:
	// ユーザーリソース
	UserResources* m_pUserResources;

	// モデル
	DirectX::Model* m_model;

	// 現在のステート
	IState* m_currentState;

	// ステート
	std::unique_ptr<Floating> m_floating;  //「浮いている」
	std::unique_ptr<Hitting> m_hitting;    //「当たった」

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

	int m_debugIndex;

	// パーティクル
	std::unique_ptr<Particle> m_particle;

	// 当たった座標
	DirectX::SimpleMath::Vector3 m_shadowHitPos;

	// ベーシックエフェクト
	std::unique_ptr<DirectX::BasicEffect> m_basicEffect; 

	// プリミティブバッチ
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionTexture>> m_primitiveBatch; 

	// 入力レイアウトへのポインタ
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;  

	// 影のテクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shadowTexture;  

	// SE
	std::unique_ptr<DirectX::SoundEffectInstance> m_se;

	// ランダムデバイス 
	std::random_device m_rd;


// 関数
public:
	// コンストラクタ
	AirTarget();

	// デストラクタ
	~AirTarget() override;

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

	// メッセージを取得する
	void OnMessegeAccepted(Message::MessageID messageID) override;

	// ステートの変更
	void ChangeState(IState* newState);

	// 影の初期化
	void InitializeShadow(ID3D11Device* device, ID3D11DeviceContext* context);

	// 影の描画
	void DrawShadow(ID3D11DeviceContext* context, DirectX::CommonStates* states, float radius = 1.0f);

	// ランダム座標
	void RandomPosition();


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

	// コライダー
	SphereCollider& GetCollider() override { return m_collider; }

	// シーン
	IState* GetCurrentState() const { return m_currentState; }

	// モデルの取得
	DirectX::Model* GetModel() const { return m_model; }

	// パーティクルの取得
	Particle* GetParticle() { return m_particle.get(); }


// ステートの取得
public:
	Floating* GetFloating() const { return m_floating.get(); }
	Hitting* GetHitting() const { return m_hitting.get(); }


// 内部処理
private:
	// レイと球体の交差
	void CalcRaySphere(DirectX::SimpleMath::Vector3 rayPos, DirectX::SimpleMath::Vector3 rayDir, DirectX::SimpleMath::Vector3 spherePos, float radius, DirectX::SimpleMath::Vector3& hitPos);
};

