/// <summary>
/// IStateに関するヘッダファイル
/// </summary>
/// <author>仲森智史</author>

// 多重インクルードの防止
#pragma once

class IState
{
// 定数
public:
	// イベント
	enum Event
	{
		STAND = 0,
		RUN,
		CATCH,
		THROW
	};


// 関数
public:
	// デストラクタ
	virtual ~IState() = default;

	// 初期化
	virtual void Initialize() = 0;

	// 更新
	virtual void Update(float elapsedTime) = 0;

	// 描画
	virtual void Render() = 0;

	// 終了処理
	virtual void Finalize() = 0;

	// 特定のイベントの処理
	virtual void EventHandle(Event e) = 0;
};

