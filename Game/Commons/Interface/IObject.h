/// <summary>
/// IObjectに関するヘッダファイル
/// </summary>
/// <author>仲森智史</author>

// 多重インクルードの防止
#pragma once

// ヘッダファイルの読み込み
#include "Game/Commons/Message.h"


// クラスの定義
class IObject 
{
public:
	// メッセージを受け取る
	virtual void OnMessegeAccepted(Message::MessageID messageID) = 0;
};

