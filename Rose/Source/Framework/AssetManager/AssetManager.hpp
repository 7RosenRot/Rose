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
 public:
  std::uint32_t Push(_In_ std::shared_ptr<T> pResource)
  {
    if (!m_FreeIndices.empty())
    {
      std::uint32_t Index = m_FreeIndices.front();
      m_FreeIndices.pop();
      m_pResources[Index] = pResource;
      
      return Index;
    }
    m_pResources.push_back(pResource);
    
    return static_cast<std::uint32_t>(m_pResources.size() - 1);
  }
  
  void Pop(_In_ std::uint32_t Index)
  {
    if (Index < m_pResources.size())
    {
      m_pResources[Index].reset();

      m_FreeIndices.push(Index);
    }
  }
  
  std::shared_ptr<T> Get(_In_ std::uint32_t Index)
  {
    if (Index < m_pResources.size())
    {
      return m_pResources[Index];
    }
  
    return nullptr;
  }

 private:
  struct Slot
  {
    std::shared_ptr<T> pResource;
    std::uint32_t Stage = 1;
  };

  std::vector<Slot> m_Slots;
  std::queue<std::uint32_t> m_FreeIndices;
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