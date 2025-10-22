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
	: m_userResources(nullptr)
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
	m_userResources = UserResources::GetUserResource();

	m_audioUI.SetTexture(Resources::GetInstance()->GetTexture(L"AudioUI.png"));

	for (int i = 0; i < BAR_COUNT; i++)
	{
		m_barCollider[i].SetPosition(BAR_POSITIONS[i]);
		m_barCollider[i].SetSize(BAR_SIZE);
		m_isHit[i] = false;
	}

	m_uiCollider.SetPosition(DirectX::SimpleMath::Vector2(640, 360));
	m_uiCollider.SetSize(DirectX::SimpleMath::Vector2(1743, 850));
	m_uiCollider.SetScale(0.4f);

	m_isOpen = false;

	Resources::GetInstance()->SetBGMVolume((m_barCollider[0].GetPosition().x - BAR_MINPOS) / (BAR_MAXPOS - BAR_MINPOS));
	Resources::GetInstance()->SetSEVolume((m_barCollider[1].GetPosition().x - BAR_MINPOS) / (BAR_MAXPOS - BAR_MINPOS));
}



/// <summary>
/// 更新処理
/// </summary>
/// <param name="elapsedTime"></param> 経過時間
void AudioUI::Update(const BoxCollider2D& collider)
{
	auto mouseTK = m_userResources->GetMouseStateTracker();

	for (int i = 0; i < BAR_COUNT; i++)
	{
		if (IsHit(m_barCollider[i], collider) && mouseTK->leftButton== mouseTK->PRESSED)
		{
			m_isHit[i] = true;
		}

		if (m_isHit[i])
		{
			m_barCollider[i].SetPosition(DirectX::SimpleMath::Vector2(collider.GetPosition().x, m_barCollider[i].GetPosition().y));

			if (mouseTK->leftButton == mouseTK->RELEASED)
			{
				m_isHit[i] = false;
			}
		}

		if (m_barCollider[i].GetPosition().x <= BAR_MINPOS)
		{
			m_barCollider[i].SetPosition(DirectX::SimpleMath::Vector2(BAR_MINPOS, m_barCollider[i].GetPosition().y));
		}
		if (m_barCollider[i].GetPosition().x >= BAR_MAXPOS)
		{
			m_barCollider[i].SetPosition(DirectX::SimpleMath::Vector2(BAR_MAXPOS, m_barCollider[i].GetPosition().y));
		}
	}

	if (!IsHit(collider, m_uiCollider)&& mouseTK->leftButton == mouseTK->PRESSED)
	{
		m_se = Resources::GetInstance()->GetSESound(L"ButtonClick.wav", Resources::GetInstance()->GetListener().Position, false);
		m_isOpen = false;
	}

	Resources::GetInstance()->SetBGMVolume((m_barCollider[0].GetPosition().x - BAR_MINPOS) / (BAR_MAXPOS - BAR_MINPOS));
	Resources::GetInstance()->SetSEVolume((m_barCollider[1].GetPosition().x - BAR_MINPOS) / (BAR_MAXPOS - BAR_MINPOS));
}



/// <summary>
/// 描画処理
/// </summary>
void AudioUI::Draw()
{
	m_audioUI.Draw(DirectX::SimpleMath::Vector2(640, 360), DirectX::SimpleMath::Vector2(1743, 850), 0.4f);

	for (int i = 0; i < BAR_COUNT; i++)
	{
		m_barCollider[i].Draw(DirectX::Colors::Gray);
	}
}



/// <summary>
/// 終了処理
/// </summary>
void AudioUI::Finalize()
{
}
