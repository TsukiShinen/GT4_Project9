#pragma once
#include <DirectXMath.h>

namespace Engine
{
	/// <summary>
	/// A Basic Transform class to manipulates Directx's meshes' position, rotation, and scale.
	/// </summary>
	class Transform
	{
	public:
		Transform(DirectX::XMFLOAT3 position = {0.f, 0.f, 0.f}, DirectX::XMFLOAT3 rotation = {0.f, 0.f, 0.f},
		          DirectX::XMFLOAT3 scale = {1.f, 1.f, 1.f});

		/// <summary>
		/// Sets transform's position in world space.
		/// </summary>
		/// <param name="position"></param>
		void SetPosition(DirectX::XMFLOAT3 position);

		/// <summary>
		/// Set transform's rotation in world space.
		/// </summary>
		/// <param name="rotation"></param>
		void SetRotation(DirectX::XMFLOAT3 rotation);

		/// <summary>
		/// Sets transform's scale.
		/// </summary>
		/// <param name="scale"></param>
		void SetScale(DirectX::XMFLOAT3 scale);

		/// <summary>
		/// Computes a new transform's rotation with the given inputs.
		/// </summary>
		/// <param name="yaw"></param>
		/// <param name="pitch"></param>
		/// <param name="roll"></param>
		void Rotate(float yaw, float pitch, float roll);

		/// <summary>
		/// Computes a new transform's position with the given inputs.
		/// </summary>
		/// <param name="offsetx"></param>
		/// <param name="offsety"></param>
		/// <param name="offsetz"></param>
		void Translate(float offsetx, float offsety, float offsetz);

		/// <summary>
		/// Moves the transform along its local forward axis.
		/// </summary>
		/// <param name="speed"></param>
		/// <param name="direction"> : positive to go forward, negative to go backward. </param>
		void MoveForward(float speed, float direction);

		/// <summary>
		/// Moves the transform along its local right axis.
		/// </summary>
		/// <param name="speed"></param>
		/// <param name="direction"> : positive to go right, negative to go left. </param>
		void MoveRight(float speed, float direction);

		/// <summary>
		/// Moves the transform along its local up axis.
		/// </summary>
		/// <param name="speed"></param>
		/// <param name="direction"> : positive to go up, negative to go down. </param>
		void MoveUp(float speed, float direction);

		/// <summary>
		/// Rotates the transform on the world's XAxis.
		/// </summary>
		/// <param name="angle"></param>
		void RotateWorldX(float angle);

		/// <summary>
		/// Rotates the transform on the world's YAxis.
		/// </summary>
		/// <param name="angle"></param>
		void RotateWorldY(float angle);

		/// <summary>
		/// Rotates the transform on the world's ZAxis.
		/// </summary>
		/// <param name="angle"></param>
		void RotateWorldZ(float angle);

		/// <summary>
		/// Rotates the transform on the local's XAxis.
		/// </summary>
		/// <param name="angle"></param>
		void RotateLocalX(float angle);

		/// <summary>
		/// Rotates the transform on the local's YAxis.
		/// </summary>
		/// <param name="angle"></param>
		void RotateLocalY(float angle);

		/// <summary>
		/// Rotates the transform on the local's ZAxis.
		/// </summary>
		/// <param name="angle"></param>
		void RotateLocalZ(float angle);

		/// GETTERS functions ------------------------

		/// <returns>The tranform's position.</returns>
		DirectX::XMVECTOR GetPosition() const;

		/// <returns>The tranform's scale.</returns>
		DirectX::XMVECTOR GetScale() const;

		/// <returns>The tranform's world matrix.</returns>
		DirectX::XMMATRIX GetWorld() const;

		/// <returns>The tranform's world matrix as XMFLOAT4X4</returns>
		DirectX::XMFLOAT4X4 GetWorldAsFloat4x4() const;

		/// <returns>The tranform's Up vector.</returns>
		DirectX::XMVECTOR GetUpVector() const;

		/// <returns>The tranform's Right vector.</returns>
		DirectX::XMVECTOR GetRightVector() const;

		/// <returns>The tranform's Forward vector.</returns>
		DirectX::XMVECTOR GetForwardVector() const;

		/// GETTERS functions end --------------------

	private:
		/// <summary>
		/// Updates the transform's world matrix.
		/// </summary>
		void UpdateMatrix();

		/// <summary>
		/// Updates the transform's rotation matrix.
		/// </summary>
		void UpdateRotation();

		DirectX::XMFLOAT3 m_Position = {0.f, 0.f, 0.f};

		DirectX::XMFLOAT3 m_Right = {1.f, 0.f, 0.f};
		DirectX::XMFLOAT3 m_Up = {0.f, 1.f, 0.f};
		DirectX::XMFLOAT3 m_Forward = {0.f, 0.f, 1.f};

		DirectX::XMFLOAT3 m_Scale = {1.f, 1.f, 1.f};

		DirectX::XMFLOAT4 m_Quat = {0.f, 0.f, 0.f, 1.f};

		DirectX::XMFLOAT4X4 m_Rot = DirectX::XMFLOAT4X4(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);

		DirectX::XMFLOAT4X4 m_World = DirectX::XMFLOAT4X4(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);
	};
}
