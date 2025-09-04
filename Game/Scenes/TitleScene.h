/// <summary>
/// Titleシーンに関するヘッダファイル
/// </summary>
/// <author>仲森智史</author>
/// <date></date>


// 多重インクルードの防止
#pragma once


// ヘッダファイルの読み込み
#include "Game/Commons/SceneManager.h"
#include "Game/Commons/UserResources.h"
#include "Game/Commons/Sprite.h"


// クラスの定義
class TitleScene : public Scene
{
private:
	UserResources* m_pUserResources;

	Sprite m_texture;


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

