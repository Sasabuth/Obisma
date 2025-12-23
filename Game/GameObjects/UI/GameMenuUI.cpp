/// <summary>
/// GameMenuUIに関するソースファイル
/// </summary>
/// <author>仲森智史</author>
/// <date></date>

// 多重インクルードの防止
#pragma once

// ヘッダファイルの読み込み
#include "pch.h"
#include "GameMenuUI.h"

#include "Game/Commons/Resources.h"
#include "Game/Commons/SceneManager.h"
#include "Game/Scenes/GameplayScene.h"
#include "Game/Scenes/TutorialScene.h"
#include "Game/GameObjects/UI/AudioUI.h"


/// <summary>
/// コンストラクタ
/// </summary>
GameMenuUI::GameMenuUI()
	: m_pUserResources(nullptr)
	, m_isOpen(false)
{
}



/// <summary>
/// デストラクタ
/// </summary>
GameMenuUI::~GameMenuUI()
{
}



/// <summary>
/// 初期化処理
/// </summary>
void GameMenuUI::Initialize(AudioUI* audioUI)
{
	// ユーザーリソースの設定
	m_pUserResources = UserResources::GetUserResource();

	// ボタンの初期化
	m_button[0].SetTexture(Resources::GetInstance()->GetTexture(L"GameBack.png"));
	// ゲームプレイシーンに変更
	m_button[0].SetFunc([=]() { m_isOpen = false; });

	// ボタンの設定(UIを開く)
	m_button[1].SetTexture(Resources::GetInstance()->GetTexture(L"Audio.png"));
	m_button[1].SetFunc([=]() { audioUI->Click(); });

	auto transitionMask = m_pUserResources->GetTransitionMask();
	// ボタンの設定(終了)
	m_button[2].SetTexture(Resources::GetInstance()->GetTexture(L"TitleBack.png"));
	m_button[2].SetFunc([=]()
		{
			// フェードアウトする
			if (transitionMask->IsOpen())
			{
				transitionMask->Close();
			}
		}
	);

	// メニューの設定
	for (int i = 0; i < GAMEMENU_COUNT; i++)
	{
		m_button[i].SetPosition(DirectX::SimpleMath::Vector2(GAMEMENU[i].pos));
		m_button[i].SetSize(DirectX::SimpleMath::Vector2(GAMEMENU[i].size));
		m_button[i].SetScale(GAMEMENU[i].scale);
	}

	// 隠すテクスチャの設定
	m_hideTexture.SetTexture(Resources::GetInstance()->GetTexture(L"Hide.png"));

	// 開いていない
	m_isOpen = false;
}



/// <summary>
/// 更新処理
/// </summary>
/// <param name="elapsedTime"></param> 経過時間
void GameMenuUI::Update(const BoxCollider2D& collider)
{
	// マウストラッカーの取得
	auto mouseTK = m_pUserResources->GetMouseStateTracker();

	// コライダーに当たっていたらクリック関数を呼ぶ
	for (int i = 0; i < GAMEMENU_COUNT; i++)
	{
		if (IsHit(collider, m_button[i].GetCollider()) && mouseTK->leftButton == mouseTK->PRESSED)
		{
			m_button[i].Click();
		}
	}

	// SEの音量の設定
	if (m_se) m_se->SetVolume(Resources::GetInstance()->GetSEVolume());
}



/// <summary>
/// 描画処理
/// </summary>
void GameMenuUI::Draw(const BoxCollider2D& collider)
{
	// 隠すテクスチャの描画
	m_hideTexture.Draw(HIDE.pos, HIDE.size, HIDE.scale);

	// メニューの数回す
	for (int i = 0; i < GAMEMENU_COUNT; i++)
	{
		// マウスのコライダーと当たったら色を変更
		if (IsHit(collider, m_button[i].GetCollider()))
		{
			m_button[i].Draw(MENU_COLORS);
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
void GameMenuUI::Finalize()
{
}
