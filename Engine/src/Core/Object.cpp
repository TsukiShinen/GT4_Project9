#include "Object.h"

Engine::Object::Object(DirectXMesh* mesh, DirectX::XMFLOAT3 position) : m_Mesh(mesh)
{
	m_Transform = std::make_unique<Transform>(position);
}

Engine::Object::~Object()
{

}

void Engine::Object::Render()
{
	m_Mesh->SetTransformMatrix(m_Transform->GetWorldAsFloat4x4());
	m_Mesh->Draw();
}

void Engine::Object::GameUpdate(float dt)
{

}

Engine::Transform* Engine::Object::GetTransform()
{
	return m_Transform.get();
}
