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

// 名前の省略
using namespace DirectX;


/// <summary>
/// コンストラクタ
/// </summary>
ScoreManager::ScoreManager()
	: m_userResources(nullptr)
	, m_score(0)
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

	m_score = 0;

	m_sprite.SetTexture(Resources::GetInstance()->GetScoreFont());
	m_frameSprite.SetTexture(Resources::GetInstance()->GetPlayerFrame());
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
	/*m_frameSprite.Draw(SimpleMath::Vector2(20, 20), SimpleMath::Vector2(0, 0), 0.3f);*/
	ScoreDraw(0, 0, m_score, 1.0f);

	auto debagFont = m_userResources->GetDebugFont();

	debagFont->Render(L"score", m_score);
	
}



/// <summary>
/// 終了処理
/// </summary>
void ScoreManager::Finalize()
{
}



/// <summary>
/// スコアの設定
/// </summary>
/// <param name="ballColorNum">ボールの色の番号</param>
void ScoreManager::SetScore(int ballColorNum)
{
	m_score += 1;
}



/// <summary>
/// スコアの描画
/// </summary>
/// <param name="x">X座標</param>
/// <param name="y">Y座標</param>
/// <param name="score">スコア</param>
/// <param name="size">サイズ</param>
void ScoreManager::ScoreDraw(int x, int y, int score, int size)
{
	// スコアフォントの描画
	int posX = x;
	int posY = y;

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
		m_sprite.Draw(SimpleMath::Vector2(posX, posY), SimpleMath::Vector2(sourceX, 50), NUMBER_WIDTH, size);
		/*m_sprite.Draw(SimpleMath::Vector2(500, 0), SimpleMath::Vector2(0,0), size);*/

		score /= 10;
		posX -= static_cast<int>(NUMBER_WIDTH);
	}
}
