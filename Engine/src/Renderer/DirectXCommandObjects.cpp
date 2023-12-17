#include "DirectXCommandObjects.h"


namespace Engine
{
    DirectXCommandObjects::DirectXCommandObjects()
    {
        D3D12_COMMAND_QUEUE_DESC queueDesc = {};
        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        ThrowIfFailed(DirectXContext::Get()->m_Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_CommandQueue)));

        ThrowIfFailed(DirectXContext::Get()->m_Device->CreateCommandAllocator(
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            IID_PPV_ARGS(m_DirectCmdListAlloc.GetAddressOf())));

        ThrowIfFailed(DirectXContext::Get()->m_Device->CreateCommandList(
            0,
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            m_DirectCmdListAlloc.Get(), // Associated command allocator
            nullptr,                   // Initial PipelineStateObject
            IID_PPV_ARGS(m_CommandList.GetAddressOf())));

        // Start off in a closed state.  This is because the first time we refer 
        // to the command list we will Reset it, and it needs to be closed before
        // calling Reset.
        m_CommandList->Close();

        ThrowIfFailed(DirectXContext::Get()->m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE,
            IID_PPV_ARGS(&m_Fence)));
    }

    DirectXCommandObjects::~DirectXCommandObjects()
    {
    }

    void DirectXCommandObjects::Flush()
    {
        m_CurrentFence++;

        // Add an instruction to the command queue to set a new fence point.  Because we 
        // are on the GPU timeline, the new fence point won't be set until the GPU finishes
        // processing all the commands prior to this Signal().
        ThrowIfFailed(m_CommandQueue->Signal(m_Fence.Get(), m_CurrentFence));

        // Wait until the GPU has completed commands up to this fence point.
        if(m_Fence->GetCompletedValue() < m_CurrentFence)
        {
            const HANDLE eventHandle = CreateEventEx(nullptr, nullptr, false, EVENT_ALL_ACCESS);

            // Fire event when GPU hits current fence.  
            ThrowIfFailed(m_Fence->SetEventOnCompletion(m_CurrentFence, eventHandle));

            // Wait until the GPU hits current fence event is fired.
            WaitForSingleObject(eventHandle, INFINITE);
            CloseHandle(eventHandle);
        }
    }
}
