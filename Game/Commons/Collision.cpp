/**
 * @file   Collider.cpp
 *
 * @brief  コライダーに関するソースファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "Collision.h"


using namespace DirectX;


// メンバ関数の定義 ===========================================================
//*************************************************************************
// 
// 球のコライダー
// 
//*************************************************************************
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
SphereCollider::SphereCollider()
	: m_position{ SimpleMath::Vector3{0.0f,0.0f,0.0f} }
    , m_radius{ 0.0f }
	, m_sphere{ nullptr }
{

}



/**
 * @brief デストラクタ
 */
SphereCollider::~SphereCollider()
{

}



/**
 * @brief 初期化処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void SphereCollider::Initialize(ID3D11DeviceContext* pContext, DirectX::SimpleMath::Vector3 position, float radius)
{
	// 座標の設定
	m_position = position;

	// 半径の設定
	m_radius = radius;

	// 球の設定
	m_sphere = DirectX::GeometricPrimitive::CreateSphere(pContext, m_radius * 2, 32);
}



/**
 * @brief 描画処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void SphereCollider::Draw(DirectX::CommonStates* states, DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj)
{
	DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::CreateTranslation(m_position);

	m_sphere->Draw(
		world,
		view,
		proj,
		DirectX::Colors::White,   // ここに半透明色
		nullptr,
		states->DepthRead()
	);
}



/// <summary>
/// 座標の取得
/// </summary>
/// <param name="position"></param> 座標
void SphereCollider::SetPosition(DirectX::SimpleMath::Vector3 position)
{
	m_position = position;
}



/// <summary>
/// 座標の設定
/// </summary>
/// <param name="posX"></param> X座標
/// <param name="posY"></param> Y座標
/// <param name="posZ"></param> Z座標
void SphereCollider::SetPosition(float posX, float posY, float posZ)
{
	m_position.x = posX;
	m_position.y = posY;
	m_position.z = posZ;
}



/// <summary>
/// 半径の設定
/// </summary>
/// <param name="radius"></param> 半径
void SphereCollider::SetRadius(float radius)
{
	m_radius = radius;
}



/// <summary>
/// 座標の取得
/// </summary>
/// <returns></returns> 座標
DirectX::SimpleMath::Vector3 SphereCollider::GetPosition() const
{
	return m_position;
}



/// <summary>
/// 半径の取得
/// </summary>
/// <returns></returns> 半径
float SphereCollider::GetRadius() const
{
	return m_radius;
}



//*************************************************************************
// 
// 立方体のコライダー
// 
//*************************************************************************
/// <summary>
/// コンストラクタ
/// </summary>
CubeCollider::CubeCollider()
	: m_position{}
	, m_extent{ 0.0f }
	, m_cube{ nullptr }
{
}



/// <summary>
/// デストラクタ
/// </summary>
CubeCollider::~CubeCollider()
{
}



/// <summary>
/// 初期化
/// </summary>
/// <param name="pContext">コンテキスト</param>
/// <param name="position">座標</param>
/// <param name="extent">半径</param>
void CubeCollider::Initialize(ID3D11DeviceContext* pContext, DirectX::SimpleMath::Vector3 position, DirectX::SimpleMath::Vector3 extent)
{
	// 座標の設定
	m_position = position;

	// サイズの設定
	m_extent = extent;

	m_cube = DirectX::GeometricPrimitive::CreateBox(pContext, m_extent);
}



/// <summary>
/// 描画
/// </summary>
/// <param name="view">ビュー行列</param>
/// <param name="proj">プロジェクション行列</param>
void CubeCollider::Draw(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj, DirectX::FXMVECTOR color)
{
	SimpleMath::Matrix world;

	world = SimpleMath::Matrix::CreateTranslation(m_position);
	m_cube->Draw(world, view, proj, color);
}



/// <summary>
/// 円と円の当たり判定
/// </summary>
/// <param name="sphereA"></param> 球A
/// <param name="sphereB"></param> 球B
/// <returns>当たっているか</returns>  
bool IsHit(const SphereCollider& sphereA, const SphereCollider& sphereB)
{
	// 中心の取得
	float dx = sphereA.GetPosition().x - sphereB.GetPosition().x;
	float dy = sphereA.GetPosition().y - sphereB.GetPosition().y;
	float dz = sphereA.GetPosition().z - sphereB.GetPosition().z;

	// 2点間の距離の取得
	float distanceSquared = dx * dx + dy * dy + dz * dz;

	// 半径の合計の取得
	float radiusSum = sphereA.GetRadius() + sphereB.GetRadius();

	// 高さが同じかつ半径の合計が大きかったら当たっている
	return distanceSquared <= radiusSum * radiusSum ? true : false;
}



/// <summary>
/// 立方体と立方体の当たり判定
/// </summary>
/// <param name="cubeA">立方体A</param>
/// <param name="cubeB">立方体B</param>
/// <returns>当たっているか</returns>
bool IsHit(const CubeCollider& cubeA, const CubeCollider& cubeB)
{
	if (std::abs(cubeA.GetPosition().x - cubeB.GetPosition().x) <= (cubeA.GetExtent().x + cubeB.GetExtent().x) &&
		(std::abs(cubeA.GetPosition().y - cubeB.GetPosition().y) <= (cubeA.GetExtent().y + cubeB.GetExtent().y)) &&
		(std::abs(cubeA.GetPosition().z - cubeB.GetPosition().z) <= (cubeA.GetExtent().z + cubeB.GetExtent().z)))
	{
		return true;
	}

	return false;
}