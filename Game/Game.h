//
// Game.h
//

#pragma once

#include "Common/DeviceResources.h"
#include "Common/StepTimer.h"

#include "Game/Commons/DebugFont.h"
#include "Game/Commons/SceneManager.h"
#include "Game/Commons/UserResources.h"
#include "Game/Commons/TransitionMask.h"
#include "Game/Commons/Sprite.h"

#include "Common/RenderTexture.h"

// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game final : public DX::IDeviceNotify
{
// 定数
private:
    static constexpr Sprite::Format MOUSECURSOR =
    {
        DirectX::SimpleMath::Vector2(-1, -1),          // 座標(使わない)
        DirectX::SimpleMath::Vector2(350, 344),        // サイズ
        0.045f                                          // 拡大率
    };

// 変数
private:
    // Device resources.
    std::unique_ptr<DX::DeviceResources>    m_deviceResources;

    // Rendering loop timer.
    DX::StepTimer                           m_timer;

    // キーボードステートトラッカー
    DirectX::Keyboard::KeyboardStateTracker m_keyboardTracker;

    // マウスステートトラッカー
    DirectX::Mouse::ButtonStateTracker m_mouseTracker;

    // 射影行列
    DirectX::SimpleMath::Matrix m_proj;

    // 共通ステート
    std::unique_ptr<DirectX::CommonStates> m_states;

    // デバック文字列
    std::unique_ptr<DebugFont> m_debugFont;

    // トランジションマスク
    std::unique_ptr<TransitionMask> m_transitionMask;

    // トランジションテクスチャ
    std::unique_ptr<DX::RenderTexture> m_transitionTexture;

    // エフェクトファクトリー
    std::unique_ptr<DirectX::EffectFactory> m_effectFactory;

    // シーンマネージャーのポインタ
    std::unique_ptr<SceneManager> m_sceneManager;

    // ユーザーリソース
    UserResources* m_userResources;

    // マウスカーソル
    std::unique_ptr<Sprite> m_mouseCursor;

    // フルスクリーンを管理
    BOOL m_fullscreen;


// 関数
public:
    Game() noexcept(false);
    ~Game() = default;

    Game(Game&&) = default;
    Game& operator= (Game&&) = default;

    Game(Game const&) = delete;
    Game& operator= (Game const&) = delete;

    // Initialization and management
    void Initialize(HWND window, int width, int height);

    // Basic game loop
    void Tick();

    // IDeviceNotify
    void OnDeviceLost() override;
    void OnDeviceRestored() override;

    // Messages
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnWindowMoved();
    void OnDisplayChange();
    void OnWindowSizeChanged(int width, int height);

    // Properties
    void GetDefaultSize( int& width, int& height ) const noexcept;

   
// 設定/取得
public:
    // 画面モードを設定する関数(TURE フルスクリーン)
    void SetFullscreenState(BOOL value)
    {
        m_fullscreen = value;
        m_deviceResources->GetSwapChain()->SetFullscreenState(m_fullscreen, nullptr);
        if (value) m_deviceResources->CreateWindowSizeDependentResources();
    }


// 内部関数
private:

    void Update(DX::StepTimer const& timer);
    void Render();

    void Clear();

    void CreateDeviceDependentResources();
    void CreateWindowSizeDependentResources();
};
