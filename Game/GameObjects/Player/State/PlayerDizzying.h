/// <summary>
/// PlayerDizzyingに関するヘッダファイル
/// </summary>
/// <author>仲森智史</author>

// 多重インクルードの防止
#pragma once

// ヘッダファイルの読み込み
#include "Game/Commons/Interface/IState.h"
#include "Game/Commons/Collision.h"
#include "Game/Commons/UserResources.h"
#include "Game/Commons/Animation.h"


// クラスの定義
class Player;


// クラスの定義
class PlayerDizzying : public IState
{
// 定数
private:
	// インターバル
	static constexpr float INTERVAL = 3.0f;
	// アニメーションの終了時間
	static constexpr float ANIMATION_ENDTIME = 1.2f;
	// アニメーションの再開時間
	static constexpr float ANIMATION_RESTARTTIME = 0.19f;


// 変数
private:
	// ユーザーリソース
	UserResources* m_pUserResources;

	// プレイヤー
	Player* m_pPlayer;

	// モデル
	DirectX::Model* m_model;

	// アニメーションボーン配列
	DirectX::ModelBone::TransformArray m_drawBones;
	// アニメーション
	std::unique_ptr<DX::AnimationSDKMESH> m_animation;

	// 時間
	float m_time;

	// エフェクト入れたか
	bool m_isEffect;

	// 右手のマトリックス
	DirectX::SimpleMath::Matrix m_rightHandMatrix;
	// 左手のマトリックス
	DirectX::SimpleMath::Matrix m_leftHandMatrix;

	// ベーシックエフェクト
	std::unique_ptr<DirectX::BasicEffect> m_basicEffect;

	// プリミティブバッチ
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_primitiveBatch;

	// 入力レイアウトへのポインタ
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;


// 関数
public:
	// コンストラクタ
	PlayerDizzying(Player* pPlayer);

	// デストラクタ
	~PlayerDizzying() override;

	// 初期化
	void Initialize() override;

	// 更新
	void Update(float elapsedTime) override;

	// 描画
	void Render() override;

	// 終了処理
	void Finalize() override;


// 内部処理
private:
	// アニメーションの更新
	void AnimationUpdate();
};

