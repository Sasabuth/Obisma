/// <summary>
/// Cameraに関するソースファイル
/// </summary>
/// <author>仲森智史</author>

#include "pch.h"
#include "Camera.h"

#include "Game/Commons/UserResources.h"
#include "Game/GameObjects/Player/Player.h"



/// <summary>
/// コンストラクタ
/// </summary>
/// <param name="windowWidth">ウインドウサイズ（幅）</param>
/// <param name="windowHeight">ウインドウサイズ（高さ）</param>
Camera::Camera(int windowWidth, int windowHeight)
	: m_yAngle(0.0f), m_yTmp(0.0f), m_xAngle(0.0f), m_xTmp(0.0f), m_x(0), m_y(0), m_scrollWheelValue(0), m_screenW(windowWidth), m_screenH(windowHeight)
{
	SetWindowSize(windowWidth, windowHeight);

	// マウスのフォイール値をリセット
	DirectX::Mouse::Get().ResetScrollWheelValue();
}



void Camera::Update()
{
	// プレイヤー位置
	DirectX::SimpleMath::Vector3 pos = DirectX::SimpleMath::Vector3{ 5,2,0 };

	DirectX::SimpleMath::Vector3 eye = DirectX::SimpleMath::Vector3{ 5,2,-10 };

	// 世界Y軸
	DirectX::SimpleMath::Vector3 up = DirectX::SimpleMath::Vector3::Up;

	// ビュー行列更新
	m_eye = eye;
	m_target = pos;
	m_view = DirectX::SimpleMath::Matrix::CreateLookAt(eye, pos, up);
	UserResources::GetUserResource()->SetView(&m_view);
}



/// <summary>
/// 更新
/// </summary>
/// <param name="pPlayer">プレイヤー</param>
/// <param name="upPos">上向きベクトル</param>
/// <param name="field">フィールド</param>
void Camera::Update(Player* pPlayer, DirectX::SimpleMath::Vector3 upPos, DirectX::SimpleMath::Vector3 field)
{
	// プレイヤー位置
	DirectX::SimpleMath::Vector3 playerPos = pPlayer->GetPosition();

	DirectX::SimpleMath::Vector3 eye = pPlayer->GetPosition() * 3;

	// 世界Y軸
	m_up = upPos + field;
	m_up.Normalize();
	m_eye = eye;
	m_target = playerPos;

	// ビュー行列更新
	m_view = DirectX::SimpleMath::Matrix::CreateLookAt(eye, pPlayer->GetPosition(), m_up);
	UserResources::GetUserResource()->SetView(&m_view);
}



/// <summary>
/// デバック用
/// </summary>
void Camera::DebugMode()
{
	auto state = DirectX::Mouse::Get().GetState();

	// 相対モードなら何もしない
	if (state.positionMode == DirectX::Mouse::MODE_RELATIVE) return;

	m_tracker.Update(state);

	// マウスの左ボタンが押された
	if (m_tracker.leftButton == DirectX::Mouse::ButtonStateTracker::ButtonState::PRESSED)
	{
		// マウスの座標を取得
		m_x = state.x;
		m_y = state.y;
	}
	else if (m_tracker.leftButton == DirectX::Mouse::ButtonStateTracker::ButtonState::RELEASED)
	{
		// 現在の回転を保存
		m_xAngle = m_xTmp;
		m_yAngle = m_yTmp;
	}
	// マウスのボタンが押されていたらカメラを移動させる
	if (state.leftButton)
	{
		Motion(state.x, state.y);
	}

	// マウスのフォイール値を取得
	m_scrollWheelValue = state.scrollWheelValue;
	if (m_scrollWheelValue > 0)
	{
		m_scrollWheelValue = 0;
		DirectX::Mouse::Get().ResetScrollWheelValue();
	}

	// ビュー行列を算出する
	DirectX::SimpleMath::Matrix rotY = DirectX::SimpleMath::Matrix::CreateRotationY(m_yTmp);
	DirectX::SimpleMath::Matrix rotX = DirectX::SimpleMath::Matrix::CreateRotationX(m_xTmp);

	DirectX::SimpleMath::Matrix rt = rotY * rotX;

	DirectX::SimpleMath::Vector3 eye(0.0f, 1.0f, 1.0f);
	DirectX::SimpleMath::Vector3 target(0.0f, 0.0f, 0.0f);
	DirectX::SimpleMath::Vector3 up(0.0f, 1.0f, 0.0f);

	eye = DirectX::SimpleMath::Vector3::Transform(eye, rt.Invert());
	eye *= (DEFAULT_CAMERA_DISTANCE - m_scrollWheelValue / 100);
	up = DirectX::SimpleMath::Vector3::Transform(up, rt.Invert());

	m_eye = eye;
	m_target = target;
	m_up = up;

	m_view = DirectX::SimpleMath::Matrix::CreateLookAt(eye, target, up);
	UserResources::GetUserResource()->SetView(&m_view);
}



/// <summary>
/// モーション
/// </summary>
/// <param name="x">X座標</param>
/// <param name="y">Y座標</param>
void Camera::Motion(int x, int y)
{
	// マウスポインタの位置のドラッグ開始位置からの変位 (相対値)
	float dx = (x - m_x) * m_sx;
	float dy = (y - m_y) * m_sy;

	if (dx != 0.0f || dy != 0.0f)
	{
		// Ｙ軸の回転
		float yAngle = dx * DirectX::XM_PI;
		// Ｘ軸の回転
		float xAngle = dy * DirectX::XM_PI;

		m_xTmp = m_xAngle + xAngle;
		m_yTmp = m_yAngle + yAngle;
	}
}



/// <summary>
/// 画面サイズの設定
/// </summary>
/// <param name="windowWidth">ウィンドウサイズ(幅)</param>
/// <param name="windowHeight">ウィンドウサイズ(高さ)</param>
void Camera::SetWindowSize(int windowWidth, int windowHeight)
{
	// 画面サイズに対する相対的なスケールに調整
	m_sx = 1.0f / float(windowWidth);
	m_sy = 1.0f / float(windowHeight);
}




/// <summary>
/// ウィンドウサイズの取得
/// </summary>
/// <param name="windowWidth">ウィンドウサイズ(幅)</param>
/// <param name="windowHeight">ウィンドウサイズ(高さ)</param>
void Camera::GetWindowSize(int & windowWidth, int & windowHeight)
{
	windowWidth = m_screenW;
	windowHeight = m_screenH;
}
