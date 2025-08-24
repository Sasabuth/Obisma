/**
 * @file   Collider.h
 *
 * @brief  コライダーに関するヘッダファイル
 *
 * @author 制作者名　仲森智史
 *
 * @date   日付 5/17
 */

 // 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include <GeometricPrimitive.h>



// クラスの定義 ===============================================================
/**
 * @brief 球のコライダー
 */
class SphereCollider
{
	// クラス定数の宣言 -------------------------------------------------
private:



	// データメンバの宣言 -----------------------------------------------
private:
	DirectX::SimpleMath::Vector3 m_position;  // 座標
	float m_radius;  // 半径

	std::unique_ptr<DirectX::GeometricPrimitive> m_sphere;  // 球


	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	SphereCollider();

	// デストラクタ
	~SphereCollider();


// 操作
public:
	// 初期化処理
	void Initialize(ID3D11DeviceContext* pContext, DirectX::SimpleMath::Vector3 position, float radius);

	// 描画処理
	void Draw(DirectX::CommonStates* states, DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj);

	// 座標の設定
	void SetPosition(DirectX::SimpleMath::Vector3 position);
	void SetPosition(float posX, float posY, float posZ);

	// 半径の設定
	void SetRadius(float radius);


// 取得/設定
public:
	// 座標の取得
	DirectX::SimpleMath::Vector3 GetPosition() const;

	// 半径の取得
	float GetRadius() const;


// 内部実装
private:


};



/**
 * @brief 立方体のコライダー
 */
class CubeCollider
{
	// クラス定数の宣言 -------------------------------------------------
private:



	// データメンバの宣言 -----------------------------------------------
private:
	DirectX::SimpleMath::Vector3 m_position;  // 座標
	DirectX::SimpleMath::Vector3 m_extent;  // 半径

	std::unique_ptr<DirectX::GeometricPrimitive> m_cube;  // 立方体


	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	CubeCollider();

	// デストラクタ
	~CubeCollider();


// 操作
public:
	// 初期化処理
	void Initialize(ID3D11DeviceContext* pContext, DirectX::SimpleMath::Vector3 position, DirectX::SimpleMath::Vector3 size);

	// 描画処理
	void Draw(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj, DirectX::FXMVECTOR color = DirectX::Colors::White);

	// 座標の設定
	void SetPosition(DirectX::SimpleMath::Vector3 pos) { m_position = pos; }
	void SetPosition(float posX, float posY, float posZ) { m_position = DirectX::SimpleMath::Vector3(posX, posY, posZ); }

	// サイズの設定
	void SetExtent(DirectX::SimpleMath::Vector3 extent) { m_extent = extent; }


// 取得/設定
public:
	// 座標の取得
	DirectX::SimpleMath::Vector3 GetPosition() const { return m_position; }
	// 半径の取得
	DirectX::SimpleMath::Vector3 GetExtent() const { return m_extent; }


	// 内部実装
private:


};


// 当たり判定
bool IsHit(const SphereCollider& sphereA, const SphereCollider& sphereB);  // 球と球
bool IsHit(const CubeCollider& cubeA, const CubeCollider& cubeB);          // 立方体と立方体
