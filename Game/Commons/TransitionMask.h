//--------------------------------------------------------------------------------------
// File: TransitionMask.h
//
// 画面切り替え用のマスク表示クラス
//
// Date: 2023.9.10
//--------------------------------------------------------------------------------------
#pragma once
#include <SpriteBatch.h>

class TransitionMask
{
public:
	enum class CreateMaskRequest
	{
		NONE,  // リクエストなし
		COPY,  // フレームバッファのコピー
	};


private:
	CreateMaskRequest m_request;

	// 割合(0～1)
	float m_rate;

	int m_flag;

	// オープン又はクローズするまでの時間
	float m_interval;

	// オープンフラグ（trueの場合オープン）
	bool m_open;

	// スプライトバッチ
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;

	// ピクセルシェーダーのポインタ
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_PS_Fade;

	// 定数バッファの構造体
	struct ConstantBuffer
	{
		float rate;
		float flag;
		float pad[2];
	};

	// 定数バッファのポインタ
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer;

	// マスク用テクスチャのポインタ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_maskTexture;

public:

	// コンストラクタ
	TransitionMask(
		ID3D11Device* device,
		ID3D11DeviceContext* context,
		float interval
	);

	// 更新処理
	void Update(float elapsedTime);

	// 描画処理
	void Draw(
		ID3D11DeviceContext* context,
		DirectX::CommonStates* states,
		ID3D11ShaderResourceView* texture,
		const RECT& rect
	);

	// オープンする関数
	void Open();

	// クローズする関数
	void Close();

	// オープン中かチェックする関数
	bool IsOpen() { return m_open; }

	// クローズ中かチェックする関数
	bool IsClose() { return !m_open; }

	// オープン具合を返す関数(0～1)
	float GetOpenRate() { return m_rate; }

	void SetFlag(int flag) { m_flag = flag; }

	// オープン、クローズまでの時間を設定する関数
	void SetInterval(float interval) { m_interval = interval; }

	// リクエストの設定
	void SetCreateMaskRequest(CreateMaskRequest request) { m_request = request; }
	
	// リクエストの取得
	CreateMaskRequest GetCreateMaskRequest() { return m_request; }

	float GetRate()
	{
		if (m_open) return 1.0f - m_rate;
		return m_rate;
	}

	bool IsEnd()
	{
		if (GetRate() == 1.0f) return true;
		return false;
	}
};
