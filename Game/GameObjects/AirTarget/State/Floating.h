/// <summary>
/// Floatingに関するヘッダファイル
/// </summary>
/// <author>仲森智史</author>


// 多重インクルードの防止
#pragma once

// ヘッダファイルの読み込み
#include "Game/Commons/Interface/IState.h"
#include "Game/Commons/UserResources.h"


// クラスの定義
class AirTarget;



// クラスの定義
class Floating : public IState
{
private:


// 変数
private:
	// ユーザーリソース
	UserResources* m_pUserResources;

	// 空中の的
	AirTarget* m_pAirTarget;

	// 回転
	float m_rotate;


// 関数
public:
	// コンストラクタ
	Floating(AirTarget* pAirTarget);

	// デストラクタ
	~Floating() override;

	// 初期化
	void Initialize() override;

	// 更新
	void Update(float elapsedTime) override;

	// 描画
	void Render() override;

	// 終了処理
	void Finalize() override;

	// 特定のイベントの処理
	void EventHandle(Event e) override;
};

