#include "AssetManager.hpp"

namespace Rose::Framework {

  void AssetManager::Initialize(_In_ const std::shared_ptr<Renderer::IRenderer>& pRenderer)
  {
    m_pRenderer = pRenderer;

    RegistryLoader<Internal::ObjLoader>();
    RegistryLoader<Internal::PngLoader>();
  }

  void AssetManager::Shutdown()
  {}

  Internal::ResourceDesc AssetManager::UploadResource(_In_ const std::string& Path)
  {
    auto it = m_ResourceMap.find(Path);
  
    if (it != m_ResourceMap.end())
      {
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
      // Logger expected

      return Resource;
    }

    Resource.Type = pResource->GetType();
    
    Resource.Index = m_ResourcePool.Push(pResource);

    m_ResourceMap.emplace(Path, Resource);

    return Resource;
  }

  void AssetManager::DeleteResource(const std::string& Path) {
    auto it = m_ResourceMap.find(Path);
    if (it == m_ResourceMap.end())
    {
      return;
    }

    Internal::ResourceDesc Resource = it->second;

    m_ResourcePool.Pop(Resource.Index);

    m_ResourceMap.erase(it);
  }

} //  namespace Rose::Framework