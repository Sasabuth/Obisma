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


// クラスの定義
class ResultScene : public Scene
{
private:
	// ユーザーリソース
	UserResources* m_pUserResources;

	// フェイステクスチャ
	std::vector<std::unique_ptr<Sprite>> m_faceTextures;

	// 勝利テクスチャ
	std::vector<std::unique_ptr<Sprite>> m_winTextures;

	// スペーステクスチャ
	Sprite m_spaceTexture;

	// 戻るテクスチャ
	Sprite m_backTexture;

	// 座標
	DirectX::SimpleMath::Vector2 m_position;
	DirectX::SimpleMath::Vector2 m_position2;

	// BGM
	std::unique_ptr<DirectX::SoundEffectInstance> m_bgm;

	// 速度
	float m_speed;


// 関数
public:
	// コンストラクタ
	ResultScene();

	// デストラクタ
	~ResultScene() override;;

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

