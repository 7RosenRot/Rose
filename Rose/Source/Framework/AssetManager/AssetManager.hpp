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

class Pool
{
 private:
  struct Slot
  {
    Rose::Framework::Internal::RESOURCE_HANDLE Handle{};
    std::shared_ptr<Rose::Framework::Internal::IResource> Ptr = nullptr;
  };

  std::vector<Slot> m_Slots;
  std::queue<std::uint32_t> m_FreeIndices;

 public:
  Rose::Framework::Internal::RESOURCE_HANDLE 
    Push(_In_ const std::shared_ptr<Rose::Framework::Internal::IResource>& pResource)
  {
    std::uint32_t Index = 0;

    if (!m_FreeIndices.empty())
    {
      Index = m_FreeIndices.front();
      m_FreeIndices.pop();

      m_Slots[Index].Handle.SetType(pResource->GetType());
      m_Slots[Index].Ptr = pResource;
    }
    else
    {
      Index = static_cast<std::uint32_t>(m_Slots.size());

      Rose::Framework::Internal::RESOURCE_HANDLE Handle
      {
        pResource->GetType(), 0, Index
      };
      
      m_Slots.push_back({Handle, pResource});
    }

    return m_Slots[Index].Handle;
  }
  
  void Pop(_In_ const Rose::Framework::Internal::RESOURCE_HANDLE& Handle)
  {
    std::uint32_t Index = Handle.GetIndex();

    if (Index < m_Slots.size())
    {
      m_Slots[Index].Handle.SetType(Rose::Framework::Internal::Type::Unknown);
      m_Slots[Index].Handle.UpStage();
      m_Slots[Index].Ptr.reset();

      m_FreeIndices.push(Index);
    }
  }
  
  std::shared_ptr<Rose::Framework::Internal::IResource>
    Get(_In_ Rose::Framework::Internal::RESOURCE_HANDLE& Handle)
  {
    std::uint32_t Index = Handle.GetIndex();

    if (Index < m_Slots.size())
    {
      if (m_Slots[Index].Handle.GetStage() == Handle.GetStage())
      {
        return m_Slots[Index].Ptr;
      }
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
    Pool m_Pool;
  };

  template <typename T>
  void AssetManager::RegistryLoader()
  {
    auto pLoader = std::make_shared<T>();

    pLoader->Initialize(m_pRenderer);

    m_pLoaders.push_back(pLoader);
  }

  template<typename T>
  std::shared_ptr<T> AssetManager::GetResource(_In_ const Internal::RESOURCE_HANDLE& Handle)
  {
    auto pResource = m_Pool.Get(Handle);

    if (!pResource)
    {
      // LOGGER expected

      return nullptr;
    }

    return std::dynamic_pointer_cast<T>(pResource);
  }

} //  Rose::Framework