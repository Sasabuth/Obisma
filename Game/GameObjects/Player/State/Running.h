/// <summary>
/// プレイヤーに関するヘッダファイル
/// </summary>
/// <author>仲森智史</author>
/// <date>2025/05/21</date>

// 多重インクルードの防止
#pragma once

// ヘッダファイルの読み込み
#include "Game/Commons/Interface/IState.h"
#include "Game/Commons/Collision.h"
#include "Game/Commons/UserResources.h"


// クラスの定義
class Player;



// クラスの定義
class Running : public IState
{
private:
	static constexpr float ROTATE_SPEED = 0.5f;

// 変数
private:
	UserResources* m_userResources;

	Player* m_player;

// 関数
public:
	// コンストラクタ
	Running(Player* player);

	// デストラクタ
	~Running() override;

	// 初期化
	void Initialize() override;

	// 更新
	void Update(float elapsedTime) override;

	// 描画
	void Render() override;

	// 終了処理
	void Finalize() override;
};

