/// <summary>
/// Fieldに関するソースファイル
/// </summary>
/// <author>仲森智史</author>

// ヘッダファイルの読み込み
#include "pch.h"
#include "Field.h"

#include "Game/Scenes/TutorialScene.h"
#include "Game/Commons/Interface/IEntity.h"
#include "Game/Commons/Resources.h"
#include "Game/Commons/Factory.h"
#include "Game/Commons/GameObjectMessenger.h"
#include "Game/GameObjects/Score/ScoreManager.h"




/// <summary>
/// コンストラクタ
/// </summary>
Field::Field()
	: m_pUserResources(nullptr)
	, m_position{}
	, m_model{}
	, m_skydomeModel{}
	, m_rotate(0)
{
	// オブジェクト番号とオブジェクトを登録する
	GameObjectMessenger::GetInstance()->Register(Factory::FIELD, this);
}



/// <summary>
/// デストラクタ
/// </summary>
Field::~Field()
{
}



/// <summary>
/// 初期化処理
/// </summary>
/// <param name="stageIndex">ステージの番号</param>
/// <param name="isSkyDome">スカイドーム入れるか</param>
void Field::Initialize(int stageIndex, bool isSkyDome)
{
	// ユーザーリソースの取得
	m_pUserResources = UserResources::GetUserResource();
	auto device = m_pUserResources->GetDeviceResources()->GetD3DDevice();
	auto context = m_pUserResources->GetDeviceResources()->GetD3DDeviceContext();

	// モデルの設定
	std::wstring filename = L"Stage" + std::to_wstring(stageIndex) + L".sdkmesh";
	m_model = Resources::GetInstance()->GetModel(filename.c_str());
	m_model->UpdateEffects(
		// 引数にラムダ式として処理内容を指定する
		[&](DirectX::IEffect* pEffect)
		{
			// BasicEffectにキャストする
			DirectX::BasicEffect* pBasicEffect = dynamic_cast<DirectX::BasicEffect*>(pEffect);

			// ライトをオフにする
			pBasicEffect->SetLightEnabled(0, false);
			pBasicEffect->SetLightEnabled(1, false);
			pBasicEffect->SetLightEnabled(2, false);

			// 自己発光(引数はカラー)
			pBasicEffect->SetEmissiveColor(DirectX::SimpleMath::Vector3(1, 1, 1));
		}
	);

	// スカイドームを入れるなら
	if (isSkyDome)
	{
		m_skydomeModel = Resources::GetInstance()->GetModel(L"skydome.sdkmesh");
		m_skydomeModel->UpdateEffects(
			[&](DirectX::IEffect* pEffect)
			{
				// BasicEffectにキャストする
				DirectX::BasicEffect* pBasicEffect = dynamic_cast<DirectX::BasicEffect*>(pEffect);

				pBasicEffect->SetAmbientLightColor(DirectX::Colors::WhiteSmoke);
			}
		);
	}

	// 座標の初期化
	m_position = DirectX::SimpleMath::Vector3{ 0.0f,0.0f,0.0f };

	// コライダーの初期化
	m_collider.Initialize(context, m_position, MODEL_SCALE);
	m_fieldCollider.Initialize(device, context, m_model, m_position, MODEL_SCALE);

	// ボールマネージャーの初期化
	m_ballManager = Factory::CreateBallManager(this, BallManager::BALLCOUNT);

	// 空中の的の初期化
	m_airTarget = Factory::CreateAirTarget(DirectX::SimpleMath::Vector3{
		Resources::GetInstance()->GetJson(L"AirTarget.json")["Position"]["x"],
		Resources::GetInstance()->GetJson(L"AirTarget.json")["Position"]["y"],
		Resources::GetInstance()->GetJson(L"AirTarget.json")["Position"]["z"]
		}
	);
	// ランダムに座標を設定
	m_airTarget->RandomPosition();

	// プレイヤーの初期化
	m_player = Factory::CreatePlayer(this, DirectX::SimpleMath::Vector3{
		Resources::GetInstance()->GetJson(L"Player.json")["Position"]["x"],
		Resources::GetInstance()->GetJson(L"Player.json")["Position"]["y"],
		Resources::GetInstance()->GetJson(L"Player.json")["Position"]["z"]
		}
	);

	// 敵の初期化
	m_enemy = Factory::CreateEnemy(this, DirectX::SimpleMath::Vector3{
		Resources::GetInstance()->GetJson(L"Enemy.json")["Position"]["x"],
		Resources::GetInstance()->GetJson(L"Enemy.json")["Position"]["y"],
		Resources::GetInstance()->GetJson(L"Enemy.json")["Position"]["z"]
		}
	);
}



