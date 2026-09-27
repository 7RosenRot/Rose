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

    std::string Token;
    std::uint32_t Index = 0;

    while (std::getline(ObjFile, Token))
    {
      std::istringstream VertexToken(Token);
      std::string VertexData_t;

      VertexToken >> VertexData_t;
      if (VertexData_t == "v")
      {
        Rose::Core::Math::FLOAT3 Position;
        VertexToken >> Position.x >> Position.y >> Position.z;
        Position.z *= -1.0f;

        PositionBuffer.push_back(Position);
      }

      else if (VertexData_t == "vt")
      {
        Rose::Core::Math::FLOAT2 Texture;
        VertexToken >> Texture.x >> Texture.y;
        Texture.y = 1.0f - Texture.y;

        TextureBuffer.push_back(Texture);
      }

      else if (VertexData_t == "vn")
      {
        Rose::Core::Math::FLOAT3 Normal;
        VertexToken >> Normal.x >> Normal.y >> Normal.z;
        Normal.z *= -1.0f;

        NormalBuffer.push_back(Normal);
      }

      else if (VertexData_t == "f")
      {
        std::string FaceData_t;
        
        std::vector<Vertex> VertexList;
        
        while (VertexToken >> FaceData_t)
        {
          std::istringstream FaceToken(FaceData_t);

          // face: v/vt/vn v/vt/vn v/vt/vn
          int vIdx = 0, vtIdx = 0, vnIdx = 0;
          char slash;
        
          FaceToken >> vIdx;
  
          if (FaceToken.peek() == '/')
          {
            FaceToken >> slash;

            if (FaceToken.peek() == '/')
            {
              FaceToken >> slash;
              FaceToken >> vnIdx;
            }

            else
            {
              FaceToken >> vtIdx;

              if (FaceToken.peek() == '/')
              {
                FaceToken >> slash;
                FaceToken >> vnIdx;
              }
            }
          }
  
          Vertex VertexData;
  
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

          VertexList.push_back(VertexData);

          if (VertexList.size() > 2)
          {
            for (std::size_t i = 1; i + 1 < VertexList.size(); i += 1)
            {
              Vertices.push_back(VertexList[0]);
              Indices.push_back(Index++);

              Vertices.push_back(VertexList[i]);
              Indices.push_back(Index++);

              Vertices.push_back(VertexList[i + 1]);
              Indices.push_back(Index++);
            }
          }
        }
      }
    }

    return true;
  }

} //  namespace Rose::Framework::Internal