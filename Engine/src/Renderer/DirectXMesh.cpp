#include "DirectXMesh.h"

#include "DirectXCommandObject.h"
#include "DirectXContext.h"
#include "Materials/DirectXMaterial.h"
#include "Core/ObjLoader.h"

namespace Engine
{

    std::unique_ptr<Engine::DirectXMesh> DirectXMesh::CreateFromFile(const char* file)
    {
        std::vector<Engine::VertexLit> vertices;
        Engine::ObjLoader::LoadObj(file, &vertices);
        std::vector<uint16_t> indices;
        for (size_t i = 0; i < vertices.size(); i++)
        {
            indices.push_back(i);
        }
        return std::make_unique<Engine::DirectXMesh>(vertices, indices);
    }

    void DirectXMesh::Draw()
    {
        DirectXContext::Get()->m_CommandObject->GetCommandList()->IASetVertexBuffers(0, 1, &m_VertexBuffer);
        DirectXContext::Get()->m_CommandObject->GetCommandList()->IASetIndexBuffer(&m_IndexBuffer);
        DirectXContext::Get()->m_CommandObject->GetCommandList()->IASetPrimitiveTopology(m_PrimitiveType);
        
        DirectXContext::Get()->m_CommandObject->GetCommandList()->DrawIndexedInstanced(m_IndexCount, 1, 0, 0, 0);
    }


}
