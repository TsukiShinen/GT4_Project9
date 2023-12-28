#include "DirectXMesh.h"

#include "DirectXCommandObject.h"
#include "DirectXContext.h"
#include "Materials/DirectXMaterial.h"

namespace Engine
{
    void DirectXMesh::Draw()
    {
        DirectXContext::Get()->m_CommandObject->GetCommandList()->IASetVertexBuffers(0, 1, &m_VertexBuffer);
        DirectXContext::Get()->m_CommandObject->GetCommandList()->IASetIndexBuffer(&m_IndexBuffer);
        DirectXContext::Get()->m_CommandObject->GetCommandList()->IASetPrimitiveTopology(m_PrimitiveType);
        
        DirectXContext::Get()->m_CommandObject->GetCommandList()->DrawIndexedInstanced(m_IndexCount, 1, 0, 0, 0);
    }
}
