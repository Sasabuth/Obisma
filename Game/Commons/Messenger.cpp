#include "pch.h"
#include "Game/Commons/Messenger.h"

// s_messengerを初期化する
std::unique_ptr<Messenger> Messenger::s_messenger = nullptr;

// コンストラクタ
Messenger::Messenger()
	:
	m_elapsedTime{},
	m_objects{}
{
}

// Messengerクラスのインスタンスを取得する
Messenger* Messenger::GetInstance()
{
	// Messengerクラスのインスタンス生成されていない場合
	if (s_messenger == nullptr)
	{
		// Messengerクラスのインスタンスを生成する
		s_messenger = std::unique_ptr<Messenger>(new Messenger());
	}
	// Messengerクラスのインスタンスを返す
	return s_messenger.get();
}

// インスタンスを破棄する
void Messenger::DestroyInstance()
{ 
	// Messengerクラスのインスタンスをリセットする
	s_messenger.reset();
}

// オブジェクトを登録する
void Messenger::Register(int objectID, IObject* object)
{
	// オブジェクトIDとオブジェクトを登録する
	m_objects.emplace(objectID, object);
}

// オブジェクトの登録を解除する
void Messenger::UnRegister(int objectID)
{
	// オブジェクトIDを指定してオブジェクトの登録を解除する
	m_objects.erase(objectID);
}

// オブジェクトにメッセージを送信する
void Messenger::Notify(int objectID, Message::MessageID messageID)
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

// 遅延メッセージを通知する
void Messenger::NotifyAfterDelay(int objectID, Message::MessageID messageID, float delaySeconds)
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

// 強制通知する
void Messenger::NotifyForce(int objectID, Message::MessageID messageID, float delaySeconds)
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

void Messenger::Update(float elapsedTime)
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


// オブジェクトを取得する
IObject* Messenger::GetObject(int objectID)
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