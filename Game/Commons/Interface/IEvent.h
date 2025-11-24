/// <summary>
/// IEventに関するヘッダファイル
/// </summary>
/// <author>仲森智史</author>

// 多重インクルードの防止
#pragma once

class IEvent
{
// 関数
public:
	// デストラクタ
	~IEvent() = default;

	// プレイヤーイベント
	enum PlayerEvent
	{
		STAND = 0,
		RUN,
		CATCH,
		THROW
	};

	enum EnemyEvent
	{

	};
};

