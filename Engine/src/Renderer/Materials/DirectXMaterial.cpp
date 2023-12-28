#include "DirectXMaterial.h"

#include "Renderer/Shaders/DirectXShader.h"

namespace Engine
{
	DirectXMaterial::DirectXMaterial(DirectXShader* shader)
		: m_IsDirty(true), m_Shader(shader)
	{
	}

}