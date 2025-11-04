/// <summary>
/// AudioUIに関するソースファイル
/// </summary>
/// <author>仲森智史</author>
/// <date></date>

// 多重インクルードの防止
#pragma once

// ヘッダファイルの読み込み
#include "pch.h"
#include "AudioUI.h"

#include "Game/Commons/Resources.h"


/// <summary>
/// コンストラクタ
/// </summary>
AudioUI::AudioUI()
	: m_pUserResources(nullptr)
	, m_isHit{}
	, m_isOpen(false)
{
}



/// <summary>
/// デストラクタ
/// </summary>
AudioUI::~AudioUI()
{
}


/// <summary>
/// 初期化処理
/// </summary>
void AudioUI::Initialize()
{
	// ユーザーリソースの取得
	m_pUserResources = UserResources::GetUserResource();

	// テクスチャの設定
	m_audioUI.SetTexture(Resources::GetInstance()->GetTexture(L"AudioUI.png"));

	// コライダーの設定
	for (int i = 0; i < BAR_COUNT; i++)
	{
		m_barCollider[i].SetPosition(DirectX::SimpleMath::Vector2(
			Resources::GetInstance()->GetJson(L"AudioUI.json")["Position"][std::to_string(i)]["x"],
			Resources::GetInstance()->GetJson(L"AudioUI.json")["Position"][std::to_string(i)]["y"])
		);

		m_barCollider[i].SetSize(BAR_SIZE);
		m_isHit[i] = false;
	}

	// UI用コライダーの設定
	m_uiCollider.SetPosition(DirectX::SimpleMath::Vector2(640, 360));
	m_uiCollider.SetSize(DirectX::SimpleMath::Vector2(1743, 850));
	m_uiCollider.SetScale(0.4f);

	// ボタンの初期化
	m_button.SetTexture(Resources::GetInstance()->GetTexture(L"BackButton.png"));
	m_button.SetPosition(DirectX::SimpleMath::Vector2(100.0f, 70.0f));
	m_button.SetSize(DirectX::SimpleMath::Vector2(475.0f, 260.0f));
	m_button.SetScale(0.3f);

	m_isOpen = false;

	// ボリューム設定
	Resources::GetInstance()->SetBGMVolume((m_barCollider[0].GetPosition().x - BAR_MINPOS) / (BAR_MAXPOS - BAR_MINPOS));
	Resources::GetInstance()->SetSEVolume((m_barCollider[1].GetPosition().x - BAR_MINPOS) / (BAR_MAXPOS - BAR_MINPOS));
}



/// <summary>
/// 更新処理
/// </summary>
/// <param name="elapsedTime"></param> 経過時間
void AudioUI::Update(const BoxCollider2D& collider)
{
	// マウストラッカーの取得
	auto mouseTK = m_pUserResources->GetMouseStateTracker();

	for (int i = 0; i < BAR_COUNT; i++)
	{
		// コライダーの上でクリックしたら当たっている
		if (IsHit(m_barCollider[i], collider) && mouseTK->leftButton== mouseTK->PRESSED)
		{
			m_isHit[i] = true;
		}

		// 当たっていたら動かす
		if (m_isHit[i])
		{
			m_barCollider[i].SetPosition(DirectX::SimpleMath::Vector2(collider.GetPosition().x, m_barCollider[i].GetPosition().y));

			// 離したら当たっていない
			if (mouseTK->leftButton == mouseTK->RELEASED)
			{
				m_isHit[i] = false;
			}
		}

		// 座標の上限
		if (m_barCollider[i].GetPosition().x <= BAR_MINPOS)
		{
			m_barCollider[i].SetPosition(DirectX::SimpleMath::Vector2(BAR_MINPOS, m_barCollider[i].GetPosition().y));
		}
		if (m_barCollider[i].GetPosition().x >= BAR_MAXPOS)
		{
			m_barCollider[i].SetPosition(DirectX::SimpleMath::Vector2(BAR_MAXPOS, m_barCollider[i].GetPosition().y));
		}

		// Jsonに入力
		nlohmann::json json = Resources::GetInstance()->GetJson(L"AudioUI.json");
		json["Position"][std::to_string(i)]["x"] = m_barCollider[i].GetPosition().x;
		json["Position"][std::to_string(i)]["y"] = m_barCollider[i].GetPosition().y;
		Resources::GetInstance()->SetJson(L"AudioUI.json", json);
	}

	// コライダーが当たってないときか戻るボタンに左クリックしたら閉じる
	if (!IsHit(collider, m_uiCollider) || IsHit(collider, m_button.GetCollider()))
	{
		if (mouseTK->leftButton == mouseTK->PRESSED)
		{
			m_se = Resources::GetInstance()->GetSESound(L"ButtonClick.wav", Resources::GetInstance()->GetListener().Position, false);
			m_isOpen = false;
		}	
	}

	// ボリューム設定
	Resources::GetInstance()->SetBGMVolume((m_barCollider[0].GetPosition().x - BAR_MINPOS) / (BAR_MAXPOS - BAR_MINPOS));
	Resources::GetInstance()->SetSEVolume((m_barCollider[1].GetPosition().x - BAR_MINPOS) / (BAR_MAXPOS - BAR_MINPOS));
}



/// <summary>
/// 描画処理
/// </summary>
void AudioUI::Draw(const BoxCollider2D& collider)
{
	m_audioUI.Draw(DirectX::SimpleMath::Vector2(640, 360), DirectX::SimpleMath::Vector2(1743, 850), 0.4f);

	for (int i = 0; i < BAR_COUNT; i++)
	{
		m_barCollider[i].Draw(DirectX::Colors::Gray);
	}

	if (IsHit(collider, m_button.GetCollider()))
	{
		m_button.Draw(DirectX::Colors::LightSkyBlue);
	}
	else
	{
		m_button.Draw(DirectX::Colors::White);
	}
}



/// <summary>
/// 終了処理
/// </summary>
void AudioUI::Finalize()
{
}
