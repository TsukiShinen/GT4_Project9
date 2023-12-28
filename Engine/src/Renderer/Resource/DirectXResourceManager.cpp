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
        THROW_IF_FAILED(DirectXContext::Get()->m_Device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_SrvDescriptorHeap)));

        for (int i = 0; i < m_MaxTextures; ++i)
            m_TextureIndicesAvailable.push(i);
    }

    void DirectXResourceManager::BindDescriptorsHeap() const
    {
        ID3D12DescriptorHeap* descriptorsHeap[] = { m_SrvDescriptorHeap.Get() };
        DirectXContext::Get()->m_CommandObject->GetCommandList()->SetDescriptorHeaps(_countof(descriptorsHeap), descriptorsHeap);
    }

    Texture& DirectXResourceManager::LoadTexture(const std::wstring& pPath, const std::string& pName)
    {
        DirectXContext::Get()->m_CommandObject->GetCommandAllocator()->Reset();
        DirectXContext::Get()->m_CommandObject->ResetList(DirectXContext::Get()->m_CommandObject->GetCommandAllocator());
        
        auto texture = std::make_unique<Texture>();
        texture->Name = pName;
        texture->Filename = pPath;
        texture->HeapIndex = m_TextureIndicesAvailable.front();
        m_TextureIndicesAvailable.pop();
        ++m_TextureCount;
        THROW_IF_FAILED(DirectX::CreateDDSTextureFromFile12(DirectXContext::Get()->m_Device.Get(),
            DirectXContext::Get()->m_CommandObject->GetCommandList().Get(), texture->Filename.c_str(),
            texture->Resource, texture->UploadHeap));
 
        m_Textures[pName] = std::move(texture);

        CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(m_SrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
        hDescriptor.Offset(m_Textures[pName]->HeapIndex, m_CbvSrvDescriptorSize);
 
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Format = m_Textures[pName]->Resource->GetDesc().Format;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MostDetailedMip = 0;
        srvDesc.Texture2D.MipLevels = m_Textures[pName]->Resource->GetDesc().MipLevels;
        srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

        DirectXContext::Get()->m_Device->CreateShaderResourceView(m_Textures[pName]->Resource.Get(), &srvDesc, hDescriptor);
        
        DirectXContext::Get()->m_CommandObject->Execute();
        DirectXContext::Get()->m_CommandObject->Flush();

        m_Textures[pName]->UploadHeap->Release();
        
        return *m_Textures[pName];
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
