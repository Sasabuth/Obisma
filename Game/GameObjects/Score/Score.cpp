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
	: m_pUserResources(nullptr)
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
	m_pUserResources = UserResources::GetUserResource();

	// スコアの初期化
	m_score = 0;

	// 座標の設定
	m_position = POSITIONS[index];

	// 番号の設定
	m_index = index;

	// テクスチャの初期化
	m_scoreTexture.SetTexture(Resources::GetInstance()->GetTexture(L"ScoreFont" + std::to_wstring(index) + L".png"));
	m_frameTexture.SetTexture(Resources::GetInstance()->GetTexture(L"ScoreFrame" + std::to_wstring(index) + L".png"));
	m_faceTexture.SetTexture(Resources::GetInstance()->GetTexture(L"Face" + std::to_wstring(index) + L".png"));
	for (int i = 0; i < BALLTEXTURE_COUNT; i++)
	{
		m_ballTexture[i].SetTexture(Resources::GetInstance()->GetTexture(L"Ball2.png"));
	}
}



/// <summary>
/// 描画処理
/// </summary>
void Score::Render()
{
	// テクスチャの描画
	m_frameTexture.Draw(m_position, DirectX::SimpleMath::Vector2::Zero, FRAME_SIZE);
	m_faceTexture.Draw(m_position, DirectX::SimpleMath::Vector2::Zero, FACE_SIZE);
	m_scoreTexture.DigitsDraw(m_position.x + FREAM.pos.x, m_position.y + FREAM.pos.y, FREAM.size.x, FREAM.size.y, (int)m_score, FREAM.scale, 2);
	for (int i = 0; i < BALLTEXTURE_COUNT; i++)
	{
		m_ballTexture[i].Draw(DirectX::SimpleMath::Vector2(m_position.x + i * BALL.pos.x, m_position.y + BALL.pos.y), BALL.size, BALL.scale);
	}

	//auto debagFont = m_pUserResources->GetDebugFont();
	//debagFont->Render(L"score", m_score);
}



/// <summary>
/// スコアを下げる
/// </summary>
void Score::ScoreDown()
{
	// 下げるときに1だったら0にする
	if (m_score == 1)
	{
		m_score = 0;
	}

	// 2を割る
	m_score = std::ceil(m_score /= 2);
}