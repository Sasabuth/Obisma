/// <summary>
/// Titleシーンに関するヘッダファイル
/// </summary>
/// <author>仲森智史</author>


// 多重インクルードの防止
#pragma once


// ヘッダファイルの読み込み
#include "Game/Commons/SceneManager.h"
#include "Game/Commons/UserResources.h"
#include "Game/Commons/Sprite.h"
#include "Game/GameObjects/Camera/Camera.h"
#include "Game/GameObjects/Field/Field.h"


// クラスの定義
class TitleScene : public Scene
{
private:
	UserResources* m_pUserResources;

	Sprite m_titleTexture;
	Sprite m_startTexture;

	// カメラ
	std::unique_ptr<Camera> m_camera;

	// フィールド
	std::unique_ptr<Field> m_field;

	// 速度
	float m_speed;

	// BGM
	std::unique_ptr<DirectX::SoundEffectInstance> m_bgm;



// 関数
public:
	// コンストラクタ
	TitleScene();

	// デストラクタ
	~TitleScene() override;;

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

};

