#include "DirectXContext.h"

#include "DirectXCommandObjects.h"
#include "DirectXSwapchain.h"
#include "Core/Application.h"

namespace Engine
{
    DirectXContext* DirectXContext::s_Instance = nullptr;
    
    void DirectXContext::Initialize()
    {
        assert(!s_Instance, "DirectX context already initialized!");
        s_Instance = new DirectXContext();
        ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&s_Instance->m_Factory)));

        // Try to create hardware device.
        HRESULT hardwareResult = D3D12CreateDevice(
            nullptr,             // default adapter
            D3D_FEATURE_LEVEL_11_0,
            IID_PPV_ARGS(&s_Instance->m_Device));

        // Fallback to WARP device.
        if(FAILED(hardwareResult))
        {
            Microsoft::WRL::ComPtr<IDXGIAdapter> warpAdapter;
            ThrowIfFailed(s_Instance->m_Factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));

            ThrowIfFailed(D3D12CreateDevice(
                warpAdapter.Get(),
                D3D_FEATURE_LEVEL_11_0,
                IID_PPV_ARGS(&s_Instance->m_Device)));
        }

        s_Instance->m_RtvDescriptorSize = s_Instance->m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        s_Instance->m_DsvDescriptorSize = s_Instance->m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
        s_Instance->m_CbvSrvUavDescriptorSize = s_Instance->m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

        s_Instance->m_CommandObjects = new DirectXCommandObjects();
        s_Instance->m_Swapchain = new DirectXSwapchain(Application::Get()->GetWindow()->GetWidth(), Application::Get()->GetWindow()->GetHeight());
        s_Instance->CreateRtvAndDsvDescriptorHeaps();
        s_Instance->m_Swapchain->Resize(Application::Get()->GetWindow()->GetWidth(), Application::Get()->GetWindow()->GetHeight());
    }

    void DirectXContext::Shutdown()
    {
        delete s_Instance->m_Swapchain;
        delete s_Instance->m_CommandObjects;
    }

    void DirectXContext::CreateRtvAndDsvDescriptorHeaps()
    {
        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
        rtvHeapDesc.NumDescriptors = DirectXSwapchain::k_SwapChainBufferCount;
        rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        rtvHeapDesc.NodeMask = 0;
        ThrowIfFailed(m_Device->CreateDescriptorHeap(
            &rtvHeapDesc, IID_PPV_ARGS(m_RtvHeap.GetAddressOf())));


        D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
        dsvHeapDesc.NumDescriptors = 1;
        dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        dsvHeapDesc.NodeMask = 0;
        ThrowIfFailed(m_Device->CreateDescriptorHeap(
            &dsvHeapDesc, IID_PPV_ARGS(m_DsvHeap.GetAddressOf())));
    }
}
