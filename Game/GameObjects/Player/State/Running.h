/// <summary>
/// Runningに関するヘッダファイル
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
class Ball;



// クラスの定義
class Running : public IState
{
private:
	static constexpr float PLAYER_SPEED = 2.0f;

// 変数
private:
	UserResources* m_userResources;

	Player* m_player;

	DirectX::Model* m_model;  // モデル

	DirectX::ModelBone::TransformArray m_drawBones;  // アニメーションボーン配列

	std::unique_ptr<DX::AnimationSDKMESH> m_animation;  // アニメーション

	DirectX::SimpleMath::Matrix m_rightHandMatrix;  // 右手のマトリックス
	DirectX::SimpleMath::Matrix m_leftHandMatrix;  // 左手のマトリックス

	std::unique_ptr<DirectX::BasicEffect> m_basicEffect;  // ベーシックエフェクト

	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_primitiveBatch;  // プリミティブバッチ

	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;  // 入力レイアウトへのポインタ


// 関数
public:
	// コンストラクタ
	Running(Player* player);

	// デストラクタ
	~Running() override;

	// 初期化
	void Initialize() override;

	// 更新
	void Update(float elapsedTime) override;

	// 描画
	void Render() override;

	// 終了処理
	void Finalize() override;

private:
	// アニメーションの更新
	void AnimationUpdate(float elapsedTime); 

	// ボールを持つ
	void CatchHandBall();

	// ボールを投げる
	void ThrowBall();

	// マウス方向の回転の更新
	void UpdateRotateToMouse();
};

