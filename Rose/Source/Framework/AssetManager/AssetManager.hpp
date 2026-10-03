#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <queue>
#include <filesystem>
#include <cstdint>
#include <cassert>

#include "Renderer/IRenderer.hpp"

#include "Resources/IResource.hpp"
#include "Resources/IMesh.hpp"
#include "Resources/ITexture.hpp"

#include "Loaders/IResourceLoader.hpp"
#include "Loaders/ObjLoader.hpp"
#include "Loaders/PngLoader.hpp"

template <typename T>
class Pool
{
 private:
  template <typename T>
  struct Slot
  {
    Rose::Framework::Internal::RESOURCE_HANDLE Handle{};
    std::shared_ptr<T> pResource = nullptr;
  };

  std::vector<Slot<T>> m_Slots;
  std::queue<std::uint32_t> m_FreeIndices;

 public:
  std::uint32_t Push(_In_ const std::shared_ptr<T>& pResource)
  {
    std::uint32_t Index;

    if (!m_FreeIndices.empty())
    {
      Index = m_FreeIndices.front();
      m_FreeIndices.pop();

      m_Slots[Index].pResource = pResource;
      m_Slots[Index].Handle.SetType(pResource->GetType());
      m_Slots[Index].Handle.UpStage();
      m_Slots[Index].Handle.SetIndex(Index);
    }
    else
    {
      Index = static_cast<std::uint32_t>(m_Slots.size());

      Rose::Framework::Internal::RESOURCE_HANDLE Handle{
        pResource->GetType(),
        0,
        Index
      };
      
      m_Slots.push_back({Handle, pResource});
    }

    return m_Slots[Index].Handle.GetID();
  }
  
  void Pop(_In_ const std::uint32_t& ID)
  {
    std::uint32_t Stage = ID & 0xFFF00000;
    std::uint32_t Index = ID & 0x000FFFFF;

    if (Index < m_Slots.size())
    {
      m_Slots[Index].pResource.reset();

      m_FreeIndices.push(Index);
    }
  }
  
  std::shared_ptr<T> Get(_In_ std::uint32_t ID)
  {
    std::uint32_t Stage = ID & 0xFFF00000;
    std::uint32_t Index = ID & 0x000FFFFF;

    if (Index < m_Slots.size())
    {
      return m_Slots[Index].pResource;
    }
  
    return nullptr;
  }
};

namespace Rose::Framework
{

  class AssetManager
  {
   public:
    AssetManager() = default;
    ~AssetManager() = default;

    void Initialize(_In_ const std::shared_ptr<Renderer::IRenderer>& pRenderer);
    void Shutdown();

    Internal::RESOURCE_HANDLE UploadResource(_In_ const std::string& Path);
    void DeleteResource(_In_ const std::string& Path);

    template <typename T>
    std::shared_ptr<T> GetResource(_In_ const Internal::RESOURCE_HANDLE& Desc);

   private:
    template <typename T>
    void RegistryLoader();

    std::shared_ptr<Renderer::IRenderer> m_pRenderer;
    std::vector<std::shared_ptr<Internal::IResourceLoader>> m_pLoaders;
    std::unordered_map<std::string, Internal::RESOURCE_HANDLE> m_ResourceMap;
    Pool<Internal::IResource> m_Pool;
  };

  template <typename T>
  void AssetManager::RegistryLoader()
  {
    auto pLoader = std::make_shared<T>();

    pLoader->Initialize(m_pRenderer);

    m_pLoaders.push_back(pLoader);
  }

  template<typename T>
  std::shared_ptr<T> AssetManager::GetResource(_In_ const Internal::RESOURCE_HANDLE& Resource)
  {
    auto pResource = m_Pool.Get(Resource.Index);

    if (!pResource)
    {
      // LOGGER expected

      return nullptr;
    }

    return std::dynamic_pointer_cast<T>(pResource);
  }

} //  Rose::Framework