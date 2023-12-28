#include "Transform.h"
#include "Debug/Log.h"

Engine::Transform::Transform(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 scale)
{
	SetPosition(position);
	SetScale(scale);
	SetRotation(rotation);
}

void Engine::Transform::SetRotation(DirectX::XMFLOAT3 rotation)
{	
	DirectX::XMStoreFloat4(&m_Quat, 
		DirectX::XMQuaternionRotationRollPitchYawFromVector(
		DirectX::XMLoadFloat3(&rotation)));

	UpdateMatrix();
}

void Engine::Transform::Rotate(float yaw, float pitch, float roll)
{
	DirectX::XMVECTOR quat = DirectX::XMQuaternionRotationRollPitchYaw(pitch, yaw, roll);
	DirectX::XMVECTOR newQuat = DirectX::XMQuaternionMultiply(DirectX::XMLoadFloat4(&m_Quat), quat);
	DirectX::XMStoreFloat4(&m_Quat, newQuat);

	UpdateRotation();
	UpdateMatrix();
}


void Engine::Transform::Translate(float offsetx, float offsety, float offsetz)
{
	DirectX::XMVECTOR translation = DirectX::XMVectorSet(offsetx, offsety, offsetz, 0.f);
	DirectX::XMVECTOR newpos = DirectX::XMVectorAdd(GetPosition(), translation);
	DirectX::XMStoreFloat3(&m_Position, newpos);

	UpdateMatrix();
}

void Engine::Transform::MoveForward(float speed, float direction)
{
	DirectX::XMVECTOR forward = GetForwardVector();
	DirectX::XMVECTOR s = DirectX::XMVectorReplicate(speed * direction);
	DirectX::XMVECTOR position = GetPosition();
	DirectX::XMVECTOR newPos = DirectX::XMVectorMultiplyAdd(s, forward, position);
	DirectX::XMStoreFloat3(&m_Position, newPos);
	UpdateMatrix();
}

void Engine::Transform::MoveRight(float speed, float direction)
{
	DirectX::XMVECTOR right = GetRightVector();
	DirectX::XMVECTOR s = DirectX::XMVectorReplicate(speed * direction);
	DirectX::XMVECTOR position = GetPosition();
	DirectX::XMVECTOR newPos = DirectX::XMVectorMultiplyAdd(s, right, position);
	DirectX::XMStoreFloat3(&m_Position, newPos);
	UpdateMatrix();
}

void Engine::Transform::MoveUp(float speed, float direction)
{
	DirectX::XMVECTOR up = GetUpVector();
	DirectX::XMVECTOR s = DirectX::XMVectorReplicate(speed * direction);
	DirectX::XMVECTOR position = GetPosition();
	DirectX::XMVECTOR newPos = DirectX::XMVectorMultiplyAdd(s, up, position);
	DirectX::XMStoreFloat3(&m_Position, newPos);
	UpdateMatrix();
}

void Engine::Transform::RotateWorldX(float angle)
{
	DirectX::XMVECTOR rotationQuaternion = DirectX::XMQuaternionRotationRollPitchYaw(angle, 0.f , 0.0f);
	DirectX::XMVECTOR newQuat = DirectX::XMQuaternionMultiply(DirectX::XMLoadFloat4(&m_Quat), rotationQuaternion);
	DirectX::XMStoreFloat4(&m_Quat, newQuat);

	UpdateRotation();
	UpdateMatrix();
}

void Engine::Transform::RotateWorldY(float angle)
{
	DirectX::XMVECTOR rotationQuaternion = DirectX::XMQuaternionRotationRollPitchYaw(0.0f, angle, 0.0f);
	DirectX::XMVECTOR newQuat = DirectX::XMQuaternionMultiply(DirectX::XMLoadFloat4(&m_Quat), rotationQuaternion);
	DirectX::XMStoreFloat4(&m_Quat, newQuat);

	UpdateRotation();
	UpdateMatrix();
}

void Engine::Transform::RotateWorldZ(float angle)
{
	DirectX::XMVECTOR rotationQuaternion = DirectX::XMQuaternionRotationRollPitchYaw(0.0f, 0.f, angle);
	DirectX::XMVECTOR newQuat = DirectX::XMQuaternionMultiply(DirectX::XMLoadFloat4(&m_Quat), rotationQuaternion);
	DirectX::XMStoreFloat4(&m_Quat, newQuat);

	UpdateRotation();
	UpdateMatrix();
}

