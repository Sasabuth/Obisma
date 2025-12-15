/// <summary>
/// MenuUIに関するソースファイル
/// </summary>
/// <author>仲森智史</author>
/// <date></date>

// 多重インクルードの防止
#pragma once

// ヘッダファイルの読み込み
#include "pch.h"
#include "MenuUI.h"

#include "Game/Commons/Resources.h"
#include "Game/Commons/SceneManager.h"
#include "Game/Scenes/GameplayScene.h"
#include "Game/Scenes/TutorialScene.h"


/// <summary>
/// コンストラクタ
/// </summary>
MenuUI::MenuUI()
	: m_pUserResources(nullptr)
	, m_pScene(nullptr)
	, m_isOpen(false)
{
}



/// <summary>
/// デストラクタ
/// </summary>
MenuUI::~MenuUI()
{
}



/// <summary>
/// 初期化処理
/// </summary>
void MenuUI::Initialize(Scene* pScene)
{
	// ユーザーリソースの設定
	m_pUserResources = UserResources::GetUserResource();

	// シーンの設定
	m_pScene = pScene;

	// ボタンの初期化
	m_button[0].SetTexture(Resources::GetInstance()->GetTexture(L"RealPerformance.png"));
	// ゲームプレイシーンに変更
	m_button[0].SetFunc([this]() { m_pScene->ChangeScene<GameplayScene>(); });

	// ボタンの初期化
	m_button[1].SetTexture(Resources::GetInstance()->GetTexture(L"Practice.png"));
	// チュートリアルシーンに変更
	m_button[1].SetFunc([this]() { m_pScene->ChangeScene<TutorialScene>(); });

	// ボタンの初期化
	m_button[2].SetTexture(Resources::GetInstance()->GetTexture(L"BackButton.png"));
	// 閉じる
	m_button[2].SetFunc([this]() { m_isOpen = false; });

	// メニューの設定
	for (int i = 0; i < MENU_COUNT; i++)
	{
		m_button[i].SetPosition(DirectX::SimpleMath::Vector2(MENU_POSITIONS[i]));
		m_button[i].SetSize(DirectX::SimpleMath::Vector2(MENU_SIZE));
		m_button[i].SetScale(MENU_SCALES[i]);
	}

	// 開いていない
	m_isOpen = false;
}



/// <summary>
/// 更新処理
/// </summary>
/// <param name="elapsedTime"></param> 経過時間
void MenuUI::Update(const BoxCollider2D& collider)
{
	// マウストラッカーの取得
	auto mouseTK = m_pUserResources->GetMouseStateTracker();

	// コライダーに当たっていたらクリック関数を呼ぶ
	for (int i = 0; i < MENU_COUNT; i++)
	{
		if (IsHit(collider, m_button[i].GetCollider()) && mouseTK->leftButton == mouseTK->PRESSED)
		{
			m_button[i].Click();
		}
	}

	// SEの音量の設定
	if(m_se) m_se->SetVolume(Resources::GetInstance()->GetSEVolume());
}



/// <summary>
/// 描画処理
/// </summary>
void MenuUI::Draw(const BoxCollider2D& collider)
{
	// メニューの数回す
	for (int i = 0; i < MENU_COUNT; i++)
	{
		// マウスのコライダーと当たったら色を変更
		if (IsHit(collider, m_button[i].GetCollider()))
		{
			m_button[i].Draw(MENU_COLORS[i]);
		}
		// 白の文字を出す
		else
		{
			m_button[i].Draw(DirectX::Colors::White);
		}
		
	}
}



/// <summary>
/// 終了処理
/// </summary>
void MenuUI::Finalize()
{
}
