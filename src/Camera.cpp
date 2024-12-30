#include "Camera.h"
#include <DirectXMath.h>

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"

using namespace DirectX;

Camera::Camera()
{
	SetLen(45.0, 780.0 / 1280.0, 0.1f, 1000.0f);
}

Camera::~Camera()
{
}

void Camera::SetPosition(float x, float y, float z)
{
	mPosition = XMFLOAT3(x, y, z);
	viewDirty = true;
}

void Camera::SetLen(float fov, float aspectRatio, float nearZ, float farZ)
{
	mFovY = fov;
	mAspect = aspectRatio;
	mNearZ = nearZ;
	mFarZ = farZ;

	UpdateProjMat();
}

void Camera::UpdateViewMat()
{
	if (viewDirty)
	{
		XMVECTOR R = XMLoadFloat3(&mRight);
		XMVECTOR U = XMLoadFloat3(&mUp);
		XMVECTOR L = XMLoadFloat3(&mLook);
		XMVECTOR P = XMLoadFloat3(&mPosition);

		// keep camera's axes orthogonal to each other and of unit length
		L = XMVector3Normalize(L);
		U = XMVector3Normalize(XMVector3Cross(L, R));

		R = XMVector3Cross(U, L);

		float x = -XMVectorGetX(XMVector3Dot(P, R));
		float y = -XMVectorGetX(XMVector3Dot(P, U));
		float z = -XMVectorGetX(XMVector3Dot(P, L));
		
		XMStoreFloat3(&mRight, R);
		XMStoreFloat3(&mUp, U);
		XMStoreFloat3(&mLook, L);

		m_ViewMat = XMMATRIX(
			R,
			U,
			L,
			XMVectorSet(x, y, z, 1.0f)
		);
	}

	viewDirty = false;
}

void Camera::Pitch(float degrees)
{
	Rotate(XMLoadFloat3(&mRight), degrees);
}

void Camera::Roll(float degrees)
{
	Rotate(XMLoadFloat3(&mLook), degrees);
}

void Camera::Yaw(float degrees)
{
	Rotate(XMLoadFloat3(&mUp), -degrees);
}

void Camera::Rotate(XMVECTOR axis, float degrees)
{
	XMMATRIX R = XMMatrixRotationAxis(axis, XMConvertToRadians(degrees));

	auto Look = XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&mLook), R));
	auto Up = XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&mUp), R));
	auto Right = XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&mRight), R));

	XMStoreFloat3(&mRight, Right);
	XMStoreFloat3(&mLook, Look);
	XMStoreFloat3(&mUp, Up);

	viewDirty = true;
}

void Camera::UpdateProjMat()
{
	//m_ProjMat = XMMatrixPerspectiveFovLH(m_VerticalFOV, m_AspectRatio, m_NearClip, m_FarClip);

	float fovRadians = XMConvertToRadians(mFovY);
	float height = mNearZ * tan(fovRadians / 2.0);
	float width = height * mAspect;

	XMVECTOR m0 = XMVectorSet(mNearZ / width, 0, 0, 0);
	XMVECTOR m1 = XMVectorSet(0, mNearZ / height, 0, 0);
	XMVECTOR m2 = XMVectorSet(0, 0, mFarZ / (mFarZ - mNearZ), 1);
	XMVECTOR m3 = XMVectorSet(0, 0, -(mFarZ * mNearZ) / (mFarZ - mNearZ), 0);

	m_ProjMat.r[0] = m0;
	m_ProjMat.r[1] = m1;
	m_ProjMat.r[2] = m2;
	m_ProjMat.r[3] = m3;
}

void Camera::SetEyeAtUp(DirectX::XMVECTOR eye, DirectX::XMVECTOR at, DirectX::XMVECTOR up)
{
	//m_ViewMat = XMMatrixLookAtLH(eye, at, up);
}

void Camera::SetLookAt(DirectX::XMVECTOR pos, DirectX::XMVECTOR target, DirectX::XMVECTOR vup)
{
	// based on left hand coordinate
	// lookforward

	XMVECTOR lookat = XMVector3Normalize(XMVectorSubtract(target, pos));
	XMVECTOR right = XMVector3Normalize(XMVector3Cross(vup, lookat));
	XMVECTOR up = XMVector3Normalize(XMVector3Cross(lookat, right));

	XMStoreFloat3(&mPosition, pos);
	XMStoreFloat3(&mLook, lookat);
	XMStoreFloat3(&mRight, right);
	XMStoreFloat3(&mUp, up);

	viewDirty = true;
}

void Camera::MoveCamera(float moveForward, float strafe, float vertical)
{
	XMVECTOR position = XMLoadFloat3(&mPosition);
	XMVECTOR lookDirection = XMVector3Normalize(XMLoadFloat3(&mLook));
	XMVECTOR rightDirection = XMVector3Normalize(XMLoadFloat3(&mRight));
	XMVECTOR upDirection = XMVector3Normalize(XMLoadFloat3(&mUp));

	position += lookDirection * moveForward;  // 前进/后退
	position += rightDirection * strafe;     // 左右平移
	position += upDirection * vertical;      // 上下移动

	XMStoreFloat3(&mPosition, position);
}

void Camera::CamerImGui()
{
	const auto dcheck = [](bool d, bool& carry) { carry = carry || d; };

	bool moveDirty = false;
	

	bool projDirty = false;

	
	bool pitchDirty = false;
	bool yawDirty = false;
	bool rollDirty = false;

	ImGui::Text("Camera Orientation");
	dcheck(ImGui::SliderFloat("FOV", &mFovY, 0.1f, 89.0f, "%.1f"), projDirty);
	dcheck(ImGui::SliderFloat("X", &mPosition.x, -50.0f, 50.0f, "%.1f"), moveDirty);
	dcheck(ImGui::SliderFloat("Y", &mPosition.y, -50.0f, 50.0f, "%.1f"), moveDirty);
	dcheck(ImGui::SliderFloat("Z", &mPosition.z, -50.0f, 50.0f, "%.1f"), moveDirty);
	
	dcheck(ImGui::SliderFloat("Pitch", &pitch, -90.0f, 90.0f, "%.1f"), pitchDirty);
	dcheck(ImGui::SliderFloat("Yaw", &yaw, -90.0f, 90.0f, "%.1f"), yawDirty);
	dcheck(ImGui::SliderFloat("Roll", &roll, -180.0f, 180.0f, "%.1f"), rollDirty);


	if (pitchDirty)
	{
		Pitch(pitch - lastPitch);
		lastPitch = pitch;
		pitchDirty = false;
	}
	if (yawDirty)
	{
		Yaw(yaw - lastYaw);
		lastYaw = yaw;
		yawDirty = false;
	}

	if (rollDirty)
	{
		Roll(roll - lastRoll);
		lastRoll = roll;
		rollDirty = false;
	}

	if (moveDirty)
	{
		//MoveCamera(moveForward, moveRight, moveUp);
		moveDirty = false;
		viewDirty = true;
	}

	if (projDirty)
	{
		UpdateProjMat();
		projDirty = false;
	}
}
