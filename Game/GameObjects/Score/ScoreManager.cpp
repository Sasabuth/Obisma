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

// 名前の省略
using namespace DirectX;


/// <summary>
/// コンストラクタ
/// </summary>
ScoreManager::ScoreManager()
	: m_userResources(nullptr)
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
	// ユーザーリソースの取得
	m_userResources = UserResources::GetUserResource();
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




int ScoreManager::GetTopScoreNumber() const
{
	float score = m_scores[0]->GetScore();
	int num = 0;

	for (int i = 1; i < m_scores.size(); i++)
	{
		float score2 = m_scores[i]->GetScore();

		if (score < score2)
		{
			score = score2;
			num = i;
		}
	}

	return num;
}
