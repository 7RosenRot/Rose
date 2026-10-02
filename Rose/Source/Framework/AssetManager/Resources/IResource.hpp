#pragma once

namespace Rose::Framework::Internal
{

  enum class Type : uint32_t
  {
    Unknown, Mesh, Texture
  };

  struct RESOURCE_HANDLE
  {
    static constexpr std::uint32_t INDEX_MAX = 0x000FFFFFU;
    static constexpr std::uint32_t STAGE_MAX = 0x00000FFFU;

    static constexpr std::uint32_t INDEX_MASK = 0x000FFFFFU;
    static constexpr std::uint32_t STAGE_MASK = 0xFFF00000U;

    Type TYPE = Type::Unknown;
    uint32_t ID = 0xFFFFFFFFU;
    
    //       max: 4096
    //       stage
    //       ↓
    //   0x00100001
    //        ↑
    // entities
    // max: 1048576

    RESOURCE_HANDLE() = default;
    
    RESOURCE_HANDLE(
      _In_ Internal::Type Type,
      _In_ std::uint32_t Stage,
      _In_ std::uint32_t Index
    ) : TYPE(Type)
    {
      SetStage(Stage);
      
      SetIndex(Index);
    }

    inline void Reset() noexcept
    {
      TYPE = Type::Unknown;
      
      ID = 0xFFFFFFFF;
    }

    inline bool IsValid() const noexcept
    {
      return TYPE != Type::Unknown && ID != 0xFFFFFFFFU;
    }

    inline std::uint32_t GetIndex() const noexcept
    {
      return ID & INDEX_MASK;
    }

    inline std::uint32_t GetStage() const noexcept
    {
      return (ID & STAGE_MASK) >> 20U;
    }

    inline void SetIndex(std::uint32_t Index) noexcept
    {
      if (Index > INDEX_MAX)
      {
        // LOGGER expected

        return;
      }
      
      ID = (ID & STAGE_MASK) | (Index & INDEX_MASK);
    }
    
    inline void SetStage(std::uint32_t Stage) noexcept
    {
      if (Stage > STAGE_MAX)
      {
        // LOGGER expected

        return;
      }

      ID = (Stage << 20U) | (ID & INDEX_MASK);
    }

    inline void UpIndex() noexcept
    {
      std::uint32_t Index = GetIndex();

      if (Index < INDEX_MASK)
      {
        ++ID;
      }

      else
      {
        ID &= ~INDEX_MASK;
      }
    }

    inline void UpStage() noexcept
    {
      std::uint32_t Stage = GetStage();

      Stage = (Stage + 1U) & STAGE_MAX;

      if (Stage == 0)
      {
        Stage = 1;
      }

      ID = (Stage << 20U) | (ID & INDEX_MASK);
    }

    RESOURCE_HANDLE& operator++() noexcept
    {
      UpIndex();

      return *this;
    }

    bool operator==(const RESOURCE_HANDLE Other) const noexcept
    {
      return (TYPE == Other.TYPE) && (ID == Other.ID);
    }

    bool operator!=(const RESOURCE_HANDLE Other) const noexcept
    {
      return !(*this == Other);
    }
  };

  struct Vertex
  {
    Core::Math::FLOAT3 Position = {0.0F, 0.0F, 0.0F};
    Core::Math::FLOAT2 Texture  = {0.0F, 0.0F};
    Core::Math::FLOAT3 Normal   = {0.0F, 0.0F, 0.0F};
  };
  
  class IResource
  {
   public:
    IResource() = default;
    virtual ~IResource() = default;

    virtual Type GetType() const = 0;
  };

} //  namespace Rose::Framework::Internal