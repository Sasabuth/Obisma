/// <summary>
/// Buttonに関するソースファイル
/// </summary>
/// <author>仲森智史</author>
/// <date></date>

// 多重インクルードの防止
#pragma once

// ヘッダファイルの読み込み
#include "pch.h"
#include "Button.h"

#include "Game/Commons/Resources.h"


/// <summary>
/// コンストラクタ
/// </summary>
Button::Button()
{
}



/// <summary>
/// デストラクタ
/// </summary>
Button::~Button()
{
}



/// <summary>
/// 描画処理
/// </summary>
void Button::Draw(DirectX::XMVECTOR color)
{
	// コライダー描画
	/*m_collider.Draw(DirectX::Colors::Yellow);*/

	m_texture.Draw(m_collider.GetPosition(), m_collider.GetSize(), m_collider.GetScale(), color);
}



/// <summary>
/// クリック
/// </summary>
void Button::Click()
{
	if (m_clickFunc)
	{  
		m_clickFunc();

		m_se = Resources::GetInstance()->GetSESound(L"ButtonClick.wav", Resources::GetInstance()->GetListener().Position, false);
	}
}
