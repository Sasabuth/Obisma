/// <summary>
/// GameObjectMessengerに関するソースファイル
/// </summary>
/// <author>仲森智史</author>

#include "pch.h"
#include "Game/Commons/GameObjectMessenger.h"

// s_messengerを初期化する
std::unique_ptr<GameObjectMessenger> GameObjectMessenger::s_messenger = nullptr;


/// <summary>
/// コンストラクタ
/// </summary>
GameObjectMessenger::GameObjectMessenger()
	:
	m_elapsedTime{},
	m_objects{}
{
}



/// <summary>
/// インスタンスの取得
/// </summary>
/// <returns>メッセンジャー</returns>
GameObjectMessenger* GameObjectMessenger::GetInstance()
{
	// GameObjectMessengerクラスのインスタンス生成されていない場合
	if (s_messenger == nullptr)
	{
		// GameObjectMessengerクラスのインスタンスを生成する
		s_messenger = std::unique_ptr<GameObjectMessenger>(new GameObjectMessenger());
	}
	// GameObjectMessengerクラスのインスタンスを返す
	return s_messenger.get();
}



/// <summary>
/// インスタンスの破棄
/// </summary>
void GameObjectMessenger::DestroyInstance()
{
	// GameObjectMessengerクラスのインスタンスをリセットする
	s_messenger.reset();
}



/// <summary>
/// オブジェクトの登録
/// </summary>
/// <param name="objectID">オブジェクトID</param>
/// <param name="object">オブジェクト</param>
void GameObjectMessenger::Register(int objectID, IObject* object)
{
	// オブジェクトIDとオブジェクトを登録する
	m_objects.emplace(objectID, object);
}



/// <summary>
/// オブジェクトの登録を解除
/// </summary>
/// <param name="objectID">オブジェクトID</param>
void GameObjectMessenger::UnRegister(int objectID)
{
	// オブジェクトIDを指定してオブジェクトの登録を解除する
	m_objects.erase(objectID);
}



/// <summary>
/// オブジェクトにメッセージを送信
/// </summary>
/// <param name="objectID">オブジェクトID</param>
/// <param name="messageID">メッセージID</param>
void GameObjectMessenger::Notify(int objectID, Message::MessageID messageID)
{
	// メッセージを送信するオブジェクトを検索する
	auto it = m_objects.find(objectID);
	auto delayIt = m_objectsAfterDelay.find(objectID);
	// メッセージを送信するオブジェクトが見つかった場合
	if (it != m_objects.end() && delayIt == m_objectsAfterDelay.end())
	{
		// 送信するオブジェクトのメッセージハンドラを呼び出す
		it->second->OnMessegeAccepted(messageID);
	}
}



/// <summary>
/// 一定時間通知拒否
/// </summary>
/// <param name="objectID">オブジェクトID</param>
/// <param name="messageID">メッセージID</param>
/// <param name="delaySeconds">拒否する時間</param>
void GameObjectMessenger::NotifyAfterDelay(int objectID, Message::MessageID messageID, float delaySeconds)
{
	// オブジェクトにメッセージを送信する
	Notify(objectID, messageID);

	// 遅延メッセージを調べる
	auto delayIt = m_objectsAfterDelay.find(objectID);

	// 遅延メッセージが空だった場合
	if (delayIt == m_objectsAfterDelay.end())
	{
		// 遅延メッセージを追加する
		m_objectsAfterDelay.emplace(objectID, delaySeconds);
	}
}



/// <summary>
/// 強制通知
/// </summary>
/// <param name="objectID">オブジェクトID</param>
/// <param name="messageID">メッセージID</param>
/// <param name="delaySeconds">拒否する時間</param>
void GameObjectMessenger::NotifyForce(int objectID, Message::MessageID messageID, float delaySeconds)
{
	// 遅延メッセージを調べる
	auto delayIt = m_objectsAfterDelay.find(objectID);

	// 遅延メッセージが見つかった場合
	if (delayIt != m_objectsAfterDelay.end())
	{
		// 削除する
		m_objectsAfterDelay.erase(delayIt);
	}

	// オブジェクトにメッセージを送信する
	Notify(objectID, messageID);

	// 遅延メッセージを追加する
	m_objectsAfterDelay.emplace(objectID, delaySeconds);
}



/// <summary>
/// 更新処理
/// </summary>
/// <param name="elapsedTime">経過時間</param>
void GameObjectMessenger::Update(float elapsedTime)
{
	// 登録された遅延メッセージの時間を更新する
	for (auto it = m_objectsAfterDelay.begin(); it != m_objectsAfterDelay.end(); )
	{
		// 遅延時間から経過時間を減らす
		it->second -= elapsedTime;
		// 遅延時間になった場合の処理を行う
		if (it->second <= 0.0f)
		{
			// 遅延メッセージ配列から通知済み遅延メッセージを削除する
			it = m_objectsAfterDelay.erase(it);
		}
		else
		{
			it++;
		}
	}
}



/// <summary>
/// オブジェクトの取得
/// </summary>
/// <param name="objectID">オブジェクトID</param>
/// <returns>オブジェクト</returns>
IObject* GameObjectMessenger::GetObject(int objectID)
{
	// メッセージを送信するオブジェクトを検索する
	auto it = m_objects.find(objectID);
	// メッセージを送信するオブジェクトが見つかった場合
	if (it != m_objects.end())
	{
		// オブジェクトを取得する
		return it->second;
	}
	return nullptr;
}