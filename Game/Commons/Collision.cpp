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

#include "Game/Commons/Resources.h"
#include "Game/Commons/Sprite.h"
#include "DebugDraw.h"



// メンバ関数の定義 ===========================================================
//*************************************************************************
// 
// 短形コライダー
// 
//*************************************************************************
/**
 * @brief コンストラクタ
 */
BoxCollider2D::BoxCollider2D()
	: m_center{}
	, m_halfSize{}
	, m_scale(1.0f)
	, m_debug{}
{
	m_debug = std::make_unique<Sprite>();
	m_debug->SetTexture(Resources::GetInstance()->GetTexture(L"debug.png"));
}



/**
 * @brief デストラクタ
 */
BoxCollider2D::~BoxCollider2D()
{
}



/**
 * @brief 描画
 *
 * @param[in] color 描画色
 *
 * @return なし
 */
void BoxCollider2D::Draw(DirectX::FXMVECTOR color)
{
	m_debug->Draw(m_center, DirectX::SimpleMath::Vector2(1, 1), m_halfSize * m_scale * 2, color);
}



/**
 * @brief 左端の取得
 *
 * @param[in] なし
 *
 * @return 左端のX座標
 */
float BoxCollider2D::GetLeft() const
{
	return m_center.x - m_halfSize.x * m_scale;
}



/**
 * @brief 上端の取得
 *
 * @param[in] なし
 *
 * @return 上端のY座標
 */
float BoxCollider2D::GetTop() const
{
	return m_center.y - m_halfSize.y * m_scale;
}



/**
 * @brief 右端の取得
 *
 * @param[in] なし
 *
 * @return 右端のX座標
 */
float BoxCollider2D::GetRight() const
{
	return m_center.x + m_halfSize.x * m_scale;
}



/**
 * @brief 下端の取得
 *
 * @param[in] なし
 *
 * @return 下端のY座標
 */
float BoxCollider2D::GetBottom() const
{
	return m_center.y + m_halfSize.y * m_scale;
}



// メンバ関数の定義 ===========================================================
//*************************************************************************
// 
// 円のコライダー
// 
//*************************************************************************
// コンストラクタ
CircleCollider2D::CircleCollider2D()
	: m_centerX(0.0f)
	, m_centerY(0.0f)
	, m_radius(0.0f)
	, m_debug{}

{
	m_debug = std::make_unique<Sprite>();
	m_debug->SetTexture(Resources::GetInstance()->GetTexture(L"debug.png"));
}



/**
 * @brief デストラクタ
 */
CircleCollider2D::~CircleCollider2D()
{
}



/**
 * @brief 描画
 *
 * @param[in] color 描画色
 *
 * @return なし
 */
void CircleCollider2D::Draw()
{
	m_debug->Draw(DirectX::SimpleMath::Vector2(m_centerX, m_centerY), DirectX::SimpleMath::Vector2(1, 1), m_radius);
}



/**
 * @brief 中心座標(X座標)の取得
 *
 * @param[in] なし
 *
 * @return 中心座標(X座標)
 */
float CircleCollider2D::GetCenterX() const
{
	return m_centerX;
}



/**
 * @brief 中心座標(Y座標)の取得
 *
 * @param[in] なし
 *
 * @return 中心座標(Y座標)
 */
float CircleCollider2D::GetCenterY() const
{
	return m_centerY;
}



/**
 * @brief 半径の取得
 *
 * @param[in] なし
 *
 * @return 半径
 */
float CircleCollider2D::GetRadius() const
{
	return m_radius;
}



/**
 * @brief 円形の中心座標の設定
 *
 * @param[in] centerX 円形の中心座標(X座標)
 * @param[in] centerY 円形の中心座標(Y座標)
 *
 * @return なし
 */
void CircleCollider2D::SetPosition(float centerX, float centerY)
{
	m_centerX = centerX;
	m_centerY = centerY;
}



/**
 * @brief 円形の中心座標の設定
 *
 * @param[in] centerX 矩形の中心座標(X座標)
 * @param[in] centerY 矩形の中心座標(Y座標)
 *
 * @return なし
 */
