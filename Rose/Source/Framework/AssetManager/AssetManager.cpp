#include "AssetManager.hpp"

#include "Loaders/ObjLoader.hpp"
#include "Loaders/PngLoader.hpp"

namespace Rose::Framework {

  void AssetManager::Initialize(_In_ const std::shared_ptr<Renderer::IRenderer>& pRenderer)
  {
    m_pRenderer = pRenderer;

    RegistryLoader<Internal::ObjLoader>();
    RegistryLoader<Internal::PngLoader>();
  }

  void AssetManager::Shutdown()
  {
    m_ResourceMap.clear();

    m_Pool.Reset();

    m_pLoaders.clear();
  }

  Internal::RESOURCE_HANDLE AssetManager::UploadResource(_In_ const std::string& Path)
  {
    auto it = m_ResourceMap.find(Path);
    if (it != m_ResourceMap.end())
    {
      return it->second;
    }

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

    Internal::RESOURCE_HANDLE Handle{};

    if (!pResource)
    {
      // LOGGER expected

      return Handle;
    }

    Handle = m_Pool.Push(pResource);

    m_ResourceMap.emplace(Path, Handle);

    return Handle;
  }

  void AssetManager::DeleteResource(const std::string& Path) {
    auto it = m_ResourceMap.find(Path);
    if (it == m_ResourceMap.end())
    {
      return;
    }

    Internal::RESOURCE_HANDLE Handle = it->second;

    m_Pool.Pop(Handle);

    m_ResourceMap.erase(it);
  }

} //  namespace Rose::Framework