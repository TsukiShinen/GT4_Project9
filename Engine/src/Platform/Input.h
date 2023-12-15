#pragma once

#include "Core/KeyCodes.h"
#include "Core/MouseCodes.h"

namespace Engine
{
	class Input
	{
	public:
		static bool IsKeyPressed(const KeyCode pKey) { return s_Keys[pKey]; }

		static bool IsMouseButtonPressed(const MouseCode pButton) { return s_MouseButtons[pButton]; }
		static float GetMouseX() { return s_MousePositionX; }
		static float GetMouseY() { return s_MousePositionY; }

	private:
		static bool s_Keys[512];

		static bool s_MouseButtons[16];
		static float s_MousePositionX;
		static float s_MousePositionY;

		friend class WindowsWindow;
	};
}
