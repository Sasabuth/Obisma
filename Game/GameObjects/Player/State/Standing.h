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
class Standing : public IState
{
private:


// 変数
private:
	UserResources* m_userResources;

	Player* m_player;

	DirectX::Model* m_model;  // モデル

// 関数
public:
	// コンストラクタ
	Standing(Player* player);

	// デストラクタ
	~Standing() override;

	// 初期化
	void Initialize() override;

	// 更新
	void Update(float elapsedTime) override;

	// 描画
	void Render() override;

	// 終了処理
	void Finalize() override;
};