void CircleCollider2D::SetRadius(float radius)
{
	m_radius = radius;
}



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
	: m_position{ DirectX::SimpleMath::Vector3{0.0f,0.0f,0.0f} }
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
	DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::CreateScale(m_radius * 2) * DirectX::SimpleMath::Matrix::CreateTranslation(m_position);

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
	DirectX::SimpleMath::Matrix world;

	world = DirectX::SimpleMath::Matrix::CreateTranslation(m_position);
	m_cube->Draw(world, view, proj, color);
}



//*************************************************************************
// 
// モデルコライダー
// 
//*************************************************************************
/// <summary>
/// コンストラクタ
/// </summary>
ModelCollider::ModelCollider()
	: m_scale(0.0f)
{
}



/// <summary>
/// デストラクタ
/// </summary>
ModelCollider::~ModelCollider()
{
}



/// <summary>
/// 初期化
/// </summary>
/// <param name="pDevice">デバイス</param>
/// <param name="pContext">コンテキスト</param>
/// <param name="model">モデル</param>
void ModelCollider::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, DirectX::Model* pModel)
{
	// エフェクトの作成
	m_effect = std::make_unique<DirectX::BasicEffect>(pDevice);

	// バッチの作成
	m_batch = std::make_unique<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>>(pContext);

	// CPU にコピー
	for (auto& mesh : pModel->meshes)
	{
		for (auto& part : mesh->meshParts)
		{
			// 頂点バッファの取得
			D3D11_BUFFER_DESC desc{};
			part->vertexBuffer->GetDesc(&desc);

			// スタギングバッファの取得
			D3D11_BUFFER_DESC stagingDesc = desc;
			stagingDesc.Usage = D3D11_USAGE_STAGING;
			stagingDesc.BindFlags = 0;
			stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

			Microsoft::WRL::ComPtr<ID3D11Buffer> stagingVB;
			pDevice->CreateBuffer(&stagingDesc, nullptr, &stagingVB);

			// GPUに転送
			pContext->CopyResource(stagingVB.Get(), part->vertexBuffer.Get());

			// データのポインタの取得
			D3D11_MAPPED_SUBRESOURCE mapped{};
			pContext->Map(stagingVB.Get(), 0, D3D11_MAP_READ, 0, &mapped);

			// データから頂点データを読み取る
			const uint8_t* src = reinterpret_cast<const uint8_t*>(mapped.pData);
			size_t vcount = desc.ByteWidth / part->vertexStride;

			// 一つずつ頂点座標を入れる
			for (size_t i = 0; i < vcount; ++i)
			{
				const uint8_t* v = src + i * part->vertexStride;
				DirectX::VertexPosition vert{};
				memcpy(&vert.position, v, sizeof(float) * 3);
				m_vertices.push_back(vert);
			}

			// 読み取り終了
			pContext->Unmap(stagingVB.Get(), 0);

			// インデックスの取得
			D3D11_BUFFER_DESC idesc{};
			part->indexBuffer->GetDesc(&idesc);
			D3D11_BUFFER_DESC stagingIDesc = idesc;
			stagingIDesc.Usage = D3D11_USAGE_STAGING;
			stagingIDesc.BindFlags = 0;
			stagingIDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

			// スタギングバッファの取得
			Microsoft::WRL::ComPtr<ID3D11Buffer> stagingIB;
			pDevice->CreateBuffer(&stagingIDesc, nullptr, &stagingIB);
			pContext->CopyResource(stagingIB.Get(), part->indexBuffer.Get());

			// データのポインタの取得
			pContext->Map(stagingIB.Get(), 0, D3D11_MAP_READ, 0, &mapped);

			// インデックスが16ビットなら
			if (part->indexFormat == DXGI_FORMAT_R16_UINT)
			{
				// 16ビットのデータの取得
				const uint16_t* idx = reinterpret_cast<const uint16_t*>(mapped.pData);
				// パーツのインデックス分入れる
				for (size_t i = 0; i < part->indexCount; ++i) m_indices.push_back(idx[i]);
			}
			// それ以外なら
			else
			{
				// 32ビットのデータの取得
				const uint32_t* idx = reinterpret_cast<const uint32_t*>(mapped.pData);
				// パーツのインデックス分入れる
				for (size_t i = 0; i < part->indexCount; ++i) m_indices.push_back(idx[i]);
			}

			// 読み取り終了
			pContext->Unmap(stagingIB.Get(), 0);
		}
	}
}



