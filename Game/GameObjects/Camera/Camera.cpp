//--------------------------------------------------------------------------------------
// File: Camera.cpp
//
// デバッグ用カメラクラス
//
// Date: 2018.4.15
// Author: Hideyasu Imase
//--------------------------------------------------------------------------------------
#include "pch.h"
#include "Camera.h"
#include "Mouse.h"
#include "Game/Commons/UserResources.h"

using namespace DirectX;
const float Camera::DEFAULT_CAMERA_DISTANCE = 5.0f;

//--------------------------------------------------------------------------------------
// コンストラクタ
//--------------------------------------------------------------------------------------
Camera::Camera(int windowWidth, int windowHeight)
	: m_yAngle(0.0f), m_yTmp(0.0f), m_xAngle(0.0f), m_xTmp(0.0f), m_x(0), m_y(0), m_scrollWheelValue(0), m_screenW(windowWidth), m_screenH(windowHeight)
{
	SetWindowSize(windowWidth, windowHeight);

	// マウスのフォイール値をリセット
	Mouse::Get().ResetScrollWheelValue();
}

//--------------------------------------------------------------------------------------
// 更新
//--------------------------------------------------------------------------------------
void Camera::Update(DirectX::SimpleMath::Vector3 viewtarget, float rotateY)
{
	// ビュー行列を算出する
	SimpleMath::Matrix rotY = SimpleMath::Matrix::CreateRotationY(XMConvertToRadians(rotateY + 180.0f));

	SimpleMath::Matrix rot = rotY;

	// 目の位置
	SimpleMath::Vector3 eye = SimpleMath::Vector3(3.0f, 5.0f, 1.0f);

	// 目とプレイヤーの差を求める
	SimpleMath::Vector3 offset = SimpleMath::Vector3::Transform(eye - viewtarget, rot);

	// プレイヤーが回転したら目を回転させ差を埋めずに回転だけするようにする
	SimpleMath::Vector3 up(0.0f, 10.0f, 0.0f);

	// カメラの位置を計算
	eye *= offset;

	// 代入
	m_eye = eye;
	m_target = viewtarget;

	// ビュー行列を更新
	m_view = SimpleMath::Matrix::CreateLookAt(eye, viewtarget, up);
}

void Camera::Update(DirectX::SimpleMath::Vector3 viewtarget, float rotateX, float rotateY)
{
	// ビュー行列を算出する
	SimpleMath::Matrix rot = SimpleMath::Matrix::CreateRotationZ(XMConvertToRadians(-rotateX)) * SimpleMath::Matrix::CreateRotationY(XMConvertToRadians(rotateY + 180.0f));

	// 目の位置
	SimpleMath::Vector3 eye = SimpleMath::Vector3(6.0f, 2.0f, 0.0f);

	// プレイヤーが回転したら目を回転させ差を埋めずに回転だけするようにする
	SimpleMath::Vector3 offset = SimpleMath::Vector3::Transform(eye, rot);

	// カメラの位置を計算
	eye = viewtarget + offset;

	// 代入
	m_eye = eye;
	m_target = viewtarget;

	// ビュー行列を更新
	m_view = SimpleMath::Matrix::CreateLookAt(eye, viewtarget, SimpleMath::Vector3::UnitY);
}

void Camera::Update(DirectX::SimpleMath::Vector3 viewtarget, DirectX::SimpleMath::Vector3 field, DirectX::SimpleMath::Quaternion rotate)
{
	// 目の位置
	SimpleMath::Vector3 currentUp = SimpleMath::Vector3::Transform(SimpleMath::Vector3::UnitY, rotate);
	viewtarget.Normalize();

	SimpleMath::Vector3 eye{ viewtarget * 10 };
	/*eye = -(eye - viewtarget)*3;*/

	////SimpleMath::Vector3 eye = SimpleMath::Vector3(viewtarget);

	//// プレイヤーが回転したら目を回転させ差を埋めずに回転だけするようにする
	//SimpleMath::Vector3 offset = viewtarget;

	//// カメラの位置を計算
	//// eye += offset;
	
	// 代入
	m_eye = eye;
	m_target = viewtarget;

	// up を再計算（常に直交）
	SimpleMath::Vector3 up = SimpleMath::Vector3::UnitY;

	// ビュー行列を更新
	m_view = SimpleMath::Matrix::CreateLookAt(eye, viewtarget, up);
}

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
}

//--------------------------------------------------------------------------------------
// 行列の生成
//--------------------------------------------------------------------------------------
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

DirectX::SimpleMath::Matrix Camera::GetCameraMatrix()
{
	return m_view;
}

DirectX::SimpleMath::Vector3 Camera::GetEyePosition()
{
	return m_eye;
}

DirectX::SimpleMath::Vector3 Camera::GetTargetPosition()
{
	return m_target;
}

void Camera::SetWindowSize(int windowWidth, int windowHeight)
{
	// 画面サイズに対する相対的なスケールに調整
	m_sx = 1.0f / float(windowWidth);
	m_sy = 1.0f / float(windowHeight);
}

void Camera::GetWindowSize(int & windowWidth, int & windowHeight)
{
	windowWidth = m_screenW;
	windowHeight = m_screenH;
}
