#pragma once

#include "IResource.hpp"

namespace Rose::Framework::Internal
{

  class ITexture : public IResource
  {
   public:
    ITexture() = default;
    ~ITexture() = default;

    virtual ResourceType GetType() const override
    {
      return ResourceType::Texture;
    }
  };
  
} //  namespace Rose::Framework::Internal