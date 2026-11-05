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

    virtual std::size_t GetVertexCount() const = 0;
    virtual std::size_t GetIndexCount() const = 0;
  };

} //  namespace Rose::Framework::Internal