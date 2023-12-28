#pragma once

#include "DirectXContext.h"
#include "Core/Transform.h"
#include "Platform/Input.h"
#include "Events/MouseEvent.h"
#include "Debug/Log.h"

namespace Engine
{
	class DirectXCamera
	{

	public:
		DirectXCamera(float width, float height, float fovDegree = 45.0f, float nearZ = 0.1f, float farZ = 1000.0f);
		~DirectXCamera();

		/// <summary>
		/// Resizes the camera's projection matrix.
		/// </summary>
		/// <param name="width"></param>
		/// <param name="height"></param>
		void Resize(float width, float height);

		/// <summary>
		/// Updates the camera view.
		/// </summary>
		void Update();

		/// <summary>
		/// Game updates.
		/// </summary>
		/// <param name="dt"> : delta time</param>
		void GameUpdate(float dt);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="x"></param>
		/// <param name="y"></param>
		void MouseMove(float x, float y); 	

	private:
		std::unique_ptr<Transform> m_Transform;

		float m_FovDegree;
		float m_NearZ;
		float m_FarZ;

		DirectX::XMFLOAT4X4 m_View = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 m_Proj = MathHelper::Identity4x4();

		DirectX::XMFLOAT4X4 m_ViewProj = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 m_ViewProjT = MathHelper::Identity4x4();

		DirectX::XMFLOAT2 m_LastMousePos = { 0.f,0.f };

		friend class DirectXApi;
	};
}