/// <summary>
/// チュートリアルの初期化
/// </summary>
/// <param name="stageIndex">ステージの番号</param>
/// <param name="isSkyDome">スカイドーム入れるか</param>
void Field::TutorialInitialize(int stageIndex, bool isSkyDome)
{
	// ユーザーリソースの取得
	m_pUserResources = UserResources::GetUserResource();
	auto device = m_pUserResources->GetDeviceResources()->GetD3DDevice();
	auto context = m_pUserResources->GetDeviceResources()->GetD3DDeviceContext();

	// モデルの設定
	std::wstring filename = L"Stage" + std::to_wstring(stageIndex) + L".sdkmesh";
	m_model = Resources::GetInstance()->GetModel(filename.c_str());
	m_model->UpdateEffects(
		// 引数にラムダ式として処理内容を指定する
		[&](DirectX::IEffect* pEffect)
		{
			// BasicEffectにキャストする
			DirectX::BasicEffect* pBasicEffect = dynamic_cast<DirectX::BasicEffect*>(pEffect);

			// ライトをオフにする
			pBasicEffect->SetLightEnabled(0, false);
			pBasicEffect->SetLightEnabled(1, false);
			pBasicEffect->SetLightEnabled(2, false);

			// 自己発光(引数はカラー)
			pBasicEffect->SetEmissiveColor(DirectX::SimpleMath::Vector3(1, 1, 1));
		}
	);

	// スカイドームを入れるなら
	if (isSkyDome)
	{
		m_skydomeModel = Resources::GetInstance()->GetModel(L"skydome.sdkmesh");
		m_skydomeModel->UpdateEffects(
			[&](DirectX::IEffect* pEffect)
			{
				// BasicEffectにキャストする
				DirectX::BasicEffect* pBasicEffect = dynamic_cast<DirectX::BasicEffect*>(pEffect);

				pBasicEffect->SetAmbientLightColor(DirectX::Colors::WhiteSmoke);
			}
		);
	}

	// 座標の初期化
	m_position = DirectX::SimpleMath::Vector3{ 0.0f,0.0f,0.0f };

	// コライダーの初期化
	m_collider.Initialize(context, m_position, MODEL_SCALE);
	m_fieldCollider.Initialize(device, context, m_model, m_position, MODEL_SCALE);

	// ボールマネージャーの初期化
	m_ballManager = Factory::CreateBallManager(this, BallManager::TUTORIAL_BALLCOUNT);

	for (int i = 0; i < m_ballManager->GetObjectCount(); i++)
	{
		m_ballManager->GetBall(i)->SetPosition(DirectX::SimpleMath::Vector3{
		Resources::GetInstance()->GetJson(L"Ball.json")["TutorialPos"]["x"],
		Resources::GetInstance()->GetJson(L"Ball.json")["TutorialPos"]["y"],
		Resources::GetInstance()->GetJson(L"Ball.json")["TutorialPos"]["z"]
			}
		);
	}

	// 空中の的の初期化
	m_airTarget = Factory::CreateAirTarget(DirectX::SimpleMath::Vector3{
		Resources::GetInstance()->GetJson(L"AirTarget.json")["TutorialPos"]["x"],
		Resources::GetInstance()->GetJson(L"AirTarget.json")["TutorialPos"]["y"],
		Resources::GetInstance()->GetJson(L"AirTarget.json")["TutorialPos"]["z"]
		}
	);

	// プレイヤーの初期化
	m_player = Factory::CreatePlayer(this, DirectX::SimpleMath::Vector3{
		Resources::GetInstance()->GetJson(L"Player.json")["Position"]["x"],
		Resources::GetInstance()->GetJson(L"Player.json")["Position"]["y"],
		Resources::GetInstance()->GetJson(L"Player.json")["Position"]["z"]
		}
	);

	// 敵の初期化
	m_enemy = Factory::CreateEnemy(this, DirectX::SimpleMath::Vector3{
		Resources::GetInstance()->GetJson(L"Enemy.json")["Position"]["x"],
		Resources::GetInstance()->GetJson(L"Enemy.json")["Position"]["y"],
		Resources::GetInstance()->GetJson(L"Enemy.json")["Position"]["z"]
		}
	);
}