void Engine::Transform::RotateLocalX(float angle)
{
	DirectX::XMVECTOR rotationQuaternion = DirectX::XMQuaternionRotationAxis(XMLoadFloat3(&m_Right), angle);

	DirectX::XMVECTOR newQuat = DirectX::XMQuaternionMultiply(DirectX::XMLoadFloat4(&m_Quat), rotationQuaternion);

	DirectX::XMStoreFloat4(&m_Quat, newQuat);

	UpdateRotation();
	UpdateMatrix();
}

void Engine::Transform::RotateLocalY(float angle)
{
	DirectX::XMVECTOR rotationQuaternion = DirectX::XMQuaternionRotationAxis(XMLoadFloat3(&m_Up), angle);
	DirectX::XMVECTOR newQuat = DirectX::XMQuaternionMultiply(DirectX::XMLoadFloat4(&m_Quat), rotationQuaternion);
	DirectX::XMStoreFloat4(&m_Quat, newQuat);

	UpdateRotation();
	UpdateMatrix();
}

void Engine::Transform::RotateLocalZ(float angle)
{
	DirectX::XMVECTOR rotationQuaternion = DirectX::XMQuaternionRotationAxis(XMLoadFloat3(&m_Forward), angle);
	DirectX::XMVECTOR newQuat = DirectX::XMQuaternionMultiply(DirectX::XMLoadFloat4(&m_Quat), rotationQuaternion);
	DirectX::XMStoreFloat4(&m_Quat, newQuat);

	UpdateRotation();
	UpdateMatrix();
}

void Engine::Transform::SetPosition(DirectX::XMFLOAT3 position)
{
	m_Position = position;
	UpdateMatrix();
}

void Engine::Transform::SetScale(DirectX::XMFLOAT3 scale)
{
	m_Scale = scale;
	UpdateMatrix();
}

DirectX::XMVECTOR Engine::Transform::GetPosition() const
{
	return DirectX::XMLoadFloat3(&m_Position);
}

DirectX::XMVECTOR Engine::Transform::GetScale() const
{
	return DirectX::XMLoadFloat3(&m_Scale);
}

DirectX::XMVECTOR Engine::Transform::GetUpVector() const
{
	return DirectX::XMLoadFloat3(&m_Up);
}

DirectX::XMVECTOR Engine::Transform::GetRightVector() const
{
	return DirectX::XMLoadFloat3(&m_Right);
}

DirectX::XMVECTOR Engine::Transform::GetForwardVector() const
{
	return DirectX::XMLoadFloat3(&m_Forward);
}

void Engine::Transform::UpdateMatrix()
{
	DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScalingFromVector(DirectX::XMLoadFloat3(&m_Scale));
	DirectX::XMMATRIX rotationMatrix = DirectX::XMLoadFloat4x4(&m_Rot);
	DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&m_Position));
	DirectX::XMMATRIX world = scaleMatrix * rotationMatrix * translationMatrix; 

	DirectX::XMStoreFloat4x4(&m_World, world);
}

void Engine::Transform::UpdateRotation()
{
	DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&m_Quat));
	DirectX::XMStoreFloat4x4(&m_Rot, rotationMatrix);

	/// Updates the directional axis with the rotation matrix
	// Forward vector 
	DirectX::XMVECTOR forwardVector = DirectX::XMVector3Rotate(DirectX::XMVectorSet(0.f, 0.f, 1.f, 0.f), DirectX::XMLoadFloat4(&m_Quat));
	forwardVector = DirectX::XMVector3Normalize(forwardVector);
	DirectX::XMStoreFloat3(&m_Forward, forwardVector);

	// Right vector
	DirectX::XMVECTOR rightVector = DirectX::XMVector3Rotate(DirectX::XMVectorSet(1.f, 0.f, 0.f, 0.f), DirectX::XMLoadFloat4(&m_Quat));
	rightVector = DirectX::XMVector3Normalize(rightVector);
	DirectX::XMStoreFloat3(&m_Right, rightVector);

	// Up vector
	DirectX::XMVECTOR upVector = DirectX::XMVector3Rotate(DirectX::XMVectorSet(0.f, 1.f, 0.f, 0.f), DirectX::XMLoadFloat4(&m_Quat));
	upVector = DirectX::XMVector3Normalize(upVector);
	DirectX::XMStoreFloat3(&m_Up, upVector);
	/// -----------------------------------------------------
}

DirectX::XMMATRIX Engine::Transform::GetWorld() const 
{
	return DirectX::XMLoadFloat4x4(&m_World);
}
