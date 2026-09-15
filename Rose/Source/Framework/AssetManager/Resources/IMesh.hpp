#pragma once

#include "IResource.hpp"

namespace Rose::Framework::Internal
{

  class IMesh : public IResource
  {
   public:
    IMesh() = default;
    ~IMesh() = default;

    virtual ResourceType GetType() const override
    {
      return ResourceType::Mesh;
    }
  };

} //  namespace Rose::Framework::Internal