/// <summary>
/// 描画
/// </summary>
/// <param name="pContext">コンテキスト</param>
/// <param name="view">ビュー行列</param>
/// <param name="proj">プロジェクション行列</param>
/// <param name="color">色</param>
void ModelCollider::Draw(ID3D11DeviceContext* pContext, DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj, DirectX::FXMVECTOR color)
{
	// ワールド行列
	DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::CreateScale(m_scale) * DirectX::SimpleMath::Matrix::CreateTranslation(m_position);

	// エフェクト準備
	m_effect->SetWorld(world);
	m_effect->SetView(view);
	m_effect->SetProjection(proj);
	m_effect->Apply(pContext);

	// PrimitiveBatch を使って描画
	m_batch->Begin();

	// 3点をとり三角形を描画(使った頂点を使わないように3個進める)
	for (size_t i = 0; i + 2 < m_indices.size(); i += 3)
	{
		DirectX::VertexPositionColor p0{ m_vertices[m_indices[i    ]].position, (DirectX::SimpleMath::Vector4)color };
		DirectX::VertexPositionColor p1{ m_vertices[m_indices[i + 1]].position, (DirectX::SimpleMath::Vector4)color };
		DirectX::VertexPositionColor p2{ m_vertices[m_indices[i + 2]].position, (DirectX::SimpleMath::Vector4)color };

		//DirectX::SimpleMath::Vector3 edge1 = DirectX::SimpleMath::Vector3(p1.position) - p0.position;
		//DirectX::SimpleMath::Vector3 edge2 = DirectX::SimpleMath::Vector3(p2.position) - p0.position;

		//// 外積で法線を求める
		//DirectX::SimpleMath::Vector3 normal = edge1.Cross(edge2);

		//// 正規化（必須）
		//normal.Normalize();

		//DirectX::SimpleMath::Vector3 center = DirectX::SimpleMath::Vector3(
		//	(p0.position.x + p1.position.x + p2.position.x) / 3,
		//	(p0.position.y + p1.position.y + p2.position.y) / 3,
		//	(p0.position.z + p1.position.z + p2.position.z) / 3
		//);

	/*	DX::DrawRay(m_batch.get(), center, normal / 5, false, DirectX::Colors::Red);*/

		m_batch->DrawLine(p0, p1);
		m_batch->DrawLine(p1, p2);
		m_batch->DrawLine(p2, p0);
	}

	m_batch->End();
}

void ModelCollider::DebugDraw(ID3D11DeviceContext* pContext, DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj, int index, DirectX::FXMVECTOR color)
{
	// ワールド行列
	DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::CreateScale(m_scale) * DirectX::SimpleMath::Matrix::CreateTranslation(m_position);

	// エフェクト準備
	m_effect->SetWorld(world);
	m_effect->SetView(view);
	m_effect->SetProjection(proj);
	m_effect->Apply(pContext);

	// PrimitiveBatch を使って描画
	m_batch->Begin();

	DirectX::VertexPositionColor p0{ m_vertices[m_indices[index]].position, (DirectX::SimpleMath::Vector4)color };
	DirectX::VertexPositionColor p1{ m_vertices[m_indices[index + 1]].position, (DirectX::SimpleMath::Vector4)color };
	DirectX::VertexPositionColor p2{ m_vertices[m_indices[index + 2]].position, (DirectX::SimpleMath::Vector4)color };

	m_batch->DrawLine(p0, p1);
	m_batch->DrawLine(p1, p2);
	m_batch->DrawLine(p2, p0);

	m_batch->End();
}

