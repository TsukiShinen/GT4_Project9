#include "DirectXMesh.h"

#include "DirectXCommandObject.h"
#include "DirectXContext.h"
#include "Shaders/DirectXShader.h"

namespace Engine
{
    void DirectXMesh::Draw()
    {
        if(m_NumFramesDirty > 0)
        {
            const DirectX::XMMATRIX world = XMLoadFloat4x4(&m_TransformMatrix);

            ObjectConstants objConstants;
            XMStoreFloat4x4(&objConstants.World, XMMatrixTranspose(world));

            m_ConstantBuffer->CopyData(0, objConstants);

            m_NumFramesDirty--;
        }

        m_Shader->Bind(this);
        
        DirectXContext::Get()->m_CommandObject->GetCommandList()->IASetVertexBuffers(0, 1, &m_VertexBuffer);
        DirectXContext::Get()->m_CommandObject->GetCommandList()->IASetIndexBuffer(&m_IndexBuffer);
        DirectXContext::Get()->m_CommandObject->GetCommandList()->IASetPrimitiveTopology(m_PrimitiveType);
        
        DirectXContext::Get()->m_CommandObject->GetCommandList()->DrawIndexedInstanced(m_IndexCount, 1, 0, 0, 0);
    }
}
