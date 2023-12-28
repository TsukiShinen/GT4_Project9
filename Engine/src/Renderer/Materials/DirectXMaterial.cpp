#include "DirectXMaterial.h"

#include "Renderer/Shaders/DirectXShader.h"

namespace Engine
{
	DirectXMaterial::DirectXMaterial(DirectXShader* shader)
		: m_Shader(shader), m_IsDirty(true)
	{
	}
}
