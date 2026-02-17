/// <summary>
/// Arrowに関するヘッダファイル
/// </summary>
/// <author>仲森智史</author>

// 多重インクルードの防止
#pragma once

// ヘッダファイルの読み込み
#include "Game/Commons/Interface/IEntity.h"
#include "Game/Commons/Collision.h"
#include "Game/Commons/UserResources.h"


// クラスの定義
class Player;


// クラスの定義
class Arrow : public IEntity
{
// 定数
public:
	// 矢のサイズ
	static constexpr float ARROW_SIZE = 0.4f;


// 変数
private:
	// ユーザーリソース
	UserResources* m_pUserResources;

	// プレイヤー
	Player* m_pPlayer;

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

	// 影の当たった座標
	DirectX::SimpleMath::Vector3 m_shadowHitPos;

	// 時間
	float m_waveSpeed;

	// 描画するか
	bool m_isDraw;

	// ベーシックエフェクト
	std::unique_ptr<DirectX::BasicEffect> m_basicEffect;

	// プリミティブバッチ
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionTexture>> m_primitiveBatch;

	// 入力レイアウトへのポインタ
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

	// 矢印のテクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_arrowTexture;


	// 関数
public:
	// コンストラクタ
	Arrow(Player* pPlayer);

	// デストラクタ
	~Arrow() override;

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

	// 矢印の初期化
	void InitializeArrow(ID3D11Device* device, ID3D11DeviceContext* context);


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

	// 描画
	void SetIsDraw(bool isDraw) { m_isDraw = isDraw; }
	bool GetIsDraw() { return m_isDraw; }
};

