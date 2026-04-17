/// <summary>
/// Collisionに関するヘッダファイル
/// </summary>
/// <author>仲森智史</author>

// 多重インクルードの防止
#pragma once

// ヘッダファイルの読み込み
#include <GeometricPrimitive.h>


// クラスの定義
class Sprite;

//-----------------------------------------------------------------------
// 2D用
//-----------------------------------------------------------------------
// 矩形コライダー
class BoxCollider2D
{
	// データメンバの宣言 -----------------------------------------------
private:
	DirectX::SimpleMath::Vector2 m_center; // 中心座標
	DirectX::SimpleMath::Vector2 m_halfSize; // 中心座標
	float m_scale;

	std::unique_ptr<Sprite> m_debug;


	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	BoxCollider2D();
	~BoxCollider2D();


	// 操作
public:
	// 描画
	void Draw(DirectX::FXMVECTOR color = DirectX::Colors::White);


	// 取得/設定
public:
	// 左端の取得
	float GetLeft() const;

	// 上端の取得
	float GetTop() const;

	// 右端の取得
	float GetRight() const;

	// 下端の取得
	float GetBottom() const;


	// 中心座標の設定
	void SetPosition(DirectX::SimpleMath::Vector2 pos) { m_center = pos; };
	// 座標の取得
	DirectX::SimpleMath::Vector2 GetPosition() const { return m_center; }

	// サイズの設定
	void SetSize(DirectX::SimpleMath::Vector2 size) { m_halfSize = size / 2; };
	// サイズの取得
	DirectX::SimpleMath::Vector2 GetSize() const { return m_halfSize * 2; }
	// 拡大率の設定
	void SetScale(float scale) { m_scale = scale; };
	// 拡大率の設定
	float GetScale() const { return m_scale; };

};



// 円のコライダー
class CircleCollider2D
{
	// データメンバの宣言 -----------------------------------------------
private:
	float m_centerX;    ///< 中心座標(X座標)
	float m_centerY;    ///< 中心座標(Y座標)
	float m_radius;     ///< 半径

	std::unique_ptr<Sprite> m_debug;



	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	CircleCollider2D();
	~CircleCollider2D();


	// 操作
public:
	// 描画
	void Draw();


	// 取得/設定
public:
	// 中心座標(X座標)の取得
	float GetCenterX() const;

	// 中心座標(Y座標)の取得
	float GetCenterY() const;

	// 半径の取得
	float GetRadius() const;


	// 中心座標の設定
	void SetPosition(float centerX, float centerY);

	// サイズの設定
	void SetRadius(float radius);
};



//-----------------------------------------------------------------------
// 3D用
//-----------------------------------------------------------------------
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
	SphereCollider();
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
	DirectX::SimpleMath::Vector3 m_extent;    // 直径

	std::unique_ptr<DirectX::GeometricPrimitive> m_cube;  // 立方体


// 関数
public:
	// コンストラクタ
	CubeCollider();

	CubeCollider(const CubeCollider&) = delete;            // コピー禁止
	CubeCollider& operator=(const CubeCollider&) = delete;

	CubeCollider(CubeCollider&&) = default;                // ムーブOK
	CubeCollider& operator=(CubeCollider&&) = default;

	// デストラクタ
	~CubeCollider();

	// 初期化処理
	void Initialize(ID3D11DeviceContext* pContext, DirectX::SimpleMath::Vector3 position, DirectX::SimpleMath::Vector3 size);

	// 描画処理
	void Draw(DirectX::CommonStates* states, DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj);

	
// 取得/設定
public:
	// 座標の設定
	void SetPosition(DirectX::SimpleMath::Vector3 pos) { m_position = pos; }
	void SetPosition(float posX, float posY, float posZ) { m_position = DirectX::SimpleMath::Vector3(posX, posY, posZ); }

	// 座標の取得
	DirectX::SimpleMath::Vector3 GetPosition() const { return m_position; }

	// サイズの設定
	void SetExtent(DirectX::SimpleMath::Vector3 extent) { m_extent = extent; }

	// 直径の取得
	DirectX::SimpleMath::Vector3 GetExtent() const { return m_extent; }


