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
	DirectX::XMStoreFloat4(&m_quat, 
		DirectX::XMQuaternionRotationRollPitchYawFromVector(
		DirectX::XMLoadFloat3(&rotation)));

	UpdateMatrix();
}

void Engine::Transform::Rotate(float yaw, float pitch, float roll)
{
	DirectX::XMVECTOR quat = DirectX::XMQuaternionRotationRollPitchYaw(pitch, yaw, roll);
	DirectX::XMVECTOR newQuat = DirectX::XMQuaternionMultiply(DirectX::XMLoadFloat4(&m_quat), quat);
	DirectX::XMStoreFloat4(&m_quat, newQuat);

	UpdateMatrix();
}


void Engine::Transform::Translate(float offsetx, float offsety, float offsetz)
{
	DirectX::XMVECTOR translation = DirectX::XMVectorSet(offsetx, offsety, offsetz, 0.f);

	DirectX::XMVECTOR newpos = DirectX::XMVectorAdd(GetPosition(), translation);

	DirectX::XMStoreFloat3(&m_position, newpos);

	UpdateMatrix();
}

void Engine::Transform::SetPosition(DirectX::XMFLOAT3 position)
{
	m_position = position;
	UpdateMatrix();
}

void Engine::Transform::SetScale(DirectX::XMFLOAT3 scale)
{
	m_scale = scale;
	UpdateMatrix();
}

DirectX::XMVECTOR Engine::Transform::GetPosition() const
{
	return DirectX::XMLoadFloat3(&m_position);
}

DirectX::XMVECTOR Engine::Transform::GetScale() const
{
	return DirectX::XMLoadFloat3(&m_scale);
}

DirectX::XMVECTOR Engine::Transform::GetUpVector() const
{
	return DirectX::XMLoadFloat3(&m_up);
}

DirectX::XMVECTOR Engine::Transform::GetRightVector() const
{
	return DirectX::XMLoadFloat3(&m_right);
}

DirectX::XMVECTOR Engine::Transform::GetForwardVector() const
{
	return DirectX::XMLoadFloat3(&m_forward);
}

void Engine::Transform::UpdateMatrix()
{
	DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScalingFromVector(DirectX::XMLoadFloat3(&m_scale));
	DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&m_quat));
	DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&m_position));
	DirectX::XMMATRIX world = scaleMatrix * rotationMatrix * translationMatrix; 

	DirectX::XMStoreFloat4x4(&m_world, world);
	
	DirectX::XMVECTOR forwardVector = DirectX::XMVector3Rotate(DirectX::XMVectorSet(0.f,0.f,1.f,0.f), DirectX::XMLoadFloat4(&m_quat));
	forwardVector = DirectX::XMVector3Normalize(forwardVector);
	DirectX::XMStoreFloat3(&m_forward, forwardVector);

	DirectX::XMVECTOR rightVector = DirectX::XMVector3Rotate(DirectX::XMVectorSet(1.f, 0.f, 0.f, 0.f), DirectX::XMLoadFloat4(&m_quat));
	rightVector = DirectX::XMVector3Normalize(rightVector);
	DirectX::XMStoreFloat3(&m_right, rightVector);

	DirectX::XMVECTOR upVector = DirectX::XMVector3Rotate(DirectX::XMVectorSet(0.f, 1.f, 0.f, 0.f), DirectX::XMLoadFloat4(&m_quat));
	upVector = DirectX::XMVector3Normalize(upVector);
	DirectX::XMStoreFloat3(&m_up, upVector);
}

DirectX::XMMATRIX Engine::Transform::GetWorld() const 
{
	return DirectX::XMLoadFloat4x4(&m_world);
}
