#pragma once

namespace Engine
{
	class DirectXShader;
	class DirectXMesh;

	class DirectXMaterial
	{
	public:
		DirectXMaterial(DirectXShader* shader);

		virtual void Bind(DirectXMesh* mesh) = 0;

	protected:
		DirectXShader* m_Shader;
		bool m_IsDirty;
	};
}


