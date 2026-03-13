/// <summary>
/// Collisionに関するヘッダファイル
/// </summary>
/// <author>仲森智史</author>


// ヘッダファイルの読み込み
#include "pch.h"
#include "Collision.h"

#include "Game/Commons/Resources.h"
#include "Game/Commons/Sprite.h"
#include "Common/DebugDraw.h"




//////////////////////////////////////////////////////////////////////////
// 四角のコライダー
//////////////////////////////////////////////////////////////////////////
/// <summary>
/// コンストラクタ
/// </summary>
BoxCollider2D::BoxCollider2D()
	: m_center{}
	, m_halfSize{}
	, m_scale(1.0f)
	, m_debug{}
{
	m_debug = std::make_unique<Sprite>();
	m_debug->SetTexture(Resources::GetInstance()->GetTexture(L"debug.png"));
}



/// <summary>
/// デストラクタ
/// </summary>
BoxCollider2D::~BoxCollider2D()
{
}



/// <summary>
/// 描画処理
/// </summary>
/// <param name="color"></param>
void BoxCollider2D::Draw(DirectX::FXMVECTOR color)
{
	m_debug->Draw(m_center, DirectX::SimpleMath::Vector2(1, 1), m_halfSize * m_scale * 2, color);
}



/// <summary>
/// 左端の取得
/// </summary>
/// <returns>左端</returns>
float BoxCollider2D::GetLeft() const
{
	return m_center.x - m_halfSize.x * m_scale;
}



/// <summary>
/// 上端の取得
/// </summary>
/// <returns>上端</returns>
float BoxCollider2D::GetTop() const
{
	return m_center.y - m_halfSize.y * m_scale;
}



/// <summary>
/// 右端の取得
/// </summary>
/// <returns>右端</returns>
float BoxCollider2D::GetRight() const
{
	return m_center.x + m_halfSize.x * m_scale;
}



/// <summary>
/// 下端の取得
/// </summary>
/// <returns>下端</returns>
float BoxCollider2D::GetBottom() const
{
	return m_center.y + m_halfSize.y * m_scale;
}



//////////////////////////////////////////////////////////////////////////
// 円のコライダー
//////////////////////////////////////////////////////////////////////////
/// <summary>
/// コンストラクタ
/// </summary>
CircleCollider2D::CircleCollider2D()
	: m_centerX(0.0f)
	, m_centerY(0.0f)
	, m_radius(0.0f)
	, m_debug{}

{
	m_debug = std::make_unique<Sprite>();
	m_debug->SetTexture(Resources::GetInstance()->GetTexture(L"debug.png"));
}



/// <summary>
/// デストラクタ
/// </summary>
CircleCollider2D::~CircleCollider2D()
{
}



/// <summary>
/// 描画処理
/// </summary>
void CircleCollider2D::Draw()
{
	m_debug->Draw(DirectX::SimpleMath::Vector2(m_centerX, m_centerY), DirectX::SimpleMath::Vector2(1, 1), m_radius);
}



/// <summary>
/// X座標の中心の取得
/// </summary>
/// <returns>X座標の中心</returns>
float CircleCollider2D::GetCenterX() const
{
	return m_centerX;
}



/// <summary>
/// Y座標の中心の取得
/// </summary>
/// <returns>Y座標の中心</returns>
float CircleCollider2D::GetCenterY() const
{
	return m_centerY;
}



/// <summary>
/// 半径の取得
/// </summary>
/// <returns>半径</returns>
float CircleCollider2D::GetRadius() const
{
	return m_radius;
}



/// <summary>
/// 中心座標の設定
/// </summary>
/// <param name="centerX">X座標</param>
/// <param name="centerY">Y座標</param>
void CircleCollider2D::SetPosition(float centerX, float centerY)
{
	m_centerX = centerX;
	m_centerY = centerY;
}



/// <summary>
/// 半径の設定
/// </summary>
/// <param name="radius">半径</param>
void CircleCollider2D::SetRadius(float radius)
{
	m_radius = radius;
}



//////////////////////////////////////////////////////////////////////////
// 球のコライダー
//////////////////////////////////////////////////////////////////////////
/// <summary>
/// コンストラクタ
/// </summary>
SphereCollider::SphereCollider()
	: m_position{ DirectX::SimpleMath::Vector3{0.0f,0.0f,0.0f} }
    , m_radius{ 0.0f }
	, m_sphere{ nullptr }
{

}



/// <summary>
/// デストラクタ
/// </summary>
SphereCollider::~SphereCollider()
{

}



/// <summary>
/// 初期化処理
/// </summary>
/// <param name="pContext">コンテキスト</param>
/// <param name="position">座標</param>
/// <param name="radius">半径</param>
void SphereCollider::Initialize(ID3D11DeviceContext* pContext, DirectX::SimpleMath::Vector3 position, float radius)
{
	// 座標の設定
	m_position = position;

	// 半径の設定
	m_radius = radius;

	// 球の設定
	m_sphere = DirectX::GeometricPrimitive::CreateSphere(pContext, m_radius * 2, 32);
}



