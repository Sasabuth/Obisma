/// <summary>
/// EnemyRunningに関するヘッダファイル
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
class IEntity;
class Enemy;
class Ball;



// クラスの定義
class EnemyRunning : public IState
{
private:
	static constexpr float ENEMY_SPEED = 2.2f;

	// 変数
private:
	UserResources* m_userResources;

	Enemy* m_enemy;

	DirectX::Model* m_model;  // モデル

	DirectX::ModelBone::TransformArray m_drawBones;  // アニメーションボーン配列

	std::unique_ptr<DX::AnimationSDKMESH> m_animation;  // アニメーション

	DirectX::SimpleMath::Matrix m_rightHandMatrix; // 右手のマトリックス
	DirectX::SimpleMath::Matrix m_leftHandMatrix;  // 左手のマトリックス

	std::unique_ptr<DirectX::BasicEffect> m_basicEffect;  // ベーシックエフェクト

	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_primitiveBatch;  // プリミティブバッチ

	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;  // 入力レイアウトへのポインタ


	// 関数
public:
	// コンストラクタ
	EnemyRunning(Enemy* enemy);

	// デストラクタ
	~EnemyRunning() override;

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

	// ボールの方向に走る
	void RunToBall();

	// 実体の方向に走る
	void RunToEntity();

	// ボールを持つ
	void CatchHandBall();

	// ボールを投げる
	void ThrowBall();

	// 一番近い実体を探す
	IEntity* NearEntity();
};

