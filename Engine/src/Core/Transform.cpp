#include "Transform.h"
#include "Debug/Log.h"

Engine::Transform::Transform() {}

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

	CORE_DEBUG("%d, %d, %d, %d", m_quat.x, m_quat.y, m_quat.z, m_quat.w);

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

void Engine::Transform::Translate(DirectX::XMFLOAT3 rotation)
{
	
}

DirectX::XMVECTOR Engine::Transform::GetScale() const
{
	return DirectX::XMLoadFloat3(&m_scale);
}

void Engine::Transform::UpdateMatrix()
{
	DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScalingFromVector(DirectX::XMLoadFloat3(&m_scale));
	DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&m_quat));
	DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&m_position));

	DirectX::XMMATRIX world = scaleMatrix * rotationMatrix * translationMatrix; 

	DirectX::XMStoreFloat4x4(&m_world, world);
}

DirectX::XMMATRIX Engine::Transform::GetWorld() const 
{
	return DirectX::XMLoadFloat4x4(&m_world);
}