/// <summary>
/// 更新処理
/// </summary>
/// <param name="pScoreManager">スコアマネージャー</param>
/// <param name="elapsedTime">経過時間</param>
void Field::Update(ScoreManager* pScoreManager, float elapsedTime)
{
	// コライダーの設定
	m_collider.SetPosition(m_position);

	// ステージコライダーの設定
	m_fieldCollider.SetPosition(m_position);

	// プレイヤーの更新
	m_player->Update(elapsedTime);

	// 敵の更新
	m_enemy->Update(elapsedTime);

	// ボールマネージャの更新
	m_ballManager->Update(elapsedTime);

	// 空中の的の更新
	m_airTarget->Update(elapsedTime);

	// ポリゴンの当たり判定
	ResolveEntityFieldCollision(m_player.get());
	ResolveEntityFieldCollision(m_enemy.get());
	for (int i = 0; i < m_ballManager->GetObjectCount(); i++)
	{
		ResolveEntityFieldCollision(m_ballManager->GetBall(i));

		// ボールと空中の的が当たったら
		if (IsHit(m_ballManager->GetBall(i)->GetCollider(), m_airTarget->GetCollider()))
		{
			// ロックオンを外す
			m_player->SetIsLockOn(false);
			// 空中の的のステートを変更
			m_airTarget->ChangeState(m_airTarget->GetHitting());
			// スコアを増やす
			pScoreManager->GetScore(m_ballManager->GetBall(i)->GetBallColorNum())->ScoreUp();
		}
	}

	// 敵との押し出し
	if (IsHit(m_player->GetCollider(), m_enemy->GetCollider()))
	{
		m_player->CorrectOverlap(*m_enemy.get());
	}
}



/// <summary>
/// タイトルの更新処理
/// </summary>
/// <param name="elapsedTime"></param> 経過時間
void Field::TitleUpdate()
{
	// コライダーの設定
	m_collider.SetPosition(m_position);

	// ステージコライダーの設定
	m_fieldCollider.SetPosition(m_position);
}



/// <summary>
/// チュートリアルの更新処理
/// </summary>
/// <param name="scene">シーン</param>
/// <param name="elapsedTime">経過時間</param>
void Field::TutorialUpdate(TutorialScene* scene, ScoreManager* pScoreManager, float elapsedTime)
{
	// プレイヤーの更新
	m_player->Update(elapsedTime);

	// シーンがボールキャッチだったら
	if (scene->GetTutorialIndex() == TutorialScene::ORDER::BALL_CATCH)
	{
		// 敵を更新させたいなら更新する
		if (IsEnemyUpdate(scene))
		{
			m_enemy->Update(elapsedTime);
		}
		else
		{
			m_enemy->ChangeState(m_enemy->GetStanding());
		}

		// ボールが止まっていたら敵の手にボールを持たせる
		if (m_ballManager->GetBall(0)->GetCurrentState() == m_ballManager->GetBall(0)->GetStopping())
		{
			m_ballManager->GetBall(0)->SetPosition(m_enemy->GetPosition());
		}
	}

	// ボールマネージャーの更新
	m_ballManager->Update(elapsedTime);

	// 空中の的の更新
	m_airTarget->Update(elapsedTime);

	// ポリゴンの当たり判定
	ResolveEntityFieldCollision(m_player.get());
	ResolveEntityFieldCollision(m_enemy.get());
	for (int i = 0; i < m_ballManager->GetObjectCount(); i++)
	{
		ResolveEntityFieldCollision(m_ballManager->GetBall(i));

		// ボールと空中の的が当たったら
		if (IsHit(m_ballManager->GetBall(i)->GetCollider(), m_airTarget->GetCollider()))
		{
			// ロックオンを外す
			m_player->SetIsLockOn(false);
			// 空中の的のステートを変更
			m_airTarget->ChangeState(m_airTarget->GetHitting());
			// スコアを増やす
			pScoreManager->GetScore(m_ballManager->GetBall(i)->GetBallColorNum())->ScoreUp();
		}
	}

	// 敵との押し出し
	if (IsHit(m_player->GetCollider(), m_enemy->GetCollider()))
	{
		m_player->CorrectOverlap(*m_enemy.get());
	}
}



