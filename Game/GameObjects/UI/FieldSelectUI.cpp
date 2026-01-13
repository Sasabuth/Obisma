/// <summary>
/// FieldSelectUIに関するソースファイル
/// </summary>
/// <author>仲森智史</author>
/// <date></date>

// 多重インクルードの防止
#pragma once

// ヘッダファイルの読み込み
#include "pch.h"
#include "FieldSelectUI.h"

#include "Game/Commons/Resources.h"
#include "Game/Commons/SceneManager.h"
#include "Game/Scenes/GameplayScene.h"
#include "Game/Scenes/TutorialScene.h"


/// <summary>
/// コンストラクタ
/// </summary>
FieldSelectUI::FieldSelectUI()
	: m_pUserResources(nullptr)
	, m_pScene(nullptr)
	, m_fieldIndex(0)
	, m_isOpen(false)
{
}



/// <summary>
/// デストラクタ
/// </summary>
FieldSelectUI::~FieldSelectUI()
{
}



/// <summary>
/// 初期化処理
/// </summary>
void FieldSelectUI::Initialize(Scene* pScene)
{
	// ユーザーリソースの設定
	m_pUserResources = UserResources::GetUserResource();

	// シーンの設定
	m_pScene = pScene;

	// フィールドの番号の初期化
	m_fieldIndex = 0;

	// ボタンの初期化
	m_button[0].SetTexture(Resources::GetInstance()->GetTexture(L"FieldSelect.png"));
	// ゲームプレイシーンに変更
	m_button[0].SetFunc([this]() { m_fieldIndex -= 1; });

	// ボタンの初期化
	m_button[1].SetTexture(Resources::GetInstance()->GetTexture(L"FieldSelect1.png"));
	// チュートリアルシーンに変更
	m_button[1].SetFunc([this]() { m_fieldIndex += 1; });

	// ボタンの初期化
	m_button[2].SetTexture(Resources::GetInstance()->GetTexture(L"BackButton.png"));
	// 閉じる
	m_button[2].SetFunc([this]() { m_isOpen = false; });

	// ボタンの初期化
	m_button[3].SetTexture(Resources::GetInstance()->GetTexture(L"StagePlay.png"));
	// ゲームプレイシーンに変更
	m_button[3].SetFunc([this]() {
		auto transitionMask = m_pUserResources->GetTransitionMask();
		// フェードアウトする
		if (transitionMask->IsOpen())
		{
			transitionMask->Close();
		}

		// Jsonに入力
		nlohmann::json json = Resources::GetInstance()->GetJson(L"FieldSelect.json");
		json["FieldIndex"] = m_fieldIndex;
		Resources::GetInstance()->SetJson(L"FieldSelect.json", json); 
		}
	);

	// メニューの設定
	for (int i = 0; i < FIELDSELECT_COUNT; i++)
	{
		m_button[i].SetPosition(DirectX::SimpleMath::Vector2(FIELDSELECT[i].pos));
		m_button[i].SetSize(DirectX::SimpleMath::Vector2(FIELDSELECT[i].size));
		m_button[i].SetScale(FIELDSELECT[i].scale);
	}

	for (int i = 0; i < MAXSTAGE_COUNT; i++)
	{
		std::wstring filename = L"Stage" + std::to_wstring(i) + L".png";
		m_stageTexture[i].SetTexture(Resources::GetInstance()->GetTexture(filename.c_str()));
	}

	m_backGround.SetTexture(Resources::GetInstance()->GetTexture(L"StagePlayBackGround.png"));

	// 開いていない
	m_isOpen = false;
}



/// <summary>
/// 更新処理
/// </summary>
/// <param name="elapsedTime"></param> 経過時間
void FieldSelectUI::Update(const BoxCollider2D& collider)
{
	// マウストラッカーの取得
	auto mouseTK = m_pUserResources->GetMouseStateTracker();

	// コライダーに当たっていたらクリック関数を呼ぶ
	for (int i = 0; i < FIELDSELECT_COUNT; i++)
	{
		if (IsHit(collider, m_button[i].GetCollider()) && mouseTK->leftButton == mouseTK->PRESSED)
		{
			m_button[i].Click();
		}
	}

	if (m_fieldIndex < 0)
	{
		m_fieldIndex = MAXSTAGE_COUNT - 1;
	}

	if (m_fieldIndex > MAXSTAGE_COUNT - 1)
	{
		m_fieldIndex = 0;
	}


	// SEの音量の設定
	if (m_se) m_se->SetVolume(Resources::GetInstance()->GetSEVolume());
}



/// <summary>
/// 描画処理
/// </summary>
void FieldSelectUI::Draw(const BoxCollider2D& collider)
{
	m_backGround.Draw(DirectX::SimpleMath::Vector2(FIELDSELECT[3].pos.x, FIELDSELECT[3].pos.y - 10), FIELDSELECT[3].size, FIELDSELECT[3].scale);

	// メニューの数回す
	for (int i = 0; i < FIELDSELECT_COUNT; i++)
	{
		// マウスのコライダーと当たったら色を変更
		if (IsHit(collider, m_button[i].GetCollider()))
		{
			m_button[i].Draw(FIELDSELECT_COLORS[i]);
		}
		// 白の文字を出す
		else
		{
			m_button[i].Draw(DirectX::Colors::White);
		}

	}

	m_stageTexture[m_fieldIndex].Draw(STAGE.pos, STAGE.size, STAGE.scale);

	/*auto debugFont = m_pUserResources->GetDebugFont();
	debugFont->Render(L"FieldIndex", m_fieldIndex);*/
}



/// <summary>
/// 終了処理
/// </summary>
void FieldSelectUI::Finalize()
{
}
