/// <summary>
/// EnemyStandingに関するヘッダファイル
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
class Enemy;
class Ball;



// クラスの定義
class EnemyStanding : public IState
{
// 定数
private:


// 変数
private:
	// ユーザーリソース
	UserResources* m_pUserResources;

	// 敵
	Enemy* m_pEnemy;

	// モデル
	DirectX::Model* m_model;  

	// アニメーションボーン配列
	DirectX::ModelBone::TransformArray m_drawBones; 
	// アニメーション
	std::unique_ptr<DX::AnimationSDKMESH> m_animation;  

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
	EnemyStanding(Enemy* pEnemy);

	// デストラクタ
	~EnemyStanding() override;

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
	void AnimationUpdate(float elapsedTime);

	// ボールを持つ
	void CatchHandBall();

	// 近い距離のボールを取得
	Ball* GetNearBall(Ball* ball, int index);
};

