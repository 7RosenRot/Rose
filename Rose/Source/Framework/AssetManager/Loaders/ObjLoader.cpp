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
      std::istringstream TokenData(Token);
      std::string TokenData_t;

      TokenData >> TokenData_t;
      
      if (TokenData_t == "v")
      {
        Rose::Core::Math::FLOAT3 Position;
        TokenData >> Position.x >> Position.y >> Position.z;
        Position.z *= -1.0f;

        PositionBuffer.push_back(Position);
      }

      else if (TokenData_t == "vt")
      {
        Rose::Core::Math::FLOAT2 Texture;
        TokenData >> Texture.x >> Texture.y;
        Texture.y = 1.0f - Texture.y;

        TextureBuffer.push_back(Texture);
      }

      else if (TokenData_t == "vn")
      {
        Rose::Core::Math::FLOAT3 Normal;
        TokenData >> Normal.x >> Normal.y >> Normal.z;
        Normal.z *= -1.0f;

        NormalBuffer.push_back(Normal);
      }

      else if (TokenData_t == "f")
      {
        std::vector<std::uint32_t> FaceIndices;
        
        // face: v/vt/vn v/vt/vn v/vt/vn
        int vIdx = 0, vtIdx = 0, vnIdx = 0;
        char slash;
        
        while (TokenData >> vIdx)
        {
          if (TokenData.peek() == '/')
          {
            TokenData >> slash;

            if (TokenData.peek() == '/')
            {
              TokenData >> slash;
              TokenData >> vnIdx;
            }

            else
            {
              TokenData >> vtIdx;

              if (TokenData.peek() == '/')
              {
                TokenData >> slash;
                TokenData >> vnIdx;
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