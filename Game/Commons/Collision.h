/// <summary>
/// Collisionに関するヘッダファイル
/// </summary>
/// <author>仲森智史</author>

// 多重インクルードの防止
#pragma once

// ヘッダファイルの読み込み
#include <GeometricPrimitive.h>



// クラスの定義
class SphereCollider
{
// 定数
private:



// 変数
private:
	DirectX::SimpleMath::Vector3 m_position;  // 座標
	float m_radius;  // 半径

	std::unique_ptr<DirectX::GeometricPrimitive> m_sphere;  // 球


// 関数
public:
	// コンストラクタ
	SphereCollider();

	// デストラクタ
	~SphereCollider();

	// 初期化処理
	void Initialize(ID3D11DeviceContext* pContext, DirectX::SimpleMath::Vector3 position, float radius);

	// 描画処理
	void Draw(DirectX::CommonStates* states, DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj);


// 取得/設定
public:
	// 座標の設定
	void SetPosition(DirectX::SimpleMath::Vector3 position);
	void SetPosition(float posX, float posY, float posZ);

	// 半径の設定
	void SetRadius(float radius);

	// 半径の取得
	float GetRadius() const;

	// 座標の取得
	DirectX::SimpleMath::Vector3 GetPosition() const;



// 内部実装
private:


};



// クラスの定義
class CubeCollider
{
// 定数
private:


// 変数
private:
	DirectX::SimpleMath::Vector3 m_position;  // 座標
	DirectX::SimpleMath::Vector3 m_extent;  // 半径

	std::unique_ptr<DirectX::GeometricPrimitive> m_cube;  // 立方体


// 関数
public:
	// コンストラクタ
	CubeCollider();

	// デストラクタ
	~CubeCollider();

	// 初期化処理
	void Initialize(ID3D11DeviceContext* pContext, DirectX::SimpleMath::Vector3 position, DirectX::SimpleMath::Vector3 size);

	// 描画処理
	void Draw(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj, DirectX::FXMVECTOR color = DirectX::Colors::White);

	
// 取得/設定
public:
	// 座標の設定
	void SetPosition(DirectX::SimpleMath::Vector3 pos) { m_position = pos; }
	void SetPosition(float posX, float posY, float posZ) { m_position = DirectX::SimpleMath::Vector3(posX, posY, posZ); }

	// 座標の取得
	DirectX::SimpleMath::Vector3 GetPosition() const { return m_position; }

	// サイズの設定
	void SetExtent(DirectX::SimpleMath::Vector3 extent) { m_extent = extent; }

	// 半径の取得
	DirectX::SimpleMath::Vector3 GetExtent() const { return m_extent; }


// 内部実装
private:


};


// 当たり判定
bool IsHit(const SphereCollider& sphereA, const SphereCollider& sphereB);  // 球と球
bool IsHit(const CubeCollider& cubeA, const CubeCollider& cubeB);          // 立方体と立方体
