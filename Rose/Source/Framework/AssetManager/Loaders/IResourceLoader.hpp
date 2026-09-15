#pragma once

#include <string>
#include <memory>

#include "Renderer/IRenderer.hpp"
#include "Resources/IResource.hpp"

namespace Rose::Framework::Internal
{

  class IResourceLoader
  {
   public:
    IResourceLoader() = default;
    virtual ~IResourceLoader() = default;

    void Initialize(Rose::Renderer::IRenderer* pRenderer)
    {
      m_pRenderer = pRenderer;
    }

    virtual std::string_view GetPattern() const = 0;
    virtual std::shared_ptr<Internal::IResource> LoadResource(const std::string& Path) = 0;

   protected:
    Rose::Renderer::IRenderer* m_pRenderer = nullptr;
  };

} //  namespace Rose::Framework::Internal