DirectX::SimpleMath::Vector3 ModelCollider::GetCenterPosition(int index) const
{
	// ワールド座標
	DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::CreateScale(m_scale) * DirectX::SimpleMath::Matrix::CreateTranslation(m_position);

	// 三角形の頂点を求める
	DirectX::SimpleMath::Vector3 p0 = DirectX::SimpleMath::Vector3::Transform(m_vertices[m_indices[index]].position, world);
	DirectX::SimpleMath::Vector3 p1 = DirectX::SimpleMath::Vector3::Transform(m_vertices[m_indices[index + 1]].position, world);
	DirectX::SimpleMath::Vector3 p2 = DirectX::SimpleMath::Vector3::Transform(m_vertices[m_indices[index + 2]].position, world);

	return (p0 + p1 + p2) / 3;
}

DirectX::SimpleMath::Vector3 ModelCollider::GetNormalVector(int index) const
{
	// ワールド座標
	DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::CreateScale(m_scale) * DirectX::SimpleMath::Matrix::CreateTranslation(m_position);

	// 三角形の頂点を求める
	DirectX::SimpleMath::Vector3 p0 = DirectX::SimpleMath::Vector3::Transform(m_vertices[m_indices[index]].position, world);
	DirectX::SimpleMath::Vector3 p1 = DirectX::SimpleMath::Vector3::Transform(m_vertices[m_indices[index + 1]].position, world);
	DirectX::SimpleMath::Vector3 p2 = DirectX::SimpleMath::Vector3::Transform(m_vertices[m_indices[index + 2]].position, world);

	// 辺を求める
	DirectX::SimpleMath::Vector3 edge1 = p1 - p0;
	DirectX::SimpleMath::Vector3 edge2 = p2 - p0;

	// 外積で法線を求める
	DirectX::SimpleMath::Vector3 normal = edge1.Cross(edge2);
	normal.Normalize();

	return normal;
}



