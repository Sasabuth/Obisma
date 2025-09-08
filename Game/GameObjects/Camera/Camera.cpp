/// <summary>
/// Cameraに関するソースファイル
/// </summary>
/// <author>仲森智史</author>

#include "pch.h"
#include "Camera.h"
#include "Mouse.h"
#include "Game/Commons/UserResources.h"
#include "Game/GameObjects/Player/Player.h"


// 名前の省略
using namespace DirectX;



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
	Mouse::Get().ResetScrollWheelValue();
}



/// <summary>
/// 更新
/// </summary>
/// <param name="player">プレイヤー</param>
/// <param name="upPos">上向きベクトル</param>
/// <param name="field">フィールド</param>
void Camera::Update(Player* player, SimpleMath::Vector3 upPos, DirectX::SimpleMath::Vector3 field)
{
	using namespace DirectX::SimpleMath;

	// プレイヤー位置
	SimpleMath::Vector3 playerPos = player->GetPosition();

	// 重力の方向
	Vector3 gravityDir = field - playerPos;
	gravityDir.Normalize();

	// 方向ベクトルの反転
	Vector3 targetUp;
	targetUp = -gravityDir;

	// 現在の姿勢制御
	Vector3 currentUp = Vector3::Transform(Vector3::UnitY, m_rotate);

	// 回転軸の計算
	SimpleMath::Vector3 axis = currentUp.Cross(targetUp);
	axis.Normalize();

	// 回転角の計算
	float dot = currentUp.Dot(targetUp);
	float angle = acosf(dot);

	// クォータニオンの作成
	Quaternion q;


	// 角度が少しでもあれば軸を作る
	if (angle > 0.01f)
	{
		q = Quaternion::CreateFromAxisAngle(axis, angle);
	}
	// なければ何もしない
	else
	{
		q = Quaternion::Identity;
	}

	// 回転の設定
	m_rotate *= q;

	SimpleMath::Vector3 forward = SimpleMath::Vector3::Transform(SimpleMath::Vector3(0.0f, 0.0f, 1.0f), m_rotate);
	SimpleMath::Vector3 vertical = SimpleMath::Vector3::Transform(SimpleMath::Vector3(0.0f, 1.0f, 0.0f), m_rotate);

	SimpleMath::Vector3 eye = player->GetPosition() * 3;

	// 世界Y軸
	SimpleMath::Vector3 up = upPos + field;
	up.Normalize();

	// ビュー行列更新
	m_eye = eye;
	m_target = playerPos;
	m_view = SimpleMath::Matrix::CreateLookAt(eye, player->GetPosition(), up);
	UserResources::GetUserResource()->SetView(&m_view);
}



/// <summary>
/// デバック用
/// </summary>
void Camera::DebugMode()
{
	auto state = Mouse::Get().GetState();

	// 相対モードなら何もしない
	if (state.positionMode == Mouse::MODE_RELATIVE) return;

	m_tracker.Update(state);

	// マウスの左ボタンが押された
	if (m_tracker.leftButton == Mouse::ButtonStateTracker::ButtonState::PRESSED)
	{
		// マウスの座標を取得
		m_x = state.x;
		m_y = state.y;
	}
	else if (m_tracker.leftButton == Mouse::ButtonStateTracker::ButtonState::RELEASED)
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
		Mouse::Get().ResetScrollWheelValue();
	}

	// ビュー行列を算出する
	SimpleMath::Matrix rotY = SimpleMath::Matrix::CreateRotationY(m_yTmp);
	SimpleMath::Matrix rotX = SimpleMath::Matrix::CreateRotationX(m_xTmp);

	SimpleMath::Matrix rt = rotY * rotX;

	SimpleMath::Vector3 eye(0.0f, 1.0f, 1.0f);
	SimpleMath::Vector3 target(0.0f, 0.0f, 0.0f);
	SimpleMath::Vector3 up(0.0f, 1.0f, 0.0f);

	eye = SimpleMath::Vector3::Transform(eye, rt.Invert());
	eye *= (DEFAULT_CAMERA_DISTANCE - m_scrollWheelValue / 100);
	up = SimpleMath::Vector3::Transform(up, rt.Invert());

	m_eye = eye;
	m_target = target;

	m_view = SimpleMath::Matrix::CreateLookAt(eye, target, up);
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
		float yAngle = dx * XM_PI;
		// Ｘ軸の回転
		float xAngle = dy * XM_PI;

		m_xTmp = m_xAngle + xAngle;
		m_yTmp = m_yAngle + yAngle;
	}
}



/// <summary>
/// カメラのビュー行列の取得
/// </summary>
/// <returns>ビュー行列</returns>
DirectX::SimpleMath::Matrix Camera::GetCameraMatrix()
{
	return m_view;
}



/// <summary>
/// カメラの座標の取得
/// </summary>
/// <returns>視点の位置</returns>
DirectX::SimpleMath::Vector3 Camera::GetEyePosition()
{
	return m_eye;
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
