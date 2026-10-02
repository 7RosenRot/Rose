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

  Internal::RESOURCE_HANDLE AssetManager::UploadResource(_In_ const std::string& Path)
  {
    auto it = m_ResourceMap.find(Path);
  
    if (it != m_ResourceMap.end())
    {
      return it->second;
    }

    Internal::RESOURCE_HANDLE Resource = {};

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

    if (!pResource)
    {
      // LOGGER expected

      return Resource;
    }

    Resource.SetType(pResource->GetType());

    auto [Stage, Index] = m_Pool.Push(pResource);
    Resource.SetStage(Stage);
    Resource.SetIndex(Index);

    m_ResourceMap.emplace(Path, Resource);

    return Resource;
  }

  void AssetManager::DeleteResource(const std::string& Path) {
    auto it = m_ResourceMap.find(Path);
    if (it == m_ResourceMap.end())
    {
      return;
    }

    Internal::RESOURCE_HANDLE Resource = it->second;

    m_Pool.Pop(Resource.GetID());

    m_ResourceMap.erase(it);
  }

} //  namespace Rose::Framework