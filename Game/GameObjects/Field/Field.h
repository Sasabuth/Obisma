/// <summary>
/// Fieldに関するヘッダファイル
/// </summary>
/// <author>仲森智史</author>

// 多重インクルードの防止
#pragma once

// ヘッダファイルの読み込み
#include "Game/Commons/Collision.h"
#include "Game/Commons/UserResources.h"

// クラスの定義
class IEntity;


// クラスの定義
class Field
{
// 定数
private:
	static constexpr float MODEL_SCALE = 3.0f;


// 変数
private:
	// ユーザーリソース
	UserResources* m_pUserResources;

	// モデル
	DirectX::Model* m_model;

	// スカイドーム
	DirectX::Model* m_skydomeModel; 

	// 座標
	DirectX::SimpleMath::Vector3 m_position;  

	// コライダー
	SphereCollider m_collider; 

	// 回転
	float m_rotate;


// 関数
public:
	// コンストラクタ
	Field();

	// デストラクタ
	~Field();

	// 初期化
	void Initialize();

	// 更新
	void Update(float elapsedTime);

	// 描画
	void Render();

	// 終了処理
	void Finalize();

	// 上方向の補正
	DirectX::SimpleMath::Vector3 CorrectUp(IEntity* iEntity);


// 設定/取得
public:
	// コライダーの取得
	SphereCollider& GetCollider();

	// 回転の設定
	void SetRotate(float rotate) { m_rotate = rotate; }
};

