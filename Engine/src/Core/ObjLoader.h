#pragma once

#include "Renderer/DirectXFrameData.h"
#include <vector>

namespace Engine
{
	

	class ObjLoader
	{
	public:
		static void LoadObj(const char* filePath, std::vector<Vertex>* objVertices);

	};
}


