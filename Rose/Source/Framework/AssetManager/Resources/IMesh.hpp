#pragma once

#include "IResource.hpp"

namespace Rose::Framework::Internal
{

  class IMesh : public IResource
  {
   public:
    IMesh() = default;
    ~IMesh() = default;

    virtual Type GetType() const override
    {
      return Type::Mesh;
    }
  };

} //  namespace Rose::Framework::Internal