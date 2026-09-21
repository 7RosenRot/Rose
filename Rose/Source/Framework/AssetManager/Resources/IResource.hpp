#pragma once

namespace Rose::Framework::Internal
{

  enum class Type : uint32_t
  {
    Unknown, Mesh, Texture
  };

  struct RESOURCE_DESC
  {
    Type Type = Type::Unknown;
    uint32_t Index = -1;
  };

  struct Vertex
  {
    Core::Math::FLOAT3 Position;
    Core::Math::FLOAT2 Texture;
    Core::Math::FLOAT3 Normal;
  };
  
  class IResource
  {
   public:
    IResource() = default;
    virtual ~IResource() = default;

    virtual Type GetType() const = 0;
  };

} //  namespace Rose::Framework::Internal