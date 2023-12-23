#include "DirectXContext.h"

#include "DirectXCommandObject.h"
#include "DirectXPipeline.h"
#include "DirectXSwapchain.h"
#include "Core/Application.h"

const int gNumFrameResources = 3;

namespace Engine
{
    DirectXContext* DirectXContext::s_Instance = nullptr;

    void DirectXContext::Initialize()
    {
        assert(!s_Instance, "DirectX context already initialized!");
        s_Instance = new DirectXContext();

        // ===== Factory =====
        ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&s_Instance->m_Factory)));

        // ===== Device =====
        if (const HRESULT hardwareResult = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0,
                                                             IID_PPV_ARGS(&s_Instance->m_Device));
            FAILED(hardwareResult))
        {
            Microsoft::WRL::ComPtr<IDXGIAdapter> warpAdapter;
            ThrowIfFailed(s_Instance->m_Factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));
            ThrowIfFailed(D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&s_Instance->m_Device)));
        }
        s_Instance->m_CbvSrvUavDescriptorSize = s_Instance->m_Device->GetDescriptorHandleIncrementSize(
            D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        s_Instance->InitializeMsaa();

        s_Instance->m_CommandObject = std::make_unique<DirectXCommandObject>();
        s_Instance->m_Swapchain = std::make_unique<DirectXSwapchain>(Application::Get()->GetWindow()->GetWidth(),
                                                                     Application::Get()->GetWindow()->GetHeight());
        s_Instance->m_Swapchain->Resize(Application::Get()->GetWindow()->GetWidth(),
                                        Application::Get()->GetWindow()->GetHeight());

        std::vector<D3D12_INPUT_ELEMENT_DESC> layout = {
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
            {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
        };
        s_Instance->m_BasePipeline = std::make_unique<DirectXPipeline>(layout, L"Shaders\\color.hlsl");

        // ===== Frame Resources =====
        D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
        cbvHeapDesc.NumDescriptors = gNumFrameResources;
        cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        cbvHeapDesc.NodeMask = 0;
        ThrowIfFailed(DirectXContext::Get()->m_Device->CreateDescriptorHeap(&cbvHeapDesc,
            IID_PPV_ARGS(&s_Instance->m_PassConstantHeap)));
        
        for(int i = 0; i < gNumFrameResources; ++i)
        {
            s_Instance->m_FramesData.push_back(std::make_unique<DirectXFrameData>(s_Instance->m_Device.Get(), 1));
            
            auto passCB = s_Instance->m_FramesData[i]->PassCB->Resource();
            D3D12_GPU_VIRTUAL_ADDRESS cbAddress = passCB->GetGPUVirtualAddress();

            // Offset to the pass cbv in the descriptor heap.
            int heapIndex = i;
            auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(s_Instance->m_PassConstantHeap->GetCPUDescriptorHandleForHeapStart());
            handle.Offset(heapIndex, s_Instance->m_CbvSrvUavDescriptorSize);

            D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
            cbvDesc.BufferLocation = cbAddress;
            cbvDesc.SizeInBytes = d3dUtil::CalcConstantBufferByteSize(sizeof(PassConstants));
        
            s_Instance->m_Device->CreateConstantBufferView(&cbvDesc, handle);
        }
    }

    void DirectXContext::Shutdown()
    {
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
        ThrowIfFailed(DirectXContext::Get()->m_Device->CheckFeatureSupport(
            D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
            &msQualityLevels,
            sizeof(msQualityLevels)));

        m_4xMsaaQuality = msQualityLevels.NumQualityLevels;
        assert(m_4xMsaaQuality > 0 && "Unexpected MSAA quality level.");
    }
}