// 内部実装
private:


};

// クラスの定義
class ModelCollider
{
// 定数
private:
	// グループ
	struct Group
	{
		DirectX::SimpleMath::Vector3 position;  // 座標
		DirectX::SimpleMath::Vector3 extent;    // 直径
		std::vector<int> index;                 // 三角形の番号
	};


// 変数
private:
	// 頂点情報の配列
	std::vector<DirectX::VertexPosition> m_vertices;

	// 三角形を作るための番号の配列
	std::vector<uint32_t> m_indices;

	// 座標
	DirectX::SimpleMath::Vector3 m_position; 
	// 拡大率
	float m_scale;  

	// グループ
	std::vector<Group> m_groups;
	// デバック用コライダー
	std::vector<CubeCollider> m_debugColliders;

	// エフェクト
	std::unique_ptr<DirectX::BasicEffect> m_effect;

	// バッチ
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_batch;


// 関数
public:
	// コンストラクタ
	ModelCollider();

	// デストラクタ
	~ModelCollider();

	// 初期化処理
	void Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, DirectX::Model* pModel, DirectX::SimpleMath::Vector3 position, float scale);

	// 描画処理
	void Draw(ID3D11DeviceContext* pContext, DirectX::CommonStates* states, DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj, DirectX::FXMVECTOR color = DirectX::Colors::White);
	void DebugDraw(ID3D11DeviceContext* pContext, DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj, int index, DirectX::FXMVECTOR color = DirectX::Colors::White);



// 取得/設定
public:
	// 座標の設定
	void SetPosition(DirectX::SimpleMath::Vector3 position) { m_position = position; }
	void SetPosition(float posX, float posY, float posZ) { m_position = DirectX::SimpleMath::Vector3(posX, posY, posZ); }
	const DirectX::SimpleMath::Vector3& GetPosition() const { return m_position; }

	// 拡大率の設定
	void SetScale(float scale) { m_scale = scale; }

	// 拡大率の取得
	float GetScale() const { return m_scale; }

	// 番号の数の取得
	size_t GetIndicesCount() const { return m_indices.size(); }
	// 番号の取得
	uint32_t GetIndices(int index) const { return m_indices[index]; }

	// グループの数の取得
	size_t GetGroupCount() const { return m_groups.size(); }
	// グループの取得
	const Group& GetGroup(int index) { return m_groups[index]; }

	// 中心の座標の取得
	DirectX::SimpleMath::Vector3 GetCenterPosition(int index) const;

	// 頂点の取得
	const DirectX::VertexPosition& GetVertices(int index)  const { return m_vertices[index]; }

	// 法線ベクトルの取得
	DirectX::SimpleMath::Vector3 GetNormalVector(int index) const;


// 内部実装
private:
	// グループの設定
	void SetGroup(ID3D11DeviceContext* pContext);
};


// 当たり判定
bool IsHit(const BoxCollider2D& boxA, const BoxCollider2D& boxB);          // 矩形コライダーと矩形コライダーの当たり判定
bool IsHit(const SphereCollider& sphereA, const SphereCollider& sphereB);  // 球と球
bool IsHit(const CubeCollider& cubeA, const CubeCollider& cubeB);          // 立方体と立方体
bool IsHit(const SphereCollider& sphere, const DirectX::SimpleMath::Vector3& boxCenter,  // 球と立方体
	const DirectX::SimpleMath::Vector3& boxHalfSize);

// レイとモデル
bool IsHit(
	const DirectX::SimpleMath::Vector3& rayOrigin,
	const DirectX::SimpleMath::Vector3& rayDir,
	const DirectX::SimpleMath::Vector3& p0,
	const DirectX::SimpleMath::Vector3& p1,
	const DirectX::SimpleMath::Vector3& p2,
	DirectX::SimpleMath::Vector3& outHitPoint
);

// レイと立方体
bool IsHit(
	const DirectX::SimpleMath::Vector3& rayOrigin,
	const DirectX::SimpleMath::Vector3& rayDir,
	const DirectX::SimpleMath::Vector3& center,
	const DirectX::SimpleMath::Vector3& extent
);


