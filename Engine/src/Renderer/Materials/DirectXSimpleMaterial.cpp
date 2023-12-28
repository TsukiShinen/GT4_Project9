#include "DirectXSimpleMaterial.h"

#include "Renderer/Shaders/DirectXShader.h"
#include "Renderer/DirectXContext.h"
#include "Renderer/Resource/DirectXResourceManager.h"

namespace Engine
{
	DirectXSimpleMaterial::DirectXSimpleMaterial(DirectXSimpleShader* shader)
		: DirectXMaterial((DirectXShader*)shader)
	{
	}

	void DirectXSimpleMaterial::Bind(DirectXMesh* mesh)
	{
		m_Shader->Bind(mesh);
	}
}