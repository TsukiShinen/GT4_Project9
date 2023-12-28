#pragma once
#include <d3d12.h>
#include <string>
#include <wrl/client.h>


namespace Engine
{
    struct Texture
    {
        std::string Name;
        INT HeapIndex;

        std::wstring Filename;

        Microsoft::WRL::ComPtr<ID3D12Resource> Resource = nullptr;
        Microsoft::WRL::ComPtr<ID3D12Resource> UploadHeap = nullptr;
    };
}
