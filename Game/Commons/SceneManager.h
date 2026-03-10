//--------------------------------------------------------------------------------------
// File: SceneManager.h
//
// シーンを管理するクラス
//
// Date: 2025.2.28
//--------------------------------------------------------------------------------------
#pragma once

// ESCキーで終了したい場合有効にしてください
#define ESC_QUIT_ENABLE

#ifdef ESC_QUIT_ENABLE
#include "Keyboard.h"
#endif


class SceneManager;

// シーンの基底クラス
class Scene
{
private:

	// シーンマネージャーへのポインタ
	SceneManager* m_sceneManager;


public:

	// コンストラクタ
	Scene() : m_sceneManager(nullptr) {}

	// デストラクタ
	virtual ~Scene() = default;

	// 初期化
	virtual void Initialize() = 0;

	// 更新
	virtual void Update(float elapsedTime) = 0;

	// 描画
	virtual void Render() = 0;

	// 終了処理
	virtual void Finalize() = 0;

	// デバイスに依存するリソースを作成する関数
	virtual void CreateDeviceDependentResources() {}

	// ウインドウサイズに依存するリソースを作成する関数
	virtual void CreateWindowSizeDependentResources() {}

	// デバイスロストした時に呼び出される関数
	virtual void OnDeviceLost() {}

public:

	// シーンマネージャー設定関数
	void SetSceneManager(SceneManager* sceneManager) { m_sceneManager = sceneManager; }
	SceneManager* GetSceneManager() const { return m_sceneManager; }

	// シーンの切り替え関数
	template <class T>
	void ChangeScene();

};

// シーンマネージャークラス
class SceneManager
{
private:
	// 実行中のシーンへのポインタ
	Scene* m_scene;

	// 次のシーンへのポインタ
	Scene* m_nextScene;

	// プレイ人数
	int m_playerCount;

	// 引き分けか
	bool m_isDraw;

	// 勝者
	std::vector<int> m_rank;

	// シーン削除関数
	void DeleteScene();

public:

	// コンストラクタ
	SceneManager()
		: m_scene(nullptr)
		, m_nextScene(nullptr)
		, m_playerCount(0)
		, m_isDraw(false)
	{
	};

	// デストラクタ
	virtual ~SceneManager() { DeleteScene(); };

	// 更新
	void Update(float elapsedTime);

	// 描画
	void Render();

	// デバイスに依存するリソースを作成する関数
	void CreateDeviceDependentResources();
	
	// ウインドウサイズに依存するリソースを作成する関数
	void CreateWindowSizeDependentResources();

	// デバイスロストした時に呼び出される関数
	virtual void OnDeviceLost();

	// シーンの設定関数
	template <class T>
	void SetScene();

	// 次のシーンの設定関数
	template <class T>
	void SetNextScene();

	// プレイヤー数
	void SetPlayerCount(int playerCount) { m_playerCount = playerCount; }  // 設定
	int GetPlayerCount() const { return m_playerCount; }                   // 取得

	// 順位
	void SetRank(int index,int charactorNum) { m_rank.resize(m_playerCount); m_rank[index] = charactorNum; }  // 設定
	int GetRank(int index) const { return m_rank[index]; }                                                    // 取得

	// 引き分け
	void SetIsDraw(bool isDraw) { m_isDraw = isDraw; }  // 設定
	bool GetIsDraw() const { return m_isDraw; }			// 取得

};


// シーンの切り替え関数
template <class T>
void Scene::ChangeScene()
{
	m_sceneManager->SetNextScene<T>();
}

// シーンの設定関数
template <class T>
void SceneManager::SetScene()
{
	// シーンを削除
	DeleteScene();

	assert(m_scene == nullptr);

	// シーンを生成
	m_scene = new T;

	// シーンにシーンマネージャーへのポインタを設定
	m_scene->SetSceneManager(this);

	// シーンの初期化処理
	m_scene->Initialize();
}

// シーンの設定関数
template <class T>
void SceneManager::SetNextScene()
{
	if (!m_nextScene)
	{
		// シーンを生成
		m_nextScene = new T;
	}
}



