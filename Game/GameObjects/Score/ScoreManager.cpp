/// <summary>
/// ScoreManagerに関するソースファイル
/// </summary>
/// <author>仲森智史</author>


// 多重インクルードの防止
#pragma once

// ヘッダファイルの読み込み
#include "pch.h"
#include "ScoreManager.h"

#include "Game/Commons/Resources.h"
#include "Game/Commons/Factory.h"



/// <summary>
/// コンストラクタ
/// </summary>
ScoreManager::ScoreManager()
	: m_isDraw(true)
{
}



/// <summary>
/// デストラクタ
/// </summary>
ScoreManager::~ScoreManager()
{
}



/// <summary>
/// 初期化処理
/// </summary>
void ScoreManager::Initialize()
{
	m_isDraw = true;
}



/// <summary>
/// 更新処理
/// </summary>
/// <param name="elapsedTime"></param> 経過時間
void ScoreManager::Update(float elapsedTime)
{
	UNREFERENCED_PARAMETER(elapsedTime);
}



/// <summary>
/// 描画処理
/// </summary>
void ScoreManager::Render()
{
	for (int i = 0; i < m_scores.size(); i++)
	{
		m_scores[i]->Render();
	}
}



/// <summary>
/// 高い順に並べる
/// </summary>
void ScoreManager::SortRank()
{
	// スコアをソートする
	std::sort(m_scores.begin(), m_scores.end(), [this](const Score* a, const Score* b) 
		{ 
			// 同じスコアじゃなかったら引き分けにしない
			if (a->GetScore() != b->GetScore())
			{
				m_isDraw = false;
			}

			// 大きいほうを入れる
			return a->GetScore() > b->GetScore();
		}
	);
}
