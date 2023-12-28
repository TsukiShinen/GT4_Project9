#pragma once

#include "DirectXContext.h"

namespace Engine
{
	class DirectXCamera
	{

	public:
		DirectXCamera(float width, float height, float fovDegree = 45.0f, float nearZ = 0.1f, float farZ = 1000.0f);
		~DirectXCamera();

		void Resize(float width, float height);

		void Update();

	private:
		PassConstants m_MainPassCB;

		float m_FovDegree;
		float m_NearZ;
		float m_FarZ;

		DirectX::XMFLOAT3 m_EyePosition = { 0.0f, 0.0f, 0.0f };

		DirectX::XMFLOAT4X4 m_View = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 m_Proj = MathHelper::Identity4x4();

		DirectX::XMFLOAT4X4 m_ViewProj = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 m_ViewProjT = MathHelper::Identity4x4();

		friend class DirectXApi;
	};
}