//*************************************************************************
// 
// 当たり判定
// 
//*************************************************************************
/// <summary>
/// 矩形コライダーと 矩形コライダーの当たり判定
/// </summary>
/// <param name="boxA">ボックスA</param>
/// <param name="boxB">ボックスB</param>
/// <returns>当たったか</returns>
bool IsHit(const BoxCollider2D& boxA, const BoxCollider2D& boxB)
{
	// 当たり判定
	if ((boxA.GetLeft() < boxB.GetRight()) &&
		(boxA.GetRight() > boxB.GetLeft()) &&
		(boxA.GetTop() < boxB.GetBottom()) &&
		(boxA.GetBottom() > boxB.GetTop()))
	{
		return true;
	}

	return false;
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



/// <summary>
/// 当たり判定
/// </summary>
/// <param name="sphereA">球</param>
/// <param name="modelB">モデル</param>
/// <param name="index">インデックス</param>
/// <returns>当たっているか</returns>
bool IsHit(const SphereCollider& sphere, const ModelCollider& model, int index)
{
	// 球体の座標
	DirectX::SimpleMath::Vector3 sphereCenter = sphere.GetPosition();
	// 球体の半径
	float sphereRadius = sphere.GetRadius();

	// 三角形のワールド座標
	DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::CreateScale(model.GetScale()) * DirectX::SimpleMath::Matrix::CreateTranslation(model.GetPosition());

	// 三角形の点のワールド座標
	DirectX::SimpleMath::Vector3 p0 = DirectX::SimpleMath::Vector3::Transform(model.GetVertices(model.GetIndices(index)).position, world);
	DirectX::SimpleMath::Vector3 p1 = DirectX::SimpleMath::Vector3::Transform(model.GetVertices(model.GetIndices(index + 1)).position, world);
	DirectX::SimpleMath::Vector3 p2 = DirectX::SimpleMath::Vector3::Transform(model.GetVertices(model.GetIndices(index + 2)).position, world);

	DirectX::SimpleMath::Vector3 center = (p0 + p1 + p2) / 3.0f;

	if ((sphereCenter - center).Length() >= 1.0f)
	{
		return false;
	}


	// 法線
	DirectX::SimpleMath::Vector3 N = (p1 - p0).Cross(p2 - p0);
	N.Normalize();

	// 平面距離
	float dist = (sphereCenter - p0).Dot(N);

	// 平面の範囲にあるか
	if (fabs(dist) > sphereRadius)
		return false;

	//// 投影点
	//auto proj = sphereCenter - N * dist;

	//// 三角形内部判定
	//auto edgeTest = [&](DirectX::SimpleMath::Vector3 a, DirectX::SimpleMath::Vector3 b)
	//	{
	//		auto edge = b - a;
	//		auto vp = proj - a;
	//		return edge.Cross(vp).Dot(N) >= 0;
	//	};

	//bool inside = edgeTest(p0, p1) && edgeTest(p1, p2) && edgeTest(p2, p0);

	//if (inside)
	//{
	//	return true;
	//}

	// 辺との距離チェック
	auto closestPointOnSegment = [&](auto a, auto b)
		{
			auto ab = b - a;
			float t = (sphereCenter - a).Dot(ab) / ab.LengthSquared();
			t = std::clamp(t, 0.0f, 1.0f);
			return a + ab * t;
		};

	auto cp0 = closestPointOnSegment(p0, p1);
	auto cp1 = closestPointOnSegment(p1, p2);
	auto cp2 = closestPointOnSegment(p2, p0);

	// 三角形の中に球体が当たっているか
	if ((sphereCenter - cp0).Length() <= sphereRadius ||
		(sphereCenter - cp1).Length() <= sphereRadius ||
		(sphereCenter - cp2).Length() <= sphereRadius)
	{
		return true;
	}

	return false;
}

bool IsHit(const DirectX::SimpleMath::Vector3& rayOrigin, const DirectX::SimpleMath::Vector3& rayDir, const DirectX::SimpleMath::Matrix world, const ModelCollider& model, int index, DirectX::SimpleMath::Vector3& outHitPoint)
{
	// 三角形の点のワールド座標を取得
	DirectX::SimpleMath::Vector3 p0 = DirectX::SimpleMath::Vector3::Transform(model.GetVertices(model.GetIndices(index)).position, world);
	DirectX::SimpleMath::Vector3 p1 = DirectX::SimpleMath::Vector3::Transform(model.GetVertices(model.GetIndices(index + 1)).position, world);
	DirectX::SimpleMath::Vector3 p2 = DirectX::SimpleMath::Vector3::Transform(model.GetVertices(model.GetIndices(index + 2)).position, world);

	DirectX::SimpleMath::Vector3 center = (p0 + p1 + p2) / 3.0f;
	float length = (rayOrigin - center).Length();

	//if (length >= 5.0f)
	//{
	//	return false;
	//}

	// 二つの辺を求める
	DirectX::SimpleMath::Vector3 edge1 = p1 - p0;
	DirectX::SimpleMath::Vector3 edge2 = p2 - p0;

	// 法線ベクトルを求めて角度を出す
	DirectX::SimpleMath::Vector3 pvec = rayDir.Cross(edge2);
	float det = edge1.Dot(pvec);

	// 裏面の当たり判定はしない
	if (fabs(det) < 0.000001f)
		return false;

	float invDet = 1.0f / det;

	// レイの地点がどこにあるか
	DirectX::SimpleMath::Vector3 tvec = rayOrigin - p0;
	float u = tvec.Dot(pvec) * invDet;

	// 三角形の外に出ていないか
	if (u < 0 || u > 1)
		return false;

	// レイが三角形の外に出ていないか
	DirectX::SimpleMath::Vector3 qvec = tvec.Cross(edge1);
	float v = rayDir.Dot(qvec) * invDet;
	if (v < 0 || u + v > 1)
		return false;

	// レイの距離が後ろに飛んでいないか
	float outT = edge2.Dot(qvec) * invDet;
	if (outT < 0)
		return false;

	// レイと三角形の当たった座標を求める
	outHitPoint = rayOrigin + rayDir * outT;
	return true;
}
