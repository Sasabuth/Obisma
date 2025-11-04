/// <summary>
/// Cameraに関するヘッダファイル
/// </summary>
/// <author>仲森智史</author>

// 多重インクルードの防止
#pragma once


// クラスの定義
class Player;


// クラスの定義
class Camera
{
// 変数
private:
	// カメラの距離
	static constexpr float DEFAULT_CAMERA_DISTANCE = 5.0f;

	// 横回転
	float m_yAngle, m_yTmp;

	// 縦回転
	float m_xAngle, m_xTmp;

	// ドラッグされた座標
	int m_x, m_y;

	float m_sx, m_sy;

	// 生成されたビュー行列
	DirectX::SimpleMath::Matrix m_view;

	// スクロールフォイール値
	int m_scrollWheelValue;

	// 視点
	DirectX::SimpleMath::Vector3 m_eye;

	// 注視点
	DirectX::SimpleMath::Vector3 m_target;

	// マウストラッカー
	DirectX::Mouse::ButtonStateTracker m_tracker;

	// スクリーンサイズ
	int m_screenW, m_screenH;


private:
	// モーション
	void Motion(int x, int y);

public:
	// コンストラクタ
	Camera(int windowWidth, int windowHeight);

	// カメラの更新
	void Update();
	void Update(Player* pPlayer, DirectX::SimpleMath::Vector3 upPos, DirectX::SimpleMath::Vector3 field);

	// デバック用
	void DebugMode();

	// カメラのビュー行列の取得
	DirectX::SimpleMath::Matrix GetCameraMatrix();

	// カメラの座標の取得
	DirectX::SimpleMath::Vector3 GetEyePosition();

	// 画面サイズの設定
	void SetWindowSize(int windowWidth, int windowHeight);

	// 画面サイズの取得
	void GetWindowSize(int& windowWidth, int& windowHeight);
};

