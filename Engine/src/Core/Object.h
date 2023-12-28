#pragma once
#include "Core/Transform.h"
#include "Renderer/DirectXMesh.h"

namespace Engine {

	class MeshRenderer;

	/// <summary>
	/// Super basic Object class to link a DirectX's mesh to a Transform
	/// </summary>
	class Object
	{
	public:

		Object(DirectX::XMFLOAT3 position, DirectXMesh* mesh, DirectXMaterial* material = nullptr);

		~Object();

		/// <summary>
		/// Call this in between BeginFrame() and EndFrame() to draw the Object's mesh.
		/// </summary>
		void Render();

		/// <summary>
		/// Call this before BeginFrame() to manipulate the object (ex. its Transform).
		/// </summary>
		/// <param name="dt"> : delta time</param>
		void GameUpdate(float dt);

		/// <returns> The Object's Transform. </returns>
		Transform* GetTransform();

	private:

		std::unique_ptr<Transform> m_Transform;
		std::unique_ptr<MeshRenderer> m_Renderer;
	};
}


