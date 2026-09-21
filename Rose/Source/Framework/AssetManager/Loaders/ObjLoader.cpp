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

    std::vector<Rose::Core::Math::FLOAT3> PositionBuffer;
    std::vector<Rose::Core::Math::FLOAT2> TextureBuffer;
    std::vector<Rose::Core::Math::FLOAT3> NormalBuffer;

    std::string Line;
    std::uint32_t Index = 0;

    while (std::getline(ObjFile, Line))
    {
      std::istringstream Stream(Line);
      std::string VertexData_t;

      Stream >> VertexData_t;
      if (VertexData_t == "v")
      {
        Rose::Core::Math::FLOAT3 Position;
        Stream >> Position.x >> Position.y >> Position.z;
        Position.z *= -1.0f;

        PositionBuffer.push_back(Position);
      }

      else if (VertexData_t == "vt")
      {
        Rose::Core::Math::FLOAT2 Texture;
        Stream >> Texture.x >> Texture.y;
        Texture.y = 1.0f - Texture.y;

        TextureBuffer.push_back(Texture);
      }

      else if (VertexData_t == "vn")
      {
        Rose::Core::Math::FLOAT3 Normal;
        Stream >> Normal.x >> Normal.y >> Normal.z;
        Normal.z *= -1.0f;

        NormalBuffer.push_back(Normal);
      }

      else if (VertexData_t == "f")
      {
        for (int i = 0; i < 3; i += 1)
        {
          // face: v/vt/vn v/vt/vn v/vt/vn
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
            VertexData.Position = PositionBuffer[vIdx - 1];
          }
        
          if (vtIdx > 0)
          {
            VertexData.Texture = TextureBuffer[vtIdx - 1];
          }
        
          if (vnIdx > 0)
          {
            VertexData.Normal = NormalBuffer[vnIdx - 1];
          }
        
          Vertices.push_back(VertexData);
          Indices.push_back(Index++);
        }
      }
    }

    return true;
  }

} //  namespace Rose::Framework::Internal