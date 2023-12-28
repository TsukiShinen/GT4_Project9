#include "DirectXContext.h"

#include "DirectXCommandObject.h"
#include "DirectXSwapchain.h"
#include "Core/Application.h"
#include "DirectXCamera.h"
#include "Resource/DirectXResourceManager.h"

#include "Shaders/DirectXSimpleShader.h"
#include "Shaders/DirectXTextureShader.h"
#include "Shaders/DirectXLitShader.h"

#include "Materials/DirectXMaterial.h"
#include "Materials/DirectXTextureMaterial.h"
#include "Materials/DirectXLitMaterial.h"

const int gNumFrameResources = 3;

namespace Engine
{
    DirectXContext* DirectXContext::s_Instance = nullptr;

    void DirectXContext::Initialize()
    {
        assert(!s_Instance, "DirectX context already initialized!");
        s_Instance = new DirectXContext();

        // ===== Factory =====
        THROW_IF_FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&s_Instance->m_Factory)));

        // ===== Device =====
        if (const HRESULT hardwareResult = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0,
                                                             IID_PPV_ARGS(&s_Instance->m_Device));
            FAILED(hardwareResult))
        {
            Microsoft::WRL::ComPtr<IDXGIAdapter> warpAdapter;
            THROW_IF_FAILED(s_Instance->m_Factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));
            THROW_IF_FAILED(D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&s_Instance->m_Device)));
        }
        s_Instance->m_CbvSrvUavDescriptorSize = s_Instance->m_Device->GetDescriptorHandleIncrementSize(
            D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        s_Instance->InitializeMsaa();

        s_Instance->m_Camera = std::make_unique<DirectXCamera>(Application::Get()->GetWindow()->GetWidth(), Application::Get()->GetWindow()->GetHeight(), 45.f, 0.1f, 1000.f);
        s_Instance->m_CommandObject = std::make_unique<DirectXCommandObject>();
        s_Instance->m_Swapchain = std::make_unique<DirectXSwapchain>(Application::Get()->GetWindow()->GetWidth(),
                                                                     Application::Get()->GetWindow()->GetHeight());
        s_Instance->m_Swapchain->Resize(Application::Get()->GetWindow()->GetWidth(),
                                        Application::Get()->GetWindow()->GetHeight());
        s_Instance->m_ResourceManager = std::make_unique<DirectXResourceManager>(1000);
        /*
        s_Instance->m_BaseShader = std::make_shared<DirectXSimpleShader>(VertexColor::GetLayout(), L"Shaders\\color.hlsl");
        s_Instance->m_TextureShader = std::make_shared<DirectXTextureShader>(VertexTex::GetLayout(), L"Shaders\\Builtin.Texture.hlsl");
        s_Instance->m_LitShader = std::make_shared<DirectXLitShader>(VertexTex::GetLayout(), L"Shaders\\Builtin.Lit.hlsl");
        */
        // ===== Frame Resources =====
        for(int i = 0; i < gNumFrameResources; ++i)
        {
            s_Instance->m_FramesData.push_back(std::make_unique<DirectXFrameData>(s_Instance->m_Device.Get(), 1));
        }
    }

    void DirectXContext::Shutdown()
    {
    }
    /*
    std::shared_ptr<DirectXShader> DirectXContext::GetBaseShader() const
    {
        return m_BaseShader;
    }

    std::shared_ptr<DirectXShader> DirectXContext::GetTextureShader() const
    {
        return m_TextureShader;
    }*/


    Microsoft::WRL::ComPtr<ID3D12Resource> DirectXContext::CreateDefaultBuffer(ID3D12Device* pDevice,
        ID3D12GraphicsCommandList* pCmdList, const void* pInitData, const UINT64 pByteSize,
        Microsoft::WRL::ComPtr<ID3D12Resource>& pUploadBuffer)
    {
        Microsoft::WRL::ComPtr<ID3D12Resource> defaultBuffer;
        
        const auto defaultProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        const auto uploadProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        const auto bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(pByteSize);

        // Create the actual default buffer resource.
        THROW_IF_FAILED(pDevice->CreateCommittedResource(
            &defaultProperty,
            D3D12_HEAP_FLAG_NONE,
            &bufferDesc,
		    D3D12_RESOURCE_STATE_COMMON,
            nullptr,
            IID_PPV_ARGS(defaultBuffer.GetAddressOf())));

        // In order to copy CPU memory data into our default buffer, we need to create
        // an intermediate upload heap.
        THROW_IF_FAILED(pDevice->CreateCommittedResource(
            &uploadProperty,
		    D3D12_HEAP_FLAG_NONE,
            &bufferDesc,
		    D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(pUploadBuffer.GetAddressOf())));


        // Describe the data we want to copy into the default buffer.
        D3D12_SUBRESOURCE_DATA subResourceData = {};
        subResourceData.pData = pInitData;
        subResourceData.RowPitch = static_cast<LONG_PTR>(pByteSize);
        subResourceData.SlicePitch = subResourceData.RowPitch;

        // Schedule to copy the data to the default buffer resource.  At a high level, the helper function UpdateSubresources
        // will copy the CPU memory into the intermediate upload heap.  Then, using ID3D12CommandList::CopySubresourceRegion,
        // the intermediate upload heap data will be copied to mBuffer.
        const auto barrierToCopy = CD3DX12_RESOURCE_BARRIER::Transition(defaultBuffer.Get(), 
                                                                        D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
	    pCmdList->ResourceBarrier(1, &barrierToCopy);
        UpdateSubresources<1>(pCmdList, defaultBuffer.Get(), pUploadBuffer.Get(), 0, 0, 1, &subResourceData);
        const auto barrierToRead = CD3DX12_RESOURCE_BARRIER::Transition(defaultBuffer.Get(),
                                                                        D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ);
	    pCmdList->ResourceBarrier(1, &barrierToRead);

        // Note: uploadBuffer has to be kept alive after the above function calls because
        // the command list has not been executed yet that performs the actual copy.
        // The caller can Release the uploadBuffer after it knows the copy has been executed.


        return defaultBuffer;
    }

    Microsoft::WRL::ComPtr<ID3DBlob> DirectXContext::CompileShader(const std::wstring& pFilename,
        const D3D_SHADER_MACRO* pDefines, const std::string& pEntrypoint, const std::string& pTarget)
    {
        UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
        compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

        HRESULT hr = S_OK;

        Microsoft::WRL::ComPtr<ID3DBlob> byteCode = nullptr;
        Microsoft::WRL::ComPtr<ID3DBlob> errors;
        hr = D3DCompileFromFile(pFilename.c_str(), pDefines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
            pEntrypoint.c_str(), pTarget.c_str(), compileFlags, 0, &byteCode, &errors);

        if(errors != nullptr)
            OutputDebugStringA(static_cast<char*>(errors->GetBufferPointer()));

        THROW_IF_FAILED(hr);

        return byteCode;
    }

    void DirectXContext::InitializeMsaa()
    {
        // Check 4X MSAA quality support for our back buffer format.
        // All Direct3D 11 capable devices support 4X MSAA for all render 
        // target formats, so we only need to check quality support.

        D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
        msQualityLevels.Format = DirectXSwapchain::k_BackBufferFormat;
        msQualityLevels.SampleCount = 4;
        msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
        msQualityLevels.NumQualityLevels = 0;
        THROW_IF_FAILED(m_Device->CheckFeatureSupport(
            D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
            &msQualityLevels,
            sizeof(msQualityLevels)));

        m_4xMsaaQuality = msQualityLevels.NumQualityLevels;
        assert(m_4xMsaaQuality > 0 && "Unexpected MSAA quality level.");
    }
}
