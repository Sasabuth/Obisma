/// <summary>
/// Gameplayシーンに関するヘッダファイル
/// </summary>
/// <author>仲森智史</author>
/// <date></date>


// 多重インクルードの防止
#pragma once


// ヘッダファイルの読み込み
#include "Game/Commons/SceneManager.h"
#include "Game/Commons/UserResources.h"
#include "Game/GameObjects/Field/Field.h"
#include "Game/GameObjects/Camera/Camera.h"
#include "Game/GameObjects/Player/Player.h"
#include "Game/GameObjects/Ball/Ball.h"


// クラスの定義
class GameplayScene : public Scene
{
// 変数
private:
	UserResources* m_userResources;

	std::unique_ptr<Field> m_field;

	//  デバッグカメラ
	std::unique_ptr<Camera> m_camera;

	//  プレイヤー
	std::unique_ptr<Player> m_player;
	
	//  ボール
	std::unique_ptr<Ball> m_ball;


// 関数
public:
	// コンストラクタ
	GameplayScene();

	// デストラクタ
	~GameplayScene() override;;

	// 初期化
	void Initialize() override;

	// 更新
	void Update(float elapsedTime) override;

	// 描画
	void Render() override;

	// 終了処理
	void Finalize() override;

	// デバイスに依存するリソースを作成する関数
	void CreateDeviceDependentResources() override;

	// ウインドウサイズに依存するリソースを作成する関数
	void CreateWindowSizeDependentResources() override;

	// デバイスロストした時に呼び出される関数
	void OnDeviceLost() override;


public:
	Field& GetField() const { return *m_field; }
	Ball& GetBall() const { return *m_ball; }

private:
	void IsHitEntityToField(IEntity* pIEntity, Field* pField);

};

