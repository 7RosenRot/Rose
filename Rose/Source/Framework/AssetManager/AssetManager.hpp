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
    
    void Initialize(const std::shared_ptr<Renderer::IRenderer>& pRenderer);
    void Shutdown();
    
    Internal::ResourceDesc UploadResource(const std::string& Path);
    void DeleteResource(const std::string& Path);
    
    template <typename T>
    T* GetResource(const Internal::ResourceDesc& Desc);

   private:
    template <typename T>
    void RegistryLoader();
    
    Renderer::IRenderer* m_pRenderer;

    std::vector<std::shared_ptr<Internal::IResourceLoader>> m_pLoaders;

    std::unordered_map<std::string, Internal::ResourceDesc> m_ResourceMap;

    std::vector<std::shared_ptr<Internal::IMesh>> m_pMeshes;
    std::vector<std::shared_ptr<Internal::ITexture>> m_pTextures;
    
    std::queue<uint32_t> m_FreeMeshIndices;
    std::queue<uint32_t> m_FreeTextureIndices;
  };

  template <typename T>
  void AssetManager::RegistryLoader()
  {
    auto pLoader = std::make_shared<T>();

    pLoader->Initialize(m_pRenderer);

    m_pLoaders.push_back(pLoader);
  }

  template<typename T>
  T* AssetManager::GetResource(const Internal::ResourceDesc& Desc)
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