/// <summary>
/// 描画処理
/// </summary>
/// <param name="states">共通ステート</param>
/// <param name="view">ビュー行列</param>
/// <param name="proj">プロジェクション行列</param>
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
/// <param name="position">座標</param> 
void SphereCollider::SetPosition(DirectX::SimpleMath::Vector3 position)
{
	m_position = position;
}



/// <summary>
/// 座標の設定
/// </summary>
/// <param name="posX">X座標</param>
/// <param name="posY">Y座標</param>
/// <param name="posZ">Z座標</param> 
void SphereCollider::SetPosition(float posX, float posY, float posZ)
{
	m_position.x = posX;
	m_position.y = posY;
	m_position.z = posZ;
}



/// <summary>
/// 半径の設定
/// </summary>
/// <param name="radius">半径</param> 
void SphereCollider::SetRadius(float radius)
{
	m_radius = radius;
}



/// <summary>
/// 座標の取得
/// </summary>
/// <returns>座標</returns>
DirectX::SimpleMath::Vector3 SphereCollider::GetPosition() const
{
	return m_position;
}



/// <summary>
/// 半径の取得
/// </summary>
/// <returns>半径</returns> 
float SphereCollider::GetRadius() const
{
	return m_radius;
}



//////////////////////////////////////////////////////////////////////////
// 立方体のコライダー
//////////////////////////////////////////////////////////////////////////
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



//////////////////////////////////////////////////////////////////////////
// モデルのコライダー
//////////////////////////////////////////////////////////////////////////
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

		DirectX::SimpleMath::Vector3 edge1 = DirectX::SimpleMath::Vector3(p1.position) - p0.position;
		DirectX::SimpleMath::Vector3 edge2 = DirectX::SimpleMath::Vector3(p2.position) - p0.position;

		// 外積で法線を求める
		DirectX::SimpleMath::Vector3 normal = edge1.Cross(edge2);

		// 正規化（必須）
		normal.Normalize();

		DirectX::SimpleMath::Vector3 center = DirectX::SimpleMath::Vector3(
			(p0.position.x + p1.position.x + p2.position.x) / 3,
			(p0.position.y + p1.position.y + p2.position.y) / 3,
			(p0.position.z + p1.position.z + p2.position.z) / 3
		);

		DX::DrawRay(m_batch.get(), center, normal / 5, false, DirectX::Colors::Red);

		m_batch->DrawLine(p0, p1);
		m_batch->DrawLine(p1, p2);
		m_batch->DrawLine(p2, p0);
	}

	m_batch->End();
}



/// <summary>
/// デバック用の描画
/// </summary>
/// <param name="pContext">コンテキスト</param>
/// <param name="view">ビュー行列</param>
/// <param name="proj">プロジェクション行列</param>
/// <param name="index">番号</param>
/// <param name="color">色</param>
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



/// <summary>
/// 中心座標の取得
/// </summary>
/// <param name="index">番号</param>
/// <returns>中心座標</returns>
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



/// <summary>
/// 法線ベクトルの取得
/// </summary>
/// <param name="index">番号</param>
/// <returns>法線ベクトル</returns>
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



//////////////////////////////////////////////////////////////////////////
// 当たり判定のコライダー
//////////////////////////////////////////////////////////////////////////
/// <summary>
/// 矩形コライダーと矩形コライダーの当たり判定
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
/// 球体とモデルの当たり判定
/// </summary>
/// <param name="sphereA">球</param>
/// <param name="modelB">モデル</param>
/// <param name="index">インデックス</param>
/// <returns>当たっているか</returns>
bool IsHit(const SphereCollider& sphere, const DirectX::SimpleMath::Vector3& p0, const DirectX::SimpleMath::Vector3& p1, const DirectX::SimpleMath::Vector3& p2)
{
	// 球体の座標
	DirectX::SimpleMath::Vector3 sphereCenter = sphere.GetPosition();
	// 球体の半径
	float sphereRadius = sphere.GetRadius();

	// 距離が長いとfalseにする
	DirectX::SimpleMath::Vector3 center = (p0 + p1 + p2) / 3.0f;

	// 法線
	DirectX::SimpleMath::Vector3 N = (p1 - p0).Cross(p2 - p0);
	N.Normalize();

	// 平面距離
	float dist = (sphereCenter - p0).Dot(N);

	// 平面の範囲にあるか
	if (fabs(dist) > sphereRadius)
		return false;

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



/// <summary>
/// レイとモデルの当たり判定
/// </summary>
/// <param name="rayOrigin">レイの座標</param>
/// <param name="rayDir">レイの方向</param>
/// <param name="world">ワールド座標</param>
/// <param name="model">モデル</param>
/// <param name="index">番号</param>
/// <param name="outHitPoint">当たった座標</param>
/// <returns>当たったか</returns>
bool IsHit(const DirectX::SimpleMath::Vector3& rayOrigin, const DirectX::SimpleMath::Vector3& rayDir,
	const DirectX::SimpleMath::Vector3& p0, const DirectX::SimpleMath::Vector3& p1, const DirectX::SimpleMath::Vector3& p2, DirectX::SimpleMath::Vector3& outHitPoint)
{
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
