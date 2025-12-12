/// <summary>
/// プレイヤーに関するヘッダファイル
/// </summary>
/// <author>仲森智史</author>
/// <date>2025/05/21</date>

// 多重インクルードの防止
#pragma once

// ヘッダファイルの読み込み
#include "Game/Commons/Interface/IState.h"
#include "Game/Commons/Collision.h"
#include "Game/Commons/UserResources.h"


// クラスの定義
class Ball;



// クラスの定義
class Moving : public IState
{
private:
	// 減速率
	static constexpr float DECELERATIONRATE = 0.999f;

	// 停止する速度
	static constexpr float STOP_SPEED = 0.05f;
	

// 変数
private:
	// ユーザーリソース
	UserResources* m_pUserResources;

	// ボール
	Ball* m_ball;

	// ベーシックエフェクト
	std::unique_ptr<DirectX::BasicEffect> m_basicEffect;

	// プリミティブバッチ
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_primitiveBatch;

	// 入力レイアウトへのポインタ
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;


// 関数
public:
	// コンストラクタ
	Moving(Ball* ball);

	// デストラクタ
	~Moving() override;

	// 初期化
	void Initialize() override;

	// 更新
	void Update(float elapsedTime) override;

	// 描画
	void Render() override;

	// 終了処理
	void Finalize() override;

	// 特定のイベントの処理
	void EventHandle(Event e) override;
};