/// <summary>
/// 描画処理
/// </summary>
void Field::Render()
{
	// デバックフォントの描画
	/*auto debugFont = m_pUserResources->GetDebugFont();*/

	auto context = m_pUserResources->GetDeviceResources()->GetD3DDeviceContext();
	auto states = m_pUserResources->GetCommonStates();
	auto view = m_pUserResources->GetView();
	auto proj = m_pUserResources->GetProject();

	// ワールド座標
	DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::CreateScale(MODEL_SCALE) * DirectX::SimpleMath::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_rotate)) * DirectX::SimpleMath::Matrix::CreateTranslation(m_position);

	// モデルの描画
	m_model->Draw(context, *states, world, *view, *proj);


	// デバック
	/*m_fieldCollider.Draw(context, states, *view, *proj);*/

	// スカイドームの描画
	if (m_skydomeModel)
	{
		DirectX::SimpleMath::Matrix sWorld = DirectX::SimpleMath::Matrix::CreateScale(SKYDOME_SCALE) * DirectX::SimpleMath::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_rotate / 4)) * DirectX::SimpleMath::Matrix::CreateTranslation(m_position);
		m_skydomeModel->Draw(context, *states, sWorld, *view, *proj);
	}

	// 空中の的の描画
	m_airTarget->Render();

	// プレイヤーの描画
	m_player->Render();

	// 敵の描画
	m_enemy->Render();

	// ボールマネージャーの描画
	m_ballManager->Render();
}



/// <summary>
/// タイトルの描画処理
/// </summary>
void Field::TitleRender()
{
	auto context = m_pUserResources->GetDeviceResources()->GetD3DDeviceContext();
	auto states = m_pUserResources->GetCommonStates();
	auto view = m_pUserResources->GetView();
	auto proj = m_pUserResources->GetProject();

	// ワールド座標
	DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::CreateScale(MODEL_SCALE) * DirectX::SimpleMath::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_rotate)) * DirectX::SimpleMath::Matrix::CreateTranslation(m_position);

	// モデルの描画
	m_model->Draw(context, *states, world, *view, *proj);

	// スカイドームの描画
	if (m_skydomeModel)
	{
		DirectX::SimpleMath::Matrix sWorld = DirectX::SimpleMath::Matrix::CreateScale(SKYDOME_SCALE) * DirectX::SimpleMath::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_rotate / 4)) * DirectX::SimpleMath::Matrix::CreateTranslation(m_position);
		m_skydomeModel->Draw(context, *states, sWorld, *view, *proj);
	}
}



/// <summary>
/// 終了処理
/// </summary>
void Field::Finalize()
{
	// ボールマネージャーの終了
	m_ballManager->Finalize();

	// プレイヤーの終了
	m_player->Finalize();

	// 敵の終了
	m_enemy->Finalize();

	// 空中の的の終了
	m_airTarget->Finalize();
}



/// <summary>
/// 上方向の補正
/// </summary>
/// <param name="iEntity">実体</param>
/// <returns>重力</returns>
DirectX::SimpleMath::Vector3 Field::CorrectUp(IEntity* iEntity)
{
	// 重力の方向
	DirectX::SimpleMath::Vector3 gravityDir = m_position - iEntity->GetPosition();
	gravityDir.Normalize();

	// 方向ベクトルの反転
	DirectX::SimpleMath::Vector3 targetUp;
	targetUp = -gravityDir;

	// 現在の姿勢制御
	DirectX::SimpleMath::Vector3 currentUp = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitY, iEntity->GetRotation());

	// 回転軸の計算
	DirectX::SimpleMath::Vector3 axis = currentUp.Cross(targetUp);
	axis.Normalize();

	// 回転角の計算
	float dot = currentUp.Dot(targetUp);
	float angle = acosf(dot);

	// クォータニオンの作成
	DirectX::SimpleMath::Quaternion q;


	// 角度が少しでもあれば軸を作る
	if (angle > 0.01f && axis.Length() > 0.01f)
	{
		q = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(axis, angle);
	}
	// なければ何もしない
	else
	{
		q = DirectX::SimpleMath::Quaternion::Identity;
	}

	// 回転の設定
	iEntity->SetRotation(iEntity->GetRotation() * q);

	// 重力を返す
	return gravityDir * 3;
}



