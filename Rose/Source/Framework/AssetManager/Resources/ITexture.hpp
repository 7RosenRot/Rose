#pragma once

#include "IResource.hpp"

namespace Rose::Framework::Internal
{

  class ITexture : public IResource
  {
   public:
    ITexture() = default;
    ~ITexture() = default;

    virtual Type GetType() const override
    {
      return Type::Texture;
    }
  };
  
} //  namespace Rose::Framework::Internal