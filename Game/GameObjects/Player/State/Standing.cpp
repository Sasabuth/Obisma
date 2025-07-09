/// <summary>
/// Standingに関するソースファイル
/// </summary>
/// <author>仲森智史</author>
/// <date>2025/05/21</date>

// ヘッダファイルの読み込み
#include "pch.h"
#include "Standing.h"

#include "Game/Scenes/GameplayScene.h"
#include "Game/GameObjects//Camera/Camera.h"
#include "Game/GameObjects/Field/Field.h"
#include "DebugDraw.h"


// 名前の省略
using namespace DirectX;


/// <summary>
/// コンストラクタ
/// </summary>
Standing::Standing(Player* player)
	: m_player(player)
	, m_userResources(nullptr)
{
}



/// <summary>
/// デストラクタ
/// </summary>
Standing::~Standing()
{
}


/// <summary>
/// 初期化処理
/// </summary>
void Standing::Initialize()
{
	m_userResources = UserResources::GetUserResource();

	auto device = m_userResources->GetDeviceResources()->GetD3DDevice();
	auto context = m_userResources->GetDeviceResources()->GetD3DDeviceContext();

	auto effectFactory = m_userResources->GetEffectFactory();
	effectFactory->SetDirectory(L"Resources/Models/");
}



/// <summary>
/// 更新処理
/// </summary>
/// <param name="elapsedTime">経過時間</param> 
void Standing::Update(float elapsedTime)
{
	UNREFERENCED_PARAMETER(elapsedTime);

	//m_model->UpdateEffects(
	//	// 引数にラムダ式として処理内容を指定する
	//	[&](IEffect* pEffect)
	//	{
	//		// BasicEffectにキャストする
	//		DirectX::BasicEffect* pBasicEffect = dynamic_cast<DirectX::BasicEffect*>(pEffect);

	//		//// ライトをオフにする
	//		pBasicEffect->SetLightEnabled(0, false);
	//		pBasicEffect->SetLightEnabled(1, false);
	//		pBasicEffect->SetLightEnabled(2, false);

	//		// 自己発光(引数はカラー)
	//		pBasicEffect->SetEmissiveColor(DirectX::SimpleMath::Vector3(1, 1, 1));
	//	}
	//);

	auto kb = Keyboard::Get().GetState();
	auto kbTracker = m_userResources->GetKeyboardStateTracker();
	auto mouse = Mouse::Get().GetState();
	auto mouseTk = m_userResources->GetMouseStateTracker();

	// プロジェクション行列
	auto proj = m_userResources->GetProject();}



/// <summary>
/// 描画処理
/// </summary>
void Standing::Render()
{
	auto context = m_userResources->GetDeviceResources()->GetD3DDeviceContext();
	auto states = m_userResources->GetCommonStates();

	// デバックフォントの描画
	auto* debugFont = m_userResources->GetDebugFont();

}



/// <summary>
/// 終了処理
/// </summary>
void Standing::Finalize()
{
}