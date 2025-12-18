//--------------------------------------------------------------------------------------
// File: TransitionMask.cpp
//
// 画面切り替え用のマスク表示クラス
//
// Date: 2023.9.10
//--------------------------------------------------------------------------------------
#include "pch.h"
#include "TransitionMask.h"
#include "Game/Commons/BinaryFile.h"
#include "Game/Commons/Resources.h"

using namespace DirectX;

// コンストラクタ
TransitionMask::TransitionMask(
	ID3D11Device* device,
	ID3D11DeviceContext* context,
	float interval
)
	: m_interval(interval)
	, m_rate(0.0f)
	, m_flag(0)
	, m_request(TransitionMask::CreateMaskRequest::NONE)
	, m_open(true)
{
	// スプライトバッチの作成
	m_spriteBatch = std::make_unique<DirectX::SpriteBatch>(context);

	// マスク用テクスチャの読み込み
	m_maskTexture = Resources::GetInstance()->GetDDSTexture(L"FadeMask.dds");

	// ピクセルシェーダーの作成
	std::unique_ptr<BinaryFile> PSData = BinaryFile::LoadFile(L"Resources/Shaders/FadePS.cso");


	DX::ThrowIfFailed(
		device->CreatePixelShader(
			PSData->GetData(),
			PSData->GetSize(),
			nullptr,
			m_PS_Fade.ReleaseAndGetAddressOf()
		)
	);

	// 定数バッファの作成
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = static_cast<UINT>(sizeof(ConstantBuffer));
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	DX::ThrowIfFailed(
		device->CreateBuffer(&bufferDesc, nullptr, m_constantBuffer.ReleaseAndGetAddressOf())
	);
}

// 更新処理
void TransitionMask::Update(float elapsedTime)
{
	if (m_open)
	{
		// オープン
		// (減算量をインターバルで割る＝インターバル分の時間にする)
		// ０以下になったら０にする

		m_rate -= elapsedTime / m_interval;

		if (m_rate < 0.0f) m_rate = 0.0f;
	}
	else
	{
		// クローズ
		// (加算量をインターバルで割る＝インターバル分の時間にする)
		// 1以上になったら１にする

		m_rate += elapsedTime / m_interval;

		if (m_rate > 1.0f) m_rate = 1.0f;
	}
}

// 描画処理
void TransitionMask::Draw(
	ID3D11DeviceContext* context,
	DirectX::CommonStates* states,
	ID3D11ShaderResourceView* texture,
	const RECT& rect
)
{
	if (m_rate == 0.0f) return;

	//m_spriteBatch->Begin(SpriteSortMode_Immediate, states->NonPremultiplied());

	//// マスクの描画
	//m_spriteBatch->Draw(texture, rect, SimpleMath::Color(1.0f, 1.0f, 1.0f, m_rate));

	//m_spriteBatch->End();

	D3D11_MAPPED_SUBRESOURCE mappedResource;

	context->Map(m_constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	static_cast<ConstantBuffer*>(mappedResource.pData)->rate = m_rate;
	static_cast<ConstantBuffer*>(mappedResource.pData)->flag = m_flag;
	context->Unmap(m_constantBuffer.Get(), 0);

	m_spriteBatch->Begin(
		SpriteSortMode_Immediate, 
		states->NonPremultiplied(),
		nullptr,
		nullptr,
		nullptr,
		[&]()
		{
			// ピクセルシェーダー
			context->PSSetShader(m_PS_Fade.Get(), nullptr, 0);

			// マスク用テクスチャの設定
			context->PSSetShaderResources(1, 1, m_maskTexture.GetAddressOf());

			// 定数バッファの設定
			ID3D11Buffer* cbuf = { m_constantBuffer.Get() };
			context->PSSetConstantBuffers(1, 1, &cbuf);
		}
	);

	// マスクの描画
	m_spriteBatch->Draw(texture, rect);

	m_spriteBatch->End();
}

// マスクをオープンする関数
void TransitionMask::Open()
{
	m_open = true;
	m_rate = 1.0f;
}

// マスクをクローズする関数
void TransitionMask::Close()
{
	m_open = false;
	m_rate = 0.0f;
}

//// オープンしているかチェックする関数
//bool TransitionMask::IsOpen()
//{
//	if (m_open && m_rate == 0.0f) return true;
//	return false;
//}
//
//// クローズしているかチェックする関数
//bool TransitionMask::IsClose()
//{
//	if (!m_open && m_rate == 1.0f) return true;
//	return false;
//}
