#include "pch.h"
#include "Game/Commons/SceneManager.h"

#include "Game/Commons/Messenger.h"



// 更新関数
void SceneManager::Update(float elapsedTime)
{
#ifdef ESC_QUIT_ENABLE
	//// ESCキーで終了
	//auto kb = DirectX::Keyboard::Get().GetState();
	//if (kb.Escape) PostQuitMessage(0);
#endif

	// シーンの切り替え処理
	if (m_nextScene)
	{
		DeleteScene();

		assert(m_scene == nullptr);

		// シーンを切り替え
		m_scene = m_nextScene;

		m_nextScene = nullptr;

		// シーンにシーンマネージャーへのポインタを設定
		m_scene->SetSceneManager(this);

		// シーンの初期化処理
		m_scene->Initialize();
	}

	// シーンの更新
	if (m_scene) m_scene->Update(elapsedTime);

}

// 描画関数

void SceneManager::Render()
{
	// シーンの描画
	if (m_scene) m_scene->Render();
}

// デバイスに依存するリソースを作成する関数

void SceneManager::CreateDeviceDependentResources()
{
	if (m_scene) m_scene->CreateDeviceDependentResources();
}

// ウインドウサイズに依存するリソースを作成する関数

void SceneManager::CreateWindowSizeDependentResources()
{
	if (m_scene) m_scene->CreateWindowSizeDependentResources();
}

// デバイスロストした時に呼び出される関数

void SceneManager::OnDeviceLost()
{
	if (m_scene) m_scene->OnDeviceLost();
}

// シーンの削除関数
void SceneManager::DeleteScene()
{
	if (m_scene)
	{
		m_scene->Finalize();

		// メッセンジャーを破棄する
		Messenger::DestroyInstance();

		delete m_scene;

		m_scene = nullptr;
	}
}
