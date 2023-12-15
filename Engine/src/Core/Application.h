﻿#pragma once
#include <string>

#include "Debug/Assert.h"
#include "Events/ApplicationEvent.h"
#include "Platform/Window.h"

struct ApplicationCommandLineArgs
{
    int Count = 0;
    char** Args = nullptr;

    const char* operator[](const int pIndex) const
    {
        CORE_ASSERT(pIndex < Count);
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

    Engine::Window* GetWindow() const { return m_Window; }
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

    Engine::Window* m_Window;

    static Application* s_Instance;
    
};