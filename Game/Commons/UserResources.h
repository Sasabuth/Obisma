/// <summary>
/// UserResourcesに関するヘッダファイル
/// </summary>
/// <author>仲森智史</author>
#pragma once

#include "Common/StepTimer.h"
#include "Common/DeviceResources.h"
#include "Game/Commons/DebugFont.h"
#include "Game/Commons/TransitionMask.h"

// 各シーンに渡す共通リソースを記述してください
class UserResources
{
private:
	// Graphicsクラスのインスタンスへのポインタ
	static std::unique_ptr<UserResources> s_userResource;

	// ステップタイマー
	DX::StepTimer* m_timer;

	// デバイスリソース
	DX::DeviceResources* m_deviceResources;

	// キーボードステートトラッカー
	DirectX::Keyboard::KeyboardStateTracker* m_keyboardTracker;

	// マウスステートトラッカー
	DirectX::Mouse::ButtonStateTracker* m_mouseTracker;

	// デバッグ用文字列表示へのポインタ
	DebugFont* m_debugFont;

	// 射影行列
	DirectX::SimpleMath::Matrix* m_proj;

	// ビュー行列
	DirectX::SimpleMath::Matrix* m_view;

	// 共通ステート
	DirectX::CommonStates* m_states;

	// エフェクトファクトリー
	DirectX::EffectFactory* m_effectFactory;

	// トランジションマスクへのポインタ
	TransitionMask* m_transitionMask;

private: 
	// コンストラクタ
	UserResources();

public:

	// ユーザーリソースの取得
	static UserResources* const GetUserResource();
	


	// ------------------------------------------ //
	// ステップタイマー	
	// ------------------------------------------ //

	// ステップタイマーを設定する関数
	void SetStepTimerStates(DX::StepTimer* timer) { m_timer = timer; }

	// ステップタイマーを取得する関数
	DX::StepTimer* GetStepTimer() { return m_timer; }

	// ------------------------------------------ //
	// デバイスリソース	
	// ------------------------------------------ //

	// デバイスリソースを設定する関数
	void SetDeviceResources(DX::DeviceResources* deviceResources) { m_deviceResources = deviceResources; }

	// デバイスリソースを取得する関数
	DX::DeviceResources* GetDeviceResources() { return m_deviceResources; }

	// ------------------------------------------ //
	// キーボードステートトラッカー	
	// ------------------------------------------ //

	// キーボードステートトラッカーを設定する関数
	void SetKeyboardStateTracker(DirectX::Keyboard::KeyboardStateTracker* tracker) { m_keyboardTracker = tracker; }

	// キーボードステートトラッカーを取得する関数
	DirectX::Keyboard::KeyboardStateTracker* GetKeyboardStateTracker() { return m_keyboardTracker; }

	// ------------------------------------------ //
	// マウスステートトラッカー	
	// ------------------------------------------ //

	// マウスステートトラッカーを設定する関数
	void SetMouseStateTracker(DirectX::Mouse::ButtonStateTracker* tracker) { m_mouseTracker = tracker; }

	// マウスステートトラッカーを取得する関数
	DirectX::Mouse::ButtonStateTracker* GetMouseStateTracker() { return m_mouseTracker; }

	// ------------------------------------------ //
	// 射影行列	
	// ------------------------------------------ //

	// 射影行列を設定する関数
	void SetProject(DirectX::SimpleMath::Matrix* project) { m_proj = project; }

	// 射影行列を取得する関数
	DirectX::SimpleMath::Matrix* GetProject() { return m_proj; }

	// ------------------------------------------ //
	// ビュー行列	
	// ------------------------------------------ //

	// ビュー行列を設定する関数
	void SetView(DirectX::SimpleMath::Matrix* view) { m_view = view; }

	// ビュー行列を取得する関数
	DirectX::SimpleMath::Matrix* GetView() { return m_view; }

	// ------------------------------------------ //
	// 共通ステート	
	// ------------------------------------------ //

	// 共通ステートを設定する関数
	void SetCommonStates(DirectX::CommonStates* states) { m_states = states; }

	// 共通ステートを取得する関数
	DirectX::CommonStates* GetCommonStates() { return m_states; }

	// ------------------------------------------ //
	// デバッグフォント	
	// ------------------------------------------ //

	// デバッグフォントを設定する関数
	void SetDebugFont(DebugFont* debugFont) { m_debugFont = debugFont; }

	// デバッグフォントを取得する関数
	DebugFont* GetDebugFont() { return m_debugFont; }

	// ------------------------------------------ //
	// エフェクトファクトリー	
	// ------------------------------------------ //

	// エフェクトファクトリーの設定する関数
	void SetEffectFactory(DirectX::EffectFactory* effectFactory) { m_effectFactory = effectFactory; }

	// エフェクトファクトリーを取得する関数
	DirectX::EffectFactory* GetEffectFactory() const { return m_effectFactory; }

	// ------------------------------------------ //
	// トランジションマスク
	// ------------------------------------------ //

	// トランジションマスクを設定する関数
	void SetTransitionMask(TransitionMask* transitionMask) { m_transitionMask = transitionMask; }

	// トランジションマスクを取得する関数
	TransitionMask* GetTransitionMask() { return m_transitionMask; }
};

