#pragma once
#include <assert.h>
#include <string>

#include "Events/ApplicationEvent.h"
#include "Platform/WindowsWindow.h"
#include "Renderer/Model.h"

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

    ApplicationSpecification m_Specification;

    bool m_IsRunning = true;
    bool m_IsMinimized = false;
    float m_LastFrameTime = 0.0f;

    Engine::WindowsWindow* m_Window;

    std::unique_ptr<Engine::Model> m_Quad; 

    static Application* s_Instance;
    
};
