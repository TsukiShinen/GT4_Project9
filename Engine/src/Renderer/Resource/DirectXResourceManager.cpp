#include "DirectXResourceManager.h"

#include "Renderer/DDSTextureLoader.h"
#include "Renderer/DirectXCommandObject.h"
#include "Renderer/DirectXContext.h"

namespace Engine
{
    DirectXResourceManager::DirectXResourceManager(const uint32_t pMaxTextures)
        : m_MaxTextures(pMaxTextures)
    {
        m_CbvSrvDescriptorSize = DirectXContext::Get()->m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        
        D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
        srvHeapDesc.NumDescriptors = m_MaxTextures;
        srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        if (FAILED(DirectXContext::Get()->m_Device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_SrvDescriptorHeap))))
            throw std::runtime_error("Failed to create SRV descriptor heap.");

        for (int i = 0; i < m_MaxTextures; ++i)
            m_TextureIndicesAvailable.push(i);
    }

    Texture& DirectXResourceManager::LoadTexture(const std::wstring& pPath, const std::string& pName)
    {
        auto texture = std::make_unique<Texture>();
        texture->Name = pName;
        texture->Filename = pPath;
        texture->HeapIndex = m_TextureIndicesAvailable.front();
        m_TextureIndicesAvailable.pop();
        ++m_TextureCount;
        if (FAILED(DirectX::CreateDDSTextureFromFile12(DirectXContext::Get()->m_Device.Get(),
            DirectXContext::Get()->m_CommandObject->GetCommandList().Get(), texture->Filename.c_str(),
            texture->Resource, texture->UploadHeap)))
            throw std::runtime_error("Failed to load texture.");
 
        m_Textures[texture->Name] = std::move(texture);
        return *m_Textures[texture->Name];
    }

    CD3DX12_GPU_DESCRIPTOR_HANDLE DirectXResourceManager::GetTextureHandle(const std::string& pName)
    {
        return GetTextureHandle(*m_Textures[pName]);
    }

    CD3DX12_GPU_DESCRIPTOR_HANDLE DirectXResourceManager::GetTextureHandle(const Texture& pTexture) const
    {
        CD3DX12_GPU_DESCRIPTOR_HANDLE handle(m_SrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
        handle.Offset(pTexture.HeapIndex, m_CbvSrvDescriptorSize);
        return handle;
    }

    void DirectXResourceManager::ReleaseTexture(const std::string& pName)
    {
        // TODO : Release Texture
    }
}
