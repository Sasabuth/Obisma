/// <summary>
/// Hittingに関するヘッダファイル
/// </summary>
/// <author>仲森智史</author>


// 多重インクルードの防止
#pragma once

// ヘッダファイルの読み込み
#include "Game/Commons/Interface/IState.h"
#include "Game/Commons/UserResources.h"
#include <random>



// クラスの定義
class AirTarget;



// クラスの定義
class Hitting : public IState
{
// 定数
private:
	// パーティクルカウント
	static constexpr int PARTICLE_COUNT = 20;
	// パーティクルのライフ時間
	static constexpr float PARTICLE_LIFE = 3.0f;

// 変数
private:
	// 空中の的
	AirTarget* m_pAirTarget;

	// エフェクトを出したか
	bool m_isEffect;

	// 座標を設定したか
	bool m_isSetPosition;


// 関数
public:
	// コンストラクタ
	Hitting(AirTarget* pAirTarget);

	// デストラクタ
	~Hitting() override;

	// 初期化
	void Initialize() override;

	// 更新
	void Update(float elapsedTime) override;

	// 描画
	void Render() override;

	// 終了処理
	void Finalize() override;
};

