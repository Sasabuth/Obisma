/// <summary>
/// Scoreに関するソースファイル
/// </summary>
/// <author>仲森智史</author>


// 多重インクルードの防止
#pragma once

// ヘッダファイルの読み込み
#include "pch.h"
#include "Score.h"

#include "Game/Commons/Resources.h"



/// <summary>
/// コンストラクタ
/// </summary>
Score::Score()
	: m_userResources(nullptr)
	, m_score(0)
	, m_index(0)
{
}



/// <summary>
/// デストラクタ
/// </summary>
Score::~Score()
{
}


/// <summary>
/// 初期化処理
/// </summary>
void Score::Initialize(int index)
{
	// ユーザーリソースの取得
	m_userResources = UserResources::GetUserResource();

	m_score = 0;

	m_position = POSITIONS[index];

	m_index = index;

	m_scoreTexture.SetTexture(Resources::GetInstance()->GetTexture(L"ScoreFont" + std::to_wstring(index) + L".png"));
	m_frameTexture.SetTexture(Resources::GetInstance()->GetTexture(L"ScoreFrame" + std::to_wstring(index) + L".png"));
	m_faceTexture.SetTexture(Resources::GetInstance()->GetTexture(L"Face" + std::to_wstring(index) + L".png"));
}



/// <summary>
/// 描画処理
/// </summary>
void Score::Render()
{
	m_frameTexture.Draw(m_position, DirectX::SimpleMath::Vector2(0, 0), 0.3f);
	m_faceTexture.Draw(m_position, DirectX::SimpleMath::Vector2(0, 0), 0.1f);
	m_scoreTexture.DigitsDraw(m_position.x + 80, m_position.y + 10, NUMBER_WIDTH, NUMBER_HEIGHT, (int)m_score, 1.0f);

	//auto debagFont = m_userResources->GetDebugFont();
	//debagFont->Render(L"score", m_score);
	
}



/// <summary>
/// スコアを下げる
/// </summary>
void Score::ScoreDown()
{
	if (m_score == 1)
	{
		m_score = 0;
	}

	m_score = std::ceil(m_score /= 2);
}