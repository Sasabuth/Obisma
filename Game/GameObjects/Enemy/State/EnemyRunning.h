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



// クラスの定義
class EnemyRunning : public IState
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

	// ターゲット時間
	float m_targetTime;

	// 投げる距離
	float m_throwDistance;


// 関数
public:
	// コンストラクタ
	EnemyRunning(Enemy* pEnemy);

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

	// ボールがキャッチできるか
	bool IsBallCatch() const;

};

