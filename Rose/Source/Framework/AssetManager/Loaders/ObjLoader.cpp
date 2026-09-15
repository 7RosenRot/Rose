#include "ObjLoader.hpp"

namespace Rose::Framework::Internal
{

  std::shared_ptr<IResource> ObjLoader::LoadResource(const std::string& Path)
  {
    if (!m_pRenderer)
    {
      // LOGGER expected

      return nullptr;
    }

    std::vector<Vertex> Vertices;
    std::vector<std::uint32_t> Indices;

    if (!ParseObjFile(Path, Vertices, Indices))
    {
      // LOGGER expected

      return nullptr;
    }

    std::shared_ptr<IMesh> pMesh = m_pRenderer->CreateMesh(Vertices, Indices);

    return pMesh;
  }

  bool ObjLoader::ParseObjFile(
    _In_  const std::string& Path,
    _Out_ std::vector<Vertex>& Vertices,
    _Out_ std::vector<std::uint32_t>& Indices
  ) {
    std::ifstream ObjFile(Path);

    if (!ObjFile.is_open()) {
      // LOGGER expected

      return false;
    }

    std::vector<Rose::Core::Math::FLOAT3> tmpPositionBuffer;
    std::vector<Rose::Core::Math::FLOAT2> tmpTextureBuffer;
    std::vector<Rose::Core::Math::FLOAT3> tmpNormalBuffer;

    std::string VtxData;
    std::uint32_t Index = 0;

    while (std::getline(ObjFile, VtxData))
    {
      std::istringstream Stream(VtxData);
      std::string VertexData_t;

      Stream >> VertexData_t;
      if (VertexData_t == "v")
      {
        Rose::Core::Math::FLOAT3 Position;
        Stream >> Position.x >> Position.y >> Position.z;
        Position.z *= -1.0f;

        tmpPositionBuffer.push_back(Position);
      }

      else if (VertexData_t == "vt")
      {
        Rose::Core::Math::FLOAT2 Texture;
        Stream >> Texture.x >> Texture.y;
        Texture.y = 1.0f - Texture.y;

        tmpTextureBuffer.push_back(Texture);
      }

      else if (VertexData_t == "vn")
      {
        Rose::Core::Math::FLOAT3 Normal;
        Stream >> Normal.x >> Normal.y >> Normal.z;
        Normal.z *= -1.0f;

        tmpNormalBuffer.push_back(Normal);
      }

      else if (VertexData_t == "f")
      {
        // face: v/vt/vn
        int vIdx = 0, vtIdx = 0, vnIdx = 0;
        char slash;

        Stream >> vIdx;

        if (Stream.peek() == '/')
        {
          Stream >> slash;

          if (Stream.peek() == '/')
          {
            Stream >> slash;
            Stream >> vnIdx;
          }

          else
          {
            Stream >> vtIdx;

            if (Stream.peek() == '/')
            {
              Stream >> slash;
              Stream >> vnIdx;
            }
          }
        }

        Vertex VertexData{};

        if (vIdx > 0)
        {
          VertexData.Position = tmpPositionBuffer[vIdx - 1];
        }

        if (vtIdx > 0)
        {
          VertexData.Texture = tmpTextureBuffer[vtIdx - 1];
        }

        if (vnIdx > 0)
        {
          VertexData.Normal = tmpNormalBuffer[vnIdx - 1];
        }

        Vertices.push_back(VertexData);
        Indices.push_back(Index++);
      }
    }

    return true;
  }

} //  namespace Rose::Framework::Internal