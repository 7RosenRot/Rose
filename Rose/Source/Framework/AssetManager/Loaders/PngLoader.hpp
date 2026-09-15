#pragma once

#include "IResourceLoader.hpp"

namespace Rose::Framework::Internal {

  class PngLoader : public IResourceLoader{
   public:
    PngLoader() = default;
    ~PngLoader() = default;

    std::string_view GetPattern() const override
    {
      return ".png";
    }

    std::shared_ptr<IResource> LoadResource(_In_ const std::string& Path);
  };
  
} //  namespace Rose::Framework::Internal