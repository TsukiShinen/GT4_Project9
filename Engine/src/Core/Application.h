#pragma once
#include <assert.h>
#include <string>

#include "Timestep.h"
#include "Events/ApplicationEvent.h"
#include "Platform/WindowsWindow.h"
#include "Renderer/DirectXMesh.h"

int main(int pArgc, char** pArgv);

namespace Engine
{
	struct ApplicationSpecification
	{
		std::string Name = "Application";
	};

	class Application
	{
	public:
		Application(const ApplicationSpecification& pSpecification);
		virtual ~Application();

		[[nodiscard]] const ApplicationSpecification& GetSpecification() const { return m_Specification; }

		[[nodiscard]] WindowsWindow* GetWindow() const { return m_Window; }
		void SetMinimized(const bool pValue) { m_IsMinimized = pValue; }

		static Application* Get() { return s_Instance; }

	protected:
		virtual void Update(Timestep pDeltaTime);
		virtual void Draw() {}
		virtual void OnEvent(Event& pEvent);

	private:
		void Run();

		bool OnWindowClose(WindowCloseEvent& pEvent);
		bool OnWindowResize(const WindowResizeEvent& pEvent);

	private:
		ApplicationSpecification m_Specification;

		bool m_IsRunning = true;
		bool m_IsMinimized = false;
		__int64 m_LastFrameTime = 0;
		double m_SecondsPerCount;
		float m_Fps = 0.0f;

		WindowsWindow* m_Window;

	private:
		static Application* s_Instance;
		friend int ::main(int pArgc, char** pArgv);
	};
}
