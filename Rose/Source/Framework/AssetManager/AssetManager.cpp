#include "AssetManager.hpp"

namespace Rose::Framework {

  void AssetManager::Initialize(const std::shared_ptr<Renderer::IRenderer>& pRenderer)
  {
    m_pRenderer = pRenderer.get();

    RegistryLoader<Internal::ObjLoader>();
    RegistryLoader<Internal::PngLoader>();
  }

  void AssetManager::Shutdown() {}

  Internal::ResourceDesc AssetManager::UploadResource(const std::string& Path) {
    if (
      auto it = m_ResourceMap.find(Path);
      it != m_ResourceMap.end()
    ) {
      return it->second;
    }

    Internal::ResourceDesc Resource = {};

    std::string FileExtension = std::filesystem::path(Path).extension().string();

    std::shared_ptr<Internal::IResource> pResource = nullptr;

    for (const auto& pLoader : m_pLoaders)
    {
      if (pLoader->GetPattern() == FileExtension)
      {
        pResource = pLoader->LoadResource(Path);

        break;
      }
    }

    if (pResource == nullptr)
    {
      return Resource;
    }

    Resource.Type = pResource->GetType();
    
    uint32_t Index = -1;

    switch (Resource.Type)
    {
      case Internal::ResourceType::Mesh: 
      {
        std::shared_ptr<Internal::IMesh>
          pMesh = std::static_pointer_cast<Internal::IMesh>(pResource);

        if (!m_FreeMeshIndices.empty())
        {
          Index = m_FreeMeshIndices.front();
          m_FreeMeshIndices.pop();

          m_pMeshes[Index] = pMesh;
        }

        else
        {
          m_pMeshes.push_back(pMesh);

          Index = static_cast<uint32_t>(m_pMeshes.size() - 1);
        }

        Resource.Index = Index;

        break;
      }

      case Internal::ResourceType::Texture:
      {
        std::shared_ptr<Internal::ITexture>
          pTexture = std::static_pointer_cast<Internal::ITexture>(pResource);

        if (!m_FreeTextureIndices.empty())
        {
          Index = m_FreeTextureIndices.front();
          m_FreeTextureIndices.pop();

          m_pTextures[Index] = pTexture;
        }

        else
        {
          m_pTextures.push_back(pTexture);

          Index = static_cast<uint32_t>(m_pTextures.size() - 1);
        }

        Resource.Index = Index;

        break;
      }

      /* case Internal::ResourceType::Other: {...} */

      default:
      {
        return Resource;
      }
    }

    m_ResourceMap.emplace(Path, Resource);

    return Resource;
  }

  void AssetManager::DeleteResource(const std::string& Path) {
    auto it = m_ResourceMap.find(Path);
    if (it == m_ResourceMap.end())
    {
      return;
    }

    Internal::ResourceDesc Desc = it->second;

    switch (Desc.Type)
    {
      case Internal::ResourceType::Mesh:
      {
        m_pMeshes[Desc.Index].reset();

        m_FreeMeshIndices.push(Desc.Index);

        break;
      }

      case Internal::ResourceType::Texture:
      {
        m_pTextures[Desc.Index].reset();
        
        m_FreeTextureIndices.push(Desc.Index);
        
        break;
      }

      /* case Internal::ResourceType::Other: {...} */

      default:
      {
        break;
      }
    }

    m_ResourceMap.erase(it);
  }

} //  namespace Rose::Framework