/// <summary>
/// Throwingに関するヘッダファイル
/// </summary>
/// <author>仲森智史</author>
/// <date>2025/07/16</date>

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
class Throwing : public IState
{
private:


	// 変数
private:
	UserResources* m_userResources;

	Player* m_player;

	DirectX::Model* m_model;  // モデル

	DirectX::ModelBone::TransformArray m_drawBones;  // アニメーションボーン配列

	std::unique_ptr<DX::AnimationSDKMESH> m_animation;  // アニメーション

	// ワールドマトリックス
	DirectX::SimpleMath::Matrix m_worldMatrix;
	DirectX::SimpleMath::Matrix m_boneMatrix;

	float m_time;
	bool m_isThowing;

	std::unique_ptr<DirectX::BasicEffect> m_basicEffect;  // ベーシックエフェクト

	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_primitiveBatch;  // プリミティブバッチ

	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;  // 入力レイアウトへのポインタ


	// 関数
public:
	// コンストラクタ
	Throwing(Player* player);

	// デストラクタ
	~Throwing() override;

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
};

