#pragma once

#include "Core/MeshRenderer.h"

namespace Engine
{
	class DirectXShader;
	class DirectXMesh;

	class DirectXMaterial
	{
	public:
		DirectXMaterial(DirectXShader* shader);

		virtual void Bind(const UploadBuffer<ObjectConstants>& objectConstantBuffer) = 0;

	protected:
		DirectXShader* m_Shader;
		bool m_IsDirty;
	};
}


