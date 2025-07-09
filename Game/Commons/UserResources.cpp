#include "pch.h"
#include "Game/Commons/UserResources.h"

using namespace DirectX;

// ユーザーリソースの初期化
std::unique_ptr<UserResources> UserResources::s_userResource = nullptr;


/// <summary>
/// ユーザーリソースの取得
/// </summary>
/// <returns>ユーザーリソース</returns>
UserResources* const UserResources::GetUserResource()
{
	if (s_userResource == nullptr)
	{
		// グラフィックスのインスタンスを生成する
		s_userResource.reset(new UserResources());
	}
	// グラフィックスのインスタンスを返す
	return s_userResource.get();
}



/// <summary>
/// コンストラクタ
/// </summary>
UserResources::UserResources()
	: m_timer(nullptr)
	, m_deviceResources(nullptr)
	, m_keyboardTracker(nullptr)
	, m_mouseTracker(nullptr)
	, m_debugFont(nullptr)
	, m_proj(nullptr)
	, m_states(nullptr)
	, m_effectFactory(nullptr)
{
}
