//--------------------------------------------------------------------------------------
// File: Particle.cpp
//
// パーティクルクラス
//
//-------------------------------------------------------------------------------------

#include "pch.h"
#include "Particle.h"

#include "Game/Commons/Resources.h"
#include "Game/Commons/BinaryFile.h"
#include "Game/GameObjects/Field/Field.h"
#include <algorithm>
#include <random>


/// <summary>
/// インプットレイアウト
/// </summary>
const std::vector<D3D11_INPUT_ELEMENT_DESC> Particle::INPUT_LAYOUT =
{
	{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0,							 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0,	sizeof(DirectX::SimpleMath::Vector3), D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0, sizeof(DirectX::SimpleMath::Vector3) + sizeof(DirectX::SimpleMath::Vector4), D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

/// <summary>
/// コンストラクタ
/// </summary>
Particle::Particle()
{
}

/// <summary>
/// デストラクタ
/// </summary>
Particle::~Particle()
{
}



/// <summary>
/// 生成関数
/// </summary>
/// <param name="pDR">ユーザーリソース等から持ってくる</param>
void Particle::Create(ID3D11Device1* device, ID3D11DeviceContext1* context)
{
	//	シェーダーの作成
	CreateShader(device);

	// テクスチャの読み込み
	m_texture = Resources::GetInstance()->GetTexture(L"Ster.png");

	//	プリミティブバッチの作成
	m_batch = std::make_unique<DirectX::PrimitiveBatch<DirectX::VertexPositionColorTexture>>(context);
}



/// <summary>
/// 更新関数
/// </summary>
/// <param name="elapsedTime">経過時間</param>
void Particle::Update(float elapsedTime)
{
	//	timerを渡してm_effectの更新処理を行う
	for (std::list<std::unique_ptr<ParticleUtility>>::iterator ite = m_particleUtility.begin(); ite != m_particleUtility.end(); ite++)
	{
		//	更新結果の戻り値（true / false）をチェック
		if (!(*ite)->Update(elapsedTime))
		{
			//	falseが返ってきたら、消す
			ite = m_particleUtility.erase(ite);

			if (ite == m_particleUtility.end())
			{
				//	最後のオブジェクトを消したので、ループ終了
				break;
			}
		}
	}
}



/// <summary>
/// Shader作成部分だけ分離した関数
/// </summary>
void Particle::CreateShader(ID3D11Device1* device)
{
	//	コンパイルされたシェーダファイルを読み込み
	std::unique_ptr<BinaryFile> VSData = BinaryFile::LoadFile(L"Resources/Shaders/ParticleVS.cso");
	std::unique_ptr<BinaryFile> PSData = BinaryFile::LoadFile(L"Resources/Shaders/ParticlePS.cso");
	std::unique_ptr<BinaryFile> GSData = BinaryFile::LoadFile(L"Resources/Shaders/ParticleGS.cso");

	//	インプットレイアウトの作成
	device->CreateInputLayout(&INPUT_LAYOUT[0],
		static_cast<UINT>(INPUT_LAYOUT.size()),
		VSData->GetData(), VSData->GetSize(),
		m_inputLayout.GetAddressOf());
	

	//	頂点シェーダ作成
	if (FAILED(device->CreateVertexShader(VSData->GetData(), VSData->GetSize(), NULL, m_vertexShader.ReleaseAndGetAddressOf())))
	{// エラー
		MessageBox(0, L"CreateVertexShader Failed.", NULL, MB_OK);
		return;
	}

	//	ピクセルシェーダ作成
	if (FAILED(device->CreatePixelShader(PSData->GetData(), PSData->GetSize(), NULL, m_pixelShader.ReleaseAndGetAddressOf())))
	{// エラー
		MessageBox(0, L"CreatePixelShader Failed.", NULL, MB_OK);
		return;
	}

	//	ジオメトリシェーダ作成
	if (FAILED(device->CreateGeometryShader(GSData->GetData(), GSData->GetSize(), NULL, m_geometryShader.ReleaseAndGetAddressOf())))
	{// エラー
		MessageBox(0, L"CreateGeometryShader Failed.", NULL, MB_OK);
		return;
	}

	//	シェーダーにデータを渡すためのコンスタントバッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	device->CreateBuffer(&bd, nullptr, &m_CBuffer);
}

void Particle::SetEffectPosition(ID3D11DeviceContext* pContext, DirectX::SimpleMath::Vector3 pos)
{
	//	新しいパーティクル情報を作成する
	std::unique_ptr<ParticleUtility> pU = std::make_unique<ParticleUtility>(
		pContext,
		2.3f,																							//	生存時間(s)
		pos,						                                                                    //	基準座標
		DirectX::SimpleMath::Vector3::Zero,													            //	速度
		DirectX::SimpleMath::Vector3::Zero,																//	加速度
		DirectX::SimpleMath::Vector3(LINE_SCALE), DirectX::SimpleMath::Vector3(0.1f),					//	初期スケール、最終スケール
		DirectX::SimpleMath::Color(1.0f, 0.5f, 0.0f, 0.5f), DirectX::SimpleMath::Color(1.0f, 1.0f, 1.0f, 0.f)//	初期カラー、最終カラー
	);

	//	リストに追加
	m_particleUtility.push_back(std::move(pU));
}



/// <summary>
/// エフェクトの座標の設定
/// </summary>
/// <param name="pContext">コンテキスト</param>
/// <param name="pos">座標</param>
/// <param name="gravity">重力</param>
void Particle::SetEffectPosition(ID3D11DeviceContext* pContext, float life, DirectX::SimpleMath::Vector3 pos, DirectX::SimpleMath::Vector3 gravity)
{
	// ランダムの数の取得
	std::mt19937 mt(m_rd());
	std::uniform_int_distribution<> rand(-2, 2);

	//	新しいパーティクル情報を作成する
	std::unique_ptr<ParticleUtility> pU = std::make_unique<ParticleUtility>(
		pContext,
		life,																							//	生存時間(s)
		pos,						                                                                    //	基準座標
		DirectX::SimpleMath::Vector3((float)rand(mt), (float)rand(mt), (float)rand(mt)),				//	速度
		gravity,																                        //	加速度
		DirectX::SimpleMath::Vector3(LINE_SCALE), DirectX::SimpleMath::Vector3(0.1f),					//	初期スケール、最終スケール
		DirectX::SimpleMath::Color(1.0f, 1.0, 0.0f, 0.7f), DirectX::SimpleMath::Color(1.0f, 1.0f, 1.0f, 0.f)//	初期カラー、最終カラー
	);

	//	リストに追加
	m_particleUtility.push_back(std::move(pU));
}



/// <summary>
/// フィールドとの当たり判定
/// </summary>
/// <param name="field"></param>
void Particle::HandleFieldCollision(Field& field)
{
	// パーティクルのイテレータの取得
	for (std::list<std::unique_ptr<ParticleUtility>>::iterator ite = m_particleUtility.begin(); ite != m_particleUtility.end(); ite++)
	{
		// パーティクルの当たり判定を取り,当たったたら跳ね返る
		if (IsHit(field.GetCollider(), (*ite)->GetCollider()))
		{
			(*ite)->CorrectOverlap(field);
		}
	}
}



/// <summary>
/// コライダーの描画
/// </summary>
/// <param name="states">共通ステート</param>
/// <param name="view">ビュー行列</param>
/// <param name="proj">プロジェクション行列</param>
void Particle::ColliderDraw(DirectX::CommonStates* states, DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj)
{
	for (std::list<std::unique_ptr<ParticleUtility>>::iterator ite = m_particleUtility.begin(); ite != m_particleUtility.end(); ite++)
	{
		(*ite)->GetCollider().Draw(states, view, proj);
	}
}



/// <summary>
/// 描画関数
/// </summary>
/// <param name="view">ビュー行列</param>
/// <param name="proj">射影行列</param>
void Particle::Render(ID3D11DeviceContext1* context, DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj)
{
	auto states = UserResources::GetUserResource()->GetCommonStates();

	//	ビルボード設定時にもらったカメラ情報から、視線ベクトルを計算する
	DirectX::SimpleMath::Vector3 cameraDir = m_cameraTarget - m_cameraPosition;
	//	視線ベクトルは正規化しておく
	cameraDir.Normalize();

	//	パーティクル情報を、カメラからの距離順でソートする
	m_particleUtility.sort(
		//	ソート処理の基準を示す関数については、ラムダ式で指定する
		[&](const std::unique_ptr<ParticleUtility>& lhs,
			const std::unique_ptr<ParticleUtility>& rhs)
		{
			//	カメラ正面の距離でソート
			return cameraDir.Dot(lhs->GetPosition() - m_cameraPosition) > cameraDir.Dot(rhs->GetPosition() - m_cameraPosition);
		});

	//	表示に使う頂点リストに登録されているデータを全削除
	m_vertices.clear();
	//	パーティクル情報から、表示に使う頂点リストを生成する
	for (auto& li : m_particleUtility)
	{
		if (cameraDir.Dot(li->GetPosition() - m_cameraPosition) < 0.0f) {
			//	内積の結果がマイナスの場合はカメラの後ろなので表示する必要なし
			continue;
		}

		DirectX::VertexPositionColorTexture vPCT;
		// 座標を入れる
		vPCT.position = DirectX::XMFLOAT3(li->GetPosition());
		//	テクスチャの色
		vPCT.color = DirectX::XMFLOAT4(li->GetNowColor());
		// サイズ
		vPCT.textureCoordinate = DirectX::XMFLOAT2(li->GetNowScale().x, li->GetNowScale().y);
		//	頂点情報を入れる
		m_vertices.push_back(vPCT);
	}

	//	表示する点がない場合は描画を終わる
	if (m_vertices.empty())
	{
		return;
	}

	//	シェーダーに渡す追加のバッファを作成
	ConstBuffer cbuff;
	//	ビュー行列の取得
	cbuff.matView = view.Transpose();
	//	プロジェクション行列の取得
	cbuff.matProj = proj.Transpose();
	//	ワールド行列の取得
	cbuff.matWorld = m_world.Transpose();
	cbuff.Diffuse = DirectX::SimpleMath::Vector4(1, 1, 1, 1);

	//	受け渡し用バッファの内容更新(ConstBufferからID3D11Bufferへの変換）
	context->UpdateSubresource(m_CBuffer.Get(), 0, NULL, &cbuff, 0, 0);

	//	シェーダーにバッファを渡す
	ID3D11Buffer* cb[1] = { m_CBuffer.Get() };
	//	頂点シェーダもピクセルシェーダも、同じ値を渡す
	context->VSSetConstantBuffers(0, 1, cb);
	context->PSSetConstantBuffers(0, 1, cb);
	context->GSSetConstantBuffers(0, 1, cb);

	//	画像用サンプラーの登録
	ID3D11SamplerState* sampler[1] = { states->LinearWrap() };
	context->PSSetSamplers(0, 1, sampler);

	//	半透明描画指定
	ID3D11BlendState* blendstate = states->NonPremultiplied();

	//	透明判定処理
	context->OMSetBlendState(blendstate, nullptr, 0xFFFFFFFF);

	//	深度バッファに書き込み参照する
	context->OMSetDepthStencilState(states->DepthRead(), 0);

	//	カリングはなし
	context->RSSetState(states->CullNone());

	//	シェーダをセットする
	context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
	context->PSSetShader(m_pixelShader.Get(), nullptr, 0);
	context->GSSetShader(m_geometryShader.Get(), nullptr, 0);

	//	for文で一気に設定する
	context->PSSetShaderResources(0, 1, m_texture.GetAddressOf());

	//	インプットレイアウトの登録
	context->IASetInputLayout(m_inputLayout.Get());

	//	板ポリゴンを描画
	m_batch->Begin();
	//	ジオメトリシェーダでPointを受け取ることになっているため、
	//	ここではD3D11_PRIMITIVE_TOPOLOGY_POINTLISTを使う
	m_batch->Draw(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST, m_vertices.data(), m_vertices.size());
	m_batch->End();

	

	//	シェーダの登録を解除しておく
	context->VSSetShader(nullptr, nullptr, 0);
	context->PSSetShader(nullptr, nullptr, 0);
	context->GSSetShader(nullptr, nullptr, 0);
}



/// <summary>
/// ビルボード作成関数
/// </summary>
/// <param name="target">カメラターゲット（注視点）</param>
/// <param name="eye">カメラアイ（カメラ座標）</param>
/// <param name="up">上向きベクトル（基本はYのみ１のベクトル）</param>
void Particle::CreateBillboard(DirectX::SimpleMath::Vector3 target, DirectX::SimpleMath::Vector3 eye, DirectX::SimpleMath::Vector3 up)
{
	m_billboard =
		DirectX::SimpleMath::Matrix::CreateBillboard(DirectX::SimpleMath::Vector3::Zero, eye - target, up);

	//	回転情報を設定する
	DirectX::SimpleMath::Matrix rot = DirectX::SimpleMath::Matrix::Identity;
	//	180度逆を向いているので、補正するための情報を入れる
	rot._11 = -1;
	rot._33 = -1;

	m_cameraPosition = eye;
	m_cameraTarget = target;
	m_billboard = rot * m_billboard;

	m_world = m_billboard;
}
