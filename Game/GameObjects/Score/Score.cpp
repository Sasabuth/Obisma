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

// 名前の省略
using namespace DirectX;


/// <summary>
/// コンストラクタ
/// </summary>
Score::Score()
	: m_userResources(nullptr)
	, m_score(0)
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

	m_scoreSprite.SetTexture(Resources::GetInstance()->GetScoreFontTexture(index));
	m_frameSprite.SetTexture(Resources::GetInstance()->GetScoreFrameTexture(index));
	m_faceSprite.SetTexture(Resources::GetInstance()->GetFaceTexture(index));
}



/// <summary>
/// 描画処理
/// </summary>
void Score::Render()
{
	m_frameSprite.Draw(m_position, SimpleMath::Vector2(0, 0), 0.3f);
	m_faceSprite.Draw(m_position, SimpleMath::Vector2(0, 0), 0.1f);
	ScoreDraw(m_position.x+80, m_position.y+10, (int)m_score, 1.0f);

	auto debagFont = m_userResources->GetDebugFont();
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



/// <summary>
/// スコアの描画
/// </summary>
/// <param name="x">X座標</param>
/// <param name="y">Y座標</param>
/// <param name="score">スコア</param>
/// <param name="size">サイズ</param>
void Score::ScoreDraw(float x, float y, int score, float size)
{
	// スコアフォントの描画
	float posX = x;
	float posY = y;

	int numDidits = 2;
	//while (score != 0)
	//{
	//	score /= 10;
	//	++numDidits;
	//}

	posX += static_cast<int>(NUMBER_WIDTH * (numDidits));

	for (int i = 0; i < numDidits; i++)
	{
		int number = score % 10 + 1;

		int sourceX = static_cast<int>(number * NUMBER_WIDTH);
		m_scoreSprite.Draw(SimpleMath::Vector2(posX, posY), SimpleMath::Vector2((float)sourceX, NUMBER_HEIGHT), NUMBER_WIDTH, size);

		score /= 10;
		posX -= static_cast<int>(NUMBER_WIDTH);
	}
}