/// <summary>
/// 上方向の補正
/// </summary>
/// <param name="iEntity">実体</param>
/// <param name="vector">方向</param>
/// <returns>重力</returns>
DirectX::SimpleMath::Vector3 Field::CorrectUp(IEntity* iEntity, DirectX::SimpleMath::Vector3 vector)
{
	// 重力の方向
	DirectX::SimpleMath::Vector3 gravityDir = -vector;
	gravityDir.Normalize();

	// 方向ベクトルの反転
	DirectX::SimpleMath::Vector3 targetUp;
	targetUp = -gravityDir;

	// 現在の姿勢制御
	DirectX::SimpleMath::Vector3 currentUp = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitY, iEntity->GetRotation());

	// 回転軸の計算
	DirectX::SimpleMath::Vector3 axis = currentUp.Cross(targetUp);
	axis.Normalize();

	// 回転角の計算
	float dot = currentUp.Dot(targetUp);
	float angle = acosf(dot);

	// クォータニオンの作成
	DirectX::SimpleMath::Quaternion q;

	// 角度が少しでもあれば軸を作る
	if (angle > 0.01f && axis.Length() > 0.01f)
	{
		q = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(axis, angle);
	}
	// なければ何もしない
	else
	{
		q = DirectX::SimpleMath::Quaternion::Identity;
	}

	// 回転の設定
	iEntity->SetRotation(iEntity->GetRotation() * q);

	// 重力を返す
	return gravityDir * 3;
}



/// <summary>
/// 実体とフィールドの衝突解決
/// </summary>
/// <param name="ray">レイ</param>
/// <param name="pIEntity">実体</param>
/// <param name="pField">フィールド</param>
void Field::ResolveEntityFieldCollision(IEntity* pIEntity)
{
	// レイ
	DirectX::SimpleMath::Ray ray{ pIEntity->GetPosition(), pIEntity->GetGravity() };
	// 座標
	DirectX::SimpleMath::Vector3 pos;
	// 方向ベクトル
	DirectX::SimpleMath::Vector3 vector;

	// グループ分回す
	for (int i = 0; i < m_fieldCollider.GetGroupCount(); i++)
	{
		// 球と立方体の当たり判定を調べる
		if (IsHit(ray.position, ray.direction, m_fieldCollider.GetGroup(i).position, m_fieldCollider.GetGroup(i).extent / 2))
		{
			// レイと三角形の当たり判定
			if (RaycastTriangles(i, ray, pIEntity, pos, vector))
			{
				i = (int)m_fieldCollider.GetGroupCount();
			}
		}
	}

	// 実体の押し出し
	EntityCorrectOverlap(pIEntity, pos);

	// 上方向の軸の更新
	UpdateCorrectUp(pIEntity, pos, vector);
}



/// <summary>
/// メッセージの取得
/// </summary>
/// <param name="messageID">メッセージID</param>
void Field::OnMessegeAccepted(Message::MessageID messageID)
{
	UNREFERENCED_PARAMETER(messageID);
}



/// <summary>
/// コライダーの取得
/// </summary>
/// <returns>コライダー</returns>
SphereCollider& Field::GetCollider()
{
	return m_collider;
}



/// <summary>
/// 敵を更新させるか
/// </summary>
/// <param name="scene">シーン</param>
/// <returns>更新させるか</returns>
bool Field::IsEnemyUpdate(TutorialScene* scene)
{
	if (!scene->GetIsCheck() && m_player->GetInvincibleTime() < 0.001f && m_player->GetCurrentState() != m_player->GetDizzying())
	{
		return true;
	}

	return false;
}



