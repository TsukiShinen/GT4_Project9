#pragma once 
#include <DirectXMath.h>

namespace Engine {

	class Transform {

	public:

		Transform();

		Transform(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 scale);

		void SetRotation(DirectX::XMFLOAT3 rotation);
		void SetPosition(DirectX::XMFLOAT3 position);
		void SetScale(DirectX::XMFLOAT3 scale);

		void Rotate(float yaw, float pitch, float roll);
		void Translate(DirectX::XMFLOAT3 rotation);

		DirectX::XMVECTOR GetPosition() const;
		DirectX::XMVECTOR GetScale() const;
		DirectX::XMMATRIX GetWorld() const;

		void UpdateMatrix();

	private:

		DirectX::XMFLOAT3 m_position = { 0.f, 0.f, 0.f };

		//TODO : update directional vectors 
		DirectX::XMFLOAT3 m_right = { 1.f, 0.f, 0.f };
		DirectX::XMFLOAT3 m_up = { 0.f, 1.f, 0.f };
		DirectX::XMFLOAT3 m_forward = { 0.f, 0.f, 1.f };

		DirectX::XMFLOAT3 m_scale = { 1.f, 1.f, 1.f };

		DirectX::XMFLOAT4 m_quat = { 0.f,0.f,0.f, 1.f };

		DirectX::XMFLOAT4X4 m_world = DirectX::XMFLOAT4X4(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);

		DirectX::XMFLOAT4X4 m_rot = DirectX::XMFLOAT4X4(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);
	};
}
