#pragma once

#include "DirectXContext.h"

namespace Engine
{
    class DirectXApi
    {
    public:
        static void Initialize();
        static void Shutdown();

        static void Resize(int pWidth, int pHeight);

        static void BeginFrame();
        static void EndFrame();

        static void UpdateCamera(float dt);

    private:
        static void InitializeDebug();
        static void LogAdapters();
        static void LogAdapterOutputs(IDXGIAdapter* pAdapter);
        static void LogOutputDisplayModes(IDXGIOutput* pOutput, DXGI_FORMAT pFormat);
    };
    
}
