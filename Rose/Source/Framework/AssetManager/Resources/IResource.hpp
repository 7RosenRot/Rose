#pragma once

namespace Rose::Framework::Internal
{

  enum class Type : std::uint32_t
  {
    Unknown, Mesh, Texture
  };

  struct RESOURCE_HANDLE
  {
   private:
    static constexpr std::uint32_t INDEX_MAX = 0x000FFFFFU;
    static constexpr std::uint32_t STAGE_MAX = 0x00000FFFU;

    static constexpr std::uint32_t INDEX_MASK = 0x000FFFFFU;
    static constexpr std::uint32_t STAGE_MASK = 0xFFF00000U;

    Type TYPE = Type::Unknown;
    uint32_t ID = 0x00000000U;

    //       max: 4096
    //       stage
    //       ↓
    //   0x001000001
    //        ↑
    // entities
    // max: 1048576

   public:
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
      
      ID = 0x00000000U;
    }

    inline bool IsValid() const noexcept
    {
      return TYPE != Type::Unknown;
    }

    inline Type GetType() const noexcept
    {
      return TYPE;
    }

    inline std::uint32_t GetID() const noexcept
    {
      return ID;
    }

    inline std::uint32_t GetIndex() const noexcept
    {
      return ID & INDEX_MASK;
    }

    inline std::uint32_t GetStage() const noexcept
    {
      return (ID & STAGE_MASK) >> 20U;
    }

    inline void SetType(_In_ const Type& Type) noexcept
    {
      TYPE = Type;
    }

    inline void SetIndex(_In_ std::uint32_t& Index) noexcept
    {
      if (Index > INDEX_MAX)
      {
        // LOGGER expected

        return;
      }

      ID = (ID & STAGE_MASK) | (Index & INDEX_MASK);
    }

    inline void SetStage(_In_ std::uint32_t& Stage) noexcept
    {
      if (Stage > STAGE_MAX)
      {
        // LOGGER expected

        return;
      }

      ID = (Stage << 20U) | (ID & INDEX_MASK);
    }

    inline void UpStage() noexcept
    {
      std::uint32_t Stage = GetStage();

      Stage = (Stage + 1U) & STAGE_MAX;

      ID = (Stage << 20U) | (ID & INDEX_MASK);
    }

    bool operator==(_In_ const RESOURCE_HANDLE Other) const noexcept
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