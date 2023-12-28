#include "ObjLoader.h"

#include "Platform/FilesSystem.h"
#include <sstream>
#include "Debug/Log.h"

#include <DirectXMath.h>

namespace Engine
{
	void ObjLoader::LoadObj(const char* filePath, std::vector<Vertex>* objVertices)
	{
		File file{};
		bool hasBeenOpened = FilesSystem::TryOpen(filePath, FileModeRead, false, &file);

		if (!hasBeenOpened) return;

		std::vector<DirectX::XMFLOAT3> vertices;
		std::vector<DirectX::XMFLOAT2> uvs;
		std::vector<DirectX::XMFLOAT3> normals;

		std::vector<int> face_vertex_index;
		std::vector<int> face_uv_index;
		std::vector<int> face_normal_index;

		std::istringstream ss;
		std::string prefix;
		std::string line;

		while (std::getline(*static_cast<std::ifstream*>(file.Handle), line))
		{
			ss.clear();
			ss.str(line);
			ss >> prefix;

			// Vertex
			if (prefix == "v")
			{
				std::istringstream ss(line.substr(2));
				float x, y, z;
				ss >> x >> y >> z;
				vertices.push_back(DirectX::XMFLOAT3(x, y, z));
			}

			// Vertex texture
			else if (prefix == "vt")
			{
				std::istringstream ss(line.substr(3));
				float u, v;
				ss >> u >> v;
				uvs.push_back(DirectX::XMFLOAT2(u, v));
			}

			// Vertex normal
			else if (prefix == "vn")
			{
				std::istringstream ss(line.substr(3));
				float x, y, z;
				ss >> x >> y >> z;
				normals.push_back(DirectX::XMFLOAT3(x, y, z));
			}

			// Faces
			else if (prefix == "f")
			{
				int counter = 0;
				int index;
				while (ss >> index)
				{
					switch (counter)
					{
					case 0:
						face_vertex_index.push_back(index);
						break;
					case 1:
						face_uv_index.push_back(index);
						break;
					case 2:
						face_normal_index.push_back(index);
						break;
					}


					while (ss.peek() == '/')
					{
						counter++;
						ss.ignore(1, '/');
					}

					if (ss.peek() == ' ') {
						counter = 0;
						ss.ignore(1, ' ');
					}
				}
			}
		}

		// recreate mesh
		objVertices->clear();
		for (size_t i = 0; i < face_vertex_index.size(); i++)
		{
			DirectX::XMFLOAT3 vertex = vertices[face_vertex_index[i] - 1];
			DirectX::XMFLOAT2 uv = uvs[face_uv_index[i] - 1];
			DirectX::XMFLOAT3 normal = vertices[face_normal_index[i] - 1];

			objVertices->push_back({ vertex , normal });
		}
	}
}