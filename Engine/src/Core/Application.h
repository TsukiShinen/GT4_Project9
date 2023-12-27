#pragma once
#include <assert.h>
#include <string>

#include "Events/ApplicationEvent.h"
#include "Platform/WindowsWindow.h"
#include "Renderer/DirectXMesh.h"

struct ApplicationCommandLineArgs
{
    int Count = 0;
    char** Args = nullptr;

    const char* operator[](const int pIndex) const
    {
        assert(pIndex < Count);
        return Args[pIndex];
    }
};

struct ApplicationSpecification
{
    std::string Name = "Application";
    ApplicationCommandLineArgs CommandLineArgs;
};

class Application
{
public:
    Application(const ApplicationSpecification& pSpecification);
    virtual ~Application();
    
    void Run();

    [[nodiscard]] const ApplicationSpecification& GetSpecification() const { return m_Specification; }

    Engine::WindowsWindow* GetWindow() const { return m_Window; }
    void SetMinimized(const bool pValue) { m_IsMinimized = pValue; }

    static Application* Get() { return s_Instance; }
    
private:
    virtual void OnEvent(Engine::Event& pEvent);
    
    bool OnWindowClose(Engine::WindowCloseEvent& pEvent);
    bool OnWindowResize(const Engine::WindowResizeEvent& pEvent);

    void GameUpdate(float dt);

    ApplicationSpecification m_Specification;

    bool m_IsRunning = true;
    bool m_IsMinimized = false;
    __int64 m_LastFrameTime = 0;
	double m_SecondsPerCount;
    float m_Fps = 0.0f;

    Engine::WindowsWindow* m_Window;

    std::unique_ptr<Engine::DirectXMesh> m_Quad; 

    static Application* s_Instance;
};
