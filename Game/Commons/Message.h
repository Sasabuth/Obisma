/// <summary>
/// Messageに関するヘッダファイル
/// </summary>
/// <author>仲森智史</author>


#pragma once
#ifndef MESSAGE_DEFINED
#define MESSAGE_DEFINED

// Messageクラス
// ・IDLINGをOFFENSIVE_IDLINGに変更する
class Message
{
public:
	// メッセージID
	enum MessageID : int  
	{ 
		// プレイヤー
		STANDING = 0,		// 「立つ」状態
		RUNNING,			// 「走る」状態
		THROWING,			// 「投げる」状態
		CATCHING,			// 「キャッチ」状態
		DIZZYING,			// 「目が回る」状態
	};
};

#endif		// MESSAGE_DEFINED

