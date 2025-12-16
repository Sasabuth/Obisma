//--------------------------------------------------------------------------------------
// File: Particle.h
//
// パーティクルクラス
//
//-------------------------------------------------------------------------------------

#pragma once

#include "StepTimer.h"
#include <DeviceResources.h>
#include <SimpleMath.h>
#include <Effects.h>
#include <PrimitiveBatch.h>
#include <VertexTypes.h>
#include <WICTextureLoader.h>
#include <CommonStates.h>
#include <vector>
#include <list>
#include <random>

#include "Game/GameObjects/Particle/ParticleUtility.h"

class Field;


class Particle
{
public:
	//	データ受け渡し用コンスタントバッファ(送信側)
	struct ConstBuffer
	{
		DirectX::SimpleMath::Matrix		matWorld;
		DirectX::SimpleMath::Matrix		matView;
		DirectX::SimpleMath::Matrix		matProj;
		DirectX::SimpleMath::Vector4	Diffuse;
	};

	static constexpr float LINE_SCALE = 0.62f;

private:
	// ランダムデバイス 
	std::random_device m_rd;

	Microsoft::WRL::ComPtr<ID3D11Buffer>	m_CBuffer;

	//	入力レイアウト
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

	//	プリミティブバッチ
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColorTexture>> m_batch;
	//	テクスチャハンドル
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;
	//	頂点情報のリスト
	std::vector<DirectX::VertexPositionColorTexture> m_vertices;
	//	パーティクルに使う情報を保存するためのクラスオブジェクト用リスト
	std::list<std::unique_ptr<ParticleUtility>> m_particleUtility;
	//	カメラに関する情報
	DirectX::SimpleMath::Vector3 m_cameraPosition;
	DirectX::SimpleMath::Vector3 m_cameraTarget;

	//	頂点シェーダ
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
	//	ピクセルシェーダ
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
	//	ジオメトリシェーダ
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> m_geometryShader;

	DirectX::SimpleMath::Matrix m_world;

	DirectX::SimpleMath::Matrix m_billboard;

public:
	//	関数
	static const std::vector<D3D11_INPUT_ELEMENT_DESC> INPUT_LAYOUT;

	Particle();
	~Particle();

	void Create(ID3D11Device1* device, ID3D11DeviceContext1* context);

	void Update(float elapsedTime);

	void Render(ID3D11DeviceContext1* context, DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj);

	void CreateBillboard(
		DirectX::SimpleMath::Vector3 target,
		DirectX::SimpleMath::Vector3 eye,
		DirectX::SimpleMath::Vector3 up);

	void SetEffectPosition(ID3D11DeviceContext* pContext, DirectX::SimpleMath::Vector3 pos);
	void SetEffectPosition(ID3D11DeviceContext* pContext, float life, DirectX::SimpleMath::Vector3 pos, DirectX::SimpleMath::Vector3 gravity);

	// 当たり判定
	void HandleFieldCollision(Field& field);
	void HandleStageCollision(Field& field);

	void ColliderDraw(DirectX::CommonStates* states, DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj);


private:

	void CreateShader(ID3D11Device1* device);

};
