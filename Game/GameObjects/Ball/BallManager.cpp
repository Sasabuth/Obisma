/// <summary>
/// BallManagerに関するソースファイル
/// </summary>
/// <author>仲森智史</author>

// ヘッダファイルの読み込み
#include "pch.h"
#include "BallManager.h"

#include "Game/GameObjects//Camera/Camera.h"
#include "Game/GameObjects/Field/Field.h"
#include "DebugDraw.h"
#include "Game/Commons/Resources.h"
#include "Game/Commons/Factory.h"



/// <summary>
/// コンストラクタ
/// </summary>
BallManager::BallManager(Field* pField)
	: m_pField(pField)
{
}



/// <summary>
/// デストラクタ
/// </summary>
BallManager::~BallManager()
{
}


/// <summary>
/// 初期化処理
/// </summary>
void BallManager::Initialize()
{
	for (int i = 0; i < Resources::GetInstance()->GetJson(L"Ball.json")["Count"]; i++)
	{
		std::unique_ptr<Ball> ball = Factory::CreateBall(m_pField, DirectX::SimpleMath::Vector3(
			Resources::GetInstance()->GetJson(L"Ball.json")["Position"][std::to_string(i)]["x"],
			Resources::GetInstance()->GetJson(L"Ball.json")["Position"][std::to_string(i)]["y"],
			Resources::GetInstance()->GetJson(L"Ball.json")["Position"][std::to_string(i)]["z"])
		);
		Add(ball);
	}
}



/// <summary>
/// 更新処理
/// </summary>
/// <param name="elapsedTime">経過時間</param> 
void BallManager::Update(float elapsedTime)
{
	for (int i = 0; i < m_balls.size(); i++)
	{
		m_balls[i]->Update(elapsedTime);
	}
}



/// <summary>
/// 描画処理
/// </summary>
void BallManager::Render()
{
	for (int i = 0; i < m_balls.size(); i++)
	{
		m_balls[i]->Render();
	}
}



/// <summary>
/// 終了処理
/// </summary>
void BallManager::Finalize()
{
	for (int i = 0; i < m_balls.size(); i++)
	{
		m_balls[i]->Finalize();
	}
}