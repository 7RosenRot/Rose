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

namespace Rose::Framework
{

  class AssetManager
  {
   public:
    AssetManager() = default;
    ~AssetManager() = default;
    
    void Initialize(_In_ const std::shared_ptr<Renderer::IRenderer>& pRenderer);
    void Shutdown();
    
    Internal::ResourceDesc UploadResource(_In_ const std::string& Path);
    void DeleteResource(_In_ const std::string& Path);
    
    template <typename T>
    T* GetResource(_In_ const Internal::ResourceDesc& Desc);

   private:
    template <typename T>
    void RegistryLoader();

    template <typename T>
    class ResourcePool
    {
     public:
      std::uint32_t Push(std::shared_ptr<T> pResource)
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

      void Pop(std::uint32_t Index)
      {
        if (Index < m_pResources.size())
        {
          m_pResources[Index].reset();
          m_FreeIndices.push(Index);
        }
      }

      template <typename T>
      std::shared_ptr<T> Get(std::uint32_t Index)
      {
        if (Index < m_pResources.size())
        {
          return m_pResources[Index];
        }

        return nullptr;
      }
     private:
      std::vector<std::shared_ptr<T>> m_pResources;
      std::queue<std::uint32_t> m_FreeIndices;
    };
    
    std::shared_ptr<Renderer::IRenderer> m_pRenderer;

    std::vector<std::shared_ptr<Internal::IResourceLoader>> m_pLoaders;

    std::unordered_map<std::string, Internal::ResourceDesc> m_ResourceMap;
  };

  template <typename T>
  void AssetManager::RegistryLoader()
  {
    auto pLoader = std::make_shared<T>();

    pLoader->Initialize(m_pRenderer);

    m_pLoaders.push_back(pLoader);
  }

  template<typename T>
  T* AssetManager::GetResource(_In_ const Internal::ResourceDesc& Desc)
  {
    switch (Desc.Type)
    {
      case Internal::ResourceType::Mesh:
      {
        if (Desc.Index < m_pMeshes.size())
        {
          return static_cast<T*>(m_pMeshes[Desc.Index].get());
        }
      
        break;
      }

      case Internal::ResourceType::Texture:
      {
        if (Desc.Index < m_pTextures.size())
        {
          return static_cast<T*>(m_pTextures[Desc.Index].get());
        }
      
        break;
      }

      /* case Internal::ResourceType::Other: {...} */

      default:
      {
        return nullptr;
      }
    }

    return nullptr;
  }

} //  Rose::Framework