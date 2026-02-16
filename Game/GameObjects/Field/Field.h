/// <summary>
/// Fieldに関するヘッダファイル
/// </summary>
/// <author>仲森智史</author>

// 多重インクルードの防止
#pragma once

// ヘッダファイルの読み込み
#include "Game/Commons/Collision.h"
#include "Game/Commons/UserResources.h"
#include "Game/GameObjects/Player/Player.h"
#include "Game/GameObjects/Enemy/Enemy.h"
#include "Game/GameObjects/Ball/BallManager.h"
#include "Game/GameObjects/AirTarget/AirTarget.h"
#include "Game/GameObjects/Tutorial/Arrow.h"


// クラスの定義
class IEntity;
class Camera;
class ScoreManager;
class TutorialScene;


// クラスの定義
class Field
{
// 定数
private:
	static constexpr float MODEL_SCALE = 3.0f;
	static constexpr float SKYDOME_SCALE = 800.0f;


// 変数
private:
	// ユーザーリソース
	UserResources* m_pUserResources;

	// カメラ
	Camera* m_pCamera;

	// プレイヤー
	std::unique_ptr<Player> m_player;

	// 敵
	std::unique_ptr<Enemy> m_enemy;

	// ボールマネージャー
	std::unique_ptr<BallManager> m_ballManager;

	// 空中の的
	std::unique_ptr<AirTarget> m_airTarget;

	// モデル
	DirectX::Model* m_model;

	// スカイドーム
	DirectX::Model* m_skydomeModel; 

	// 座標
	DirectX::SimpleMath::Vector3 m_position;  

	// コライダー
	SphereCollider m_collider; 
	ModelCollider m_stageCollider; 

	// 回転
	float m_rotate;


// 関数
public:
	// コンストラクタ
	Field(Camera* pCamera);

	// デストラクタ
	~Field();

	// 初期化
	void Initialize(int stageIndex, bool isSkyDome);
	// チュートリアルの初期化
	void TutorialInitialize(int stageIndex, bool isSkyDome);

	// 更新
	void Update(ScoreManager* pScoreManager, float elapsedTime);
	// タイトルの更新
	void TitleUpdate();
	// チュートリアルの更新
	void TutorialUpdate(TutorialScene* scene, float elapsedTime);

	// 描画
	void Render();
	// タイトルの描画
	void TitleRender();

	// 終了処理
	void Finalize();

	// 上方向の補正
	DirectX::SimpleMath::Vector3 CorrectUp(IEntity* iEntity);
	DirectX::SimpleMath::Vector3 CorrectUp(IEntity* iEntity, DirectX::SimpleMath::Vector3 vector);

	// 実体とフィールドの衝突判定
	void IsHitEntityToField(IEntity* pIEntity);


// 設定/取得
public:
	// 座標
	void SetPosition(DirectX::SimpleMath::Vector3 pos) { m_position = pos; }  // 設定
	const DirectX::SimpleMath::Vector3& GetPosition() { return m_position; }  // 取得

	// コライダーの取得
	SphereCollider& GetCollider();

	// コライダーの取得
	ModelCollider& GetStageCollider() { return m_stageCollider; }

	// 回転の設定
	void SetRotate(float rotate) { m_rotate = rotate; }

	// プレイヤーの取得
	Player* GetPlayer() const { return m_player.get(); }

	// 敵の取得
	Enemy* GetEnemy() const { return m_enemy.get(); }

	// ボールマネージャーの取得
	BallManager* GetBallManager() const { return m_ballManager.get(); }

	// 空中の的の取得
	AirTarget* GetAirTarget() const { return m_airTarget.get(); }

	// カメラの取得
	Camera* GetCamera() const { return m_pCamera; }
};

