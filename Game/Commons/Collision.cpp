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
#include "Game/Commons/Collision.h"


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
	, m_halfSize{ 0.0f }
	, m_cube{ nullptr }
{
}



/// <summary>
/// デストラクタ
/// </summary>
CubeCollider::~CubeCollider()
{
}

void CubeCollider::Initialize(ID3D11DeviceContext* pContext, DirectX::SimpleMath::Vector3 position, float size)
{
	// 座標の設定
	m_position = position;

	// サイズの設定
	m_halfSize = size / 2;

	m_cube = DirectX::GeometricPrimitive::CreateCube(pContext, m_halfSize * 2);
}

void CubeCollider::Draw(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj)
{
	SimpleMath::Matrix world;

	world = SimpleMath::Matrix::CreateTranslation(m_position);
	m_cube->Draw(world, view, proj);
}

void CubeCollider::SetPosition(DirectX::SimpleMath::Vector3 position)
{
	m_position = position;
}

void CubeCollider::SetPosition(float posX, float posY, float posZ)
{
	m_position.x = posX;
	m_position.y = posY;
	m_position.z = posZ;
}

void CubeCollider::SetSize(float size)
{
	m_halfSize = size / 2;
}

DirectX::SimpleMath::Vector3 CubeCollider::GetPosition() const
{
	return m_position;
}

float CubeCollider::GetMinX() const
{
	return m_position.x - m_halfSize;
}

float CubeCollider::GetMaxX() const
{
	return m_position.x + m_halfSize;
}

float CubeCollider::GetMinY() const
{
	return m_position.y - m_halfSize;
}

float CubeCollider::GetMaxY() const
{
	return m_position.y + m_halfSize;
}

float CubeCollider::GetMinZ() const
{
	return m_position.z - m_halfSize;
}

float CubeCollider::GetMaxZ() const
{
	return m_position.z + m_halfSize;
}



/// <summary>
/// 円と円の当たり判定
/// </summary>
/// <param name="sphereA"></param> 球A
/// <param name="sphereB"></param> 球B
/// <returns></returns>  当たっているか
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

bool IsHit(const CubeCollider& cubeA, const CubeCollider& cubeB)
{
	if (cubeA.GetPosition().x >= cubeB.GetMinX() &&
		cubeA.GetPosition().x <= cubeB.GetMaxX() &&
		cubeA.GetPosition().y >= cubeB.GetMinY() &&
		cubeA.GetPosition().y <= cubeB.GetMaxY() /*&&
		cubeA.GetPosition().z >= cubeB.GetMinZ() &&
		cubeA.GetPosition().z <= cubeB.GetMaxZ()*/)
	{
		return true;
	}

	return false;
}