/// <summary>
/// レイと三角形の当たり判定
/// </summary>
/// <param name="groupIndex">グループ番号</param>
/// <param name="ray">レイ</param>
/// <param name="pIEntity">実体</param>
/// <param name="pos">座標</param>
/// <param name="vector">ベクトル</param>
bool Field::RaycastTriangles(int groupIndex, DirectX::SimpleMath::Ray ray, IEntity* pIEntity, DirectX::SimpleMath::Vector3& pos, DirectX::SimpleMath::Vector3& vector)
{
	bool isHit = false;

	// ワールド座標
	DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::CreateScale(m_fieldCollider.GetScale()) *
		DirectX::SimpleMath::Matrix::CreateTranslation(m_fieldCollider.GetPosition());

	// 当たっているグループが持っている三角形を調べる
	for (size_t i = 0; i < m_fieldCollider.GetGroup(groupIndex).index.size(); i++)
	{
		// 三角形の番号
		int index = (int)m_fieldCollider.GetGroup(groupIndex).index[i];

		// 三角形の点のワールド座標を取得
		DirectX::SimpleMath::Vector3 p0 = DirectX::SimpleMath::Vector3::Transform(m_fieldCollider.GetVertices(m_fieldCollider.GetIndices(index)).position, world);
		DirectX::SimpleMath::Vector3 p1 = DirectX::SimpleMath::Vector3::Transform(m_fieldCollider.GetVertices(m_fieldCollider.GetIndices(index + 1)).position, world);
		DirectX::SimpleMath::Vector3 p2 = DirectX::SimpleMath::Vector3::Transform(m_fieldCollider.GetVertices(m_fieldCollider.GetIndices(index + 2)).position, world);

		// 三角形の中心から遠かったら当たってないことにする
		DirectX::SimpleMath::Vector3 center = (p0 + p1 + p2) / 3.0f;
		float length = (ray.position - center).Length();
		if (length > LENGTH)
		{
			continue;
		}

		// 当たった座標
		DirectX::SimpleMath::Vector3 pos1;
		// レイと三角形が当たっているか
		if (IsHit(ray.position, ray.direction, p0, p1, p2, pos))
		{
			// 三角形の法線ベクトルを入れる
			vector = DirectX::SimpleMath::Vector3::Lerp(
				-pIEntity->GetGravity(),
				m_fieldCollider.GetNormalVector((int)index),
				0.3f
			);

			isHit = true;
		}
	}

	return isHit;
}



/// <summary>
/// 実体との押し出し
/// </summary>
/// <param name="pIEntity">実体</param>
/// <param name="pos">座標</param>
void Field::EntityCorrectOverlap(IEntity* pIEntity, DirectX::SimpleMath::Vector3& pos)
{
	// 座標とレイの衝突点の距離がコライダーの半径より小さかったら当たっている
	DirectX::SimpleMath::Vector3 dir = pIEntity->GetPosition() - pos;
	if (dir.Length() < pIEntity->GetCollider().GetRadius())
	{
		// 押し出しをする
		pIEntity->CorrectOverlap(pos);
	}
	else
	{
		// 万が一ステージに埋まったら
		if ((pIEntity->GetPosition() - m_position).Length() < (pIEntity->GetShadowHitPos() - m_position).Length())
		{
			// Y軸ベクトル
			DirectX::SimpleMath::Vector3 currentUp = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitY, pIEntity->GetRotation());
			// 当たった座標
			DirectX::SimpleMath::Vector3 hitPos = pIEntity->GetShadowHitPos();

			// ベクトル方向にコライダーの半径分押し出す
			pIEntity->SetPosition(hitPos + currentUp * pIEntity->GetCollider().GetRadius());
		}
	}
}



/// <summary>
/// 上方向の軸の更新
/// </summary>
/// <param name="pIEntity">実体</param>
/// <param name="pos">座標</param>
/// <param name="vector">ベクトル</param>
void Field::UpdateCorrectUp(IEntity* pIEntity, DirectX::SimpleMath::Vector3& pos, DirectX::SimpleMath::Vector3& vector)
{
	// 法線ベクトルがあったら
	if (vector.Length() >= 0.00001f)
	{
		// 重力の設定
		pIEntity->SetGravity(CorrectUp(pIEntity, vector));
		// 影の座標を当たった座標にする
		pIEntity->SetShadowHitPos(pos);
	}
	else
	{
		// 重力の設定
		pIEntity->SetGravity(CorrectUp(pIEntity));
	}
}
