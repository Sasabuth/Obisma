/// <summary>
/// Messengerに関するヘッダファイル
/// </summary>
/// <author>仲森智史</author>

#pragma once
#ifndef MESSENGER_DEFINED
#define MESSENGER_DEFINED
#include "Game/Commons/Message.h"
#include "Game/Commons/Interface/IObject.h"
#include <unordered_map>

// Messengerクラス
class Messenger
{
public:
	// オブジェクトを取得する
	IObject* GetObject(int objectID);
	// 経過時間を取得する
	float GetElapsedTime() const { return m_elapsedTime; }
	// 経過時間を設定する
	void SetElapsedTime(const float& elapsedTime) { m_elapsedTime = elapsedTime; }

public:
	// Messengerクラスのインスタンスを取得する
	static Messenger* GetInstance();
	// Messengerクラスのインスタンスを破棄する
	static void DestroyInstance();
	// オブジェクトを登録する
	void Register(int objectID, IObject* object);
	// オブジェクトの登録を解除する
	void UnRegister(int objectID);
	// メッセージを通知する
	void Notify(int objectID, Message::MessageID messageID);
	// 一定時間通知拒否
	void NotifyAfterDelay(int objectID, Message::MessageID messageID, float delaySeconds);
	// 強制通知する
	void NotifyForce(int objectID, Message::MessageID messageID, float delaySeconds);
	// 更新
	void Update(float elapsedTime);

private:
	// コピー・ムーブ禁止
	Messenger(const Messenger&) = delete;
	Messenger& operator=(const Messenger&) = delete;
	Messenger(Messenger&&) = delete;
	Messenger& operator=(Messenger&&) = delete;
	// コンストラクタ
	Messenger();

private:
	// メッセンジャーインスタンス
	static std::unique_ptr<Messenger> s_messenger;
	// オブジェクトIDとオブジェクトマッピング
	std::unordered_map<int, IObject*> m_objects;
	// オブジェクトIDと時間マッピング
	std::unordered_map<int, float> m_objectsAfterDelay;
	// 経過時間
	float m_elapsedTime;
};

#endif		// MESSENGER_DEFINED

