#include "ObjLoader.hpp"
#include <unordered_map>

struct VKey
{
  int v, vt, vn;

  bool operator==(const VKey& Other) const
  {
    return v == Other.v && vt == Other.vt && vn == Other.vn;
  }
};

struct VHasher
{
  std::size_t operator()(const VKey& Key) const
  {
    return std::hash<int>()(Key.v) ^ (std::hash<int>()(Key.vt) << 1) ^ (std::hash<int>()(Key.vn) << 2);
  }
};

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
    std::unordered_map<VKey, std::uint32_t, VHasher> UniqueVertices;

    while (std::getline(ObjFile, Token))
    {
      char* TokenData = Token.data();

      while (*TokenData == ' ') { ++TokenData; }
      if (*TokenData == '\0' && *TokenData == '\r' && *TokenData == '\n') { break; }
      
      if (TokenData[0] == 'v' && TokenData[1] == ' ')
      {
        Rose::Core::Math::FLOAT3 Position;
        Position.x = std::strtof(TokenData, &TokenData);
        Position.y = std::strtof(TokenData, &TokenData);
        Position.z = std::strtof(TokenData, &TokenData);
        Position.z *= -1.0f;

        PositionBuffer.push_back(Position);
      }

      else if (TokenData[0] == 'v' && TokenData[1] == 't' && TokenData[2] == ' ')
      {
        Rose::Core::Math::FLOAT2 Texture;
        Texture.x = std::strtof(TokenData, &TokenData);
        Texture.y = std::strtof(TokenData, &TokenData);
        Texture.y = 1.0f - Texture.y;

        TextureBuffer.push_back(Texture);
      }

      else if (TokenData[0] == 'v' && TokenData[1] == 'n' && TokenData[2] == ' ')
      {
        Rose::Core::Math::FLOAT3 Normal;
        Normal.x = std::strtof(TokenData, &TokenData);
        Normal.y = std::strtof(TokenData, &TokenData);
        Normal.z = std::strtof(TokenData, &TokenData);
        Normal.z *= -1.0f;

        NormalBuffer.push_back(Normal);
      }

      else if (TokenData[0] == 'f' && TokenData[1] == ' ')
      {
        std::vector<std::uint32_t> FaceIndices;
        
        // f v/vt/vn v/vt/vn v/vt/vn
        // f 14/95/107 2727/722/760 858/103/115

        while (*TokenData != '\0' && *TokenData != '\r' && *TokenData != '\n')
        {
          int vIdx = 0, vtIdx = 0, vnIdx = 0;

          while (*TokenData == ' ') { ++TokenData; }
          if (*TokenData == '\0' && *TokenData == '\r' && *TokenData == '\n') { break; }

          vIdx = static_cast<int>(std::strtol(TokenData, &TokenData, 10));
          ++TokenData;

          if (*TokenData == '/')
          {
            ++TokenData;
            if (*TokenData == '/')
            {
              ++TokenData;
              vnIdx = static_cast<int>(std::strtol(TokenData, &TokenData, 10));
            }
            else
            {
              vtIdx = static_cast<int>(std::strtol(TokenData, &TokenData, 10));
              
              if (*TokenData == '/')
              {
                ++TokenData;
                vnIdx = static_cast<int>(std::strtol(TokenData, &TokenData, 10));
              }
            }

          }
          VKey Key{vIdx, vtIdx, vnIdx};
          std::uint32_t Index = 0;

          auto it = UniqueVertices.find(Key);
          if (it != UniqueVertices.end())
          {
            Index = it->second;
          }

          else
          {
            Vertex Vertex_{};
    
            if (vIdx > 0 && static_cast<size_t>(vIdx) <= PositionBuffer.size())
            {
              Vertex_.Position = PositionBuffer[vIdx - 1];
            }
    
            if (vtIdx > 0 && static_cast<size_t>(vtIdx) <= TextureBuffer.size())
            {
              Vertex_.Texture = TextureBuffer[vtIdx - 1];
            }
  
            if (vnIdx > 0 && static_cast<size_t>(vnIdx) <= NormalBuffer.size())
            {
              Vertex_.Normal = NormalBuffer[vnIdx - 1];
            }
  
            Index = static_cast<std::uint32_t>(Vertices.size());

            Vertices.push_back(Vertex_);

            UniqueVertices[Key] = Index;
          }

          FaceIndices.push_back(Index);

          vIdx = 0; vtIdx = 0; vnIdx = 0;
        }

        if (FaceIndices.size() >= 3)
        {
          for (std::size_t i = 1; i + 1 < FaceIndices.size(); i += 1)
          {
            Indices.push_back(FaceIndices[0]);
            Indices.push_back(FaceIndices[i]);
            Indices.push_back(FaceIndices[i + 1]);
          }
        }
      }
    }

    return true;
  }

} //  namespace Rose::Framework::Internal

//////////////////////////////////////////
//              *EXAMPLE*               //
// v -1.034430 17.354212 1.871622       //
// vn 0.4809 0.0080 -0.8767             //
// vt 0.419992 0.409470                 //
// f 14/95/107 2727/722/760 858/103/115 //
//////////////////////////////////////////