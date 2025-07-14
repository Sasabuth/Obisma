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
#include "Game/Commons/Animation.h"


// クラスの定義
class Player;



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

	// ワールドマトリックス
	DirectX::SimpleMath::Matrix m_worldMatrix;
	DirectX::SimpleMath::Matrix m_boneMatrix;

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
};

