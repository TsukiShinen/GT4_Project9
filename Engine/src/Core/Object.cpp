#include "Object.h"

#include "MeshRenderer.h"

Engine::Object::Object(DirectX::XMFLOAT3 position, DirectXMesh* mesh, DirectXMaterial* material)
{
	m_Transform = std::make_unique<Transform>(position);
	m_Renderer = std::make_unique<MeshRenderer>(mesh, material);
}

Engine::Object::~Object()
{

}

void Engine::Object::Render()
{
	m_Renderer->Draw(m_Transform->GetWorldAsFloat4x4());
}

void Engine::Object::GameUpdate(float dt)
{

}

Engine::Transform* Engine::Object::GetTransform()
{
	return m_Transform.get